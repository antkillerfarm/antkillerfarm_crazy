#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

import matplotlib.pyplot as plt

import keras.backend as K
from keras.datasets import mnist
from keras.layers import *
from keras.models import *
from keras.optimizers import *
from keras.initializers import *
from keras.utils.generic_utils import Progbar

GPU = "0"
RUN = '10'
OUT_DIR = 'out/' + RUN
TENSORBOARD_DIR = 'tensorboard/wgans/' + RUN

# latent vector size
Z_SIZE = 100

# number of iterations D is trained for per each G iteration
D_ITERS = 5

EPOCHS = 100
BATCH_SIZE = 100

# use specific GPU
os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
os.environ["CUDA_VISIBLE_DEVICES"] = GPU

if not os.path.isdir(OUT_DIR): os.makedirs(OUT_DIR)

np.random.seed(777)

K.set_image_dim_ordering('tf')

# basically return mean(y_pred),
# but with ability to inverse it for minimization (when y_true == -1)
def wasserstein_dist(y_true, y_pred):
    return K.mean(y_true * y_pred)

def create_D():

    # weights are initlaized from normal distribution with below params
    weight_init = RandomNormal(mean=0., stddev=0.02)

    input_image = Input(shape=(28, 28, 1), name='input_image')

    x = Conv2D(
        32, (3, 3),
        padding='same',
        name='conv_1',
        kernel_initializer=weight_init)(input_image)
    x = LeakyReLU()(x)
    x = MaxPool2D(pool_size=2)(x)
    x = Dropout(0.3)(x)

    x = Conv2D(
        64, (3, 3),
        padding='same',
        name='conv_2',
        kernel_initializer=weight_init)(x)
    x = MaxPool2D(pool_size=1)(x)
    x = LeakyReLU()(x)
    x = Dropout(0.3)(x)

    x = Conv2D(
        128, (3, 3),
        padding='same',
        name='conv_3',
        kernel_initializer=weight_init)(x)
    x = MaxPool2D(pool_size=2)(x)
    x = LeakyReLU()(x)
    x = Dropout(0.3)(x)

    x = Conv2D(
        256, (3, 3),
        padding='same',
        name='coonv_4',
        kernel_initializer=weight_init)(x)
    x = MaxPool2D(pool_size=1)(x)
    x = LeakyReLU()(x)
    x = Dropout(0.3)(x)

    features = Flatten()(x)

    output_is_fake = Dense(
        1, activation='linear', name='output_is_fake')(features)

    output_class = Dense(
        10, activation='softmax', name='output_class')(features)

    return Model(
        inputs=[input_image], outputs=[output_is_fake, output_class], name='D')

def create_G(Z_SIZE=Z_SIZE):
    DICT_LEN = 10
    EMBEDDING_LEN = Z_SIZE

    # weights are initlaized from normal distribution with below params
    weight_init = RandomNormal(mean=0., stddev=0.02)

    # class#
    input_class = Input(shape=(1, ), dtype='int32', name='input_class')
    # encode class# to the same size as Z to use hadamard multiplication later on
    e = Embedding(
        DICT_LEN, EMBEDDING_LEN,
        embeddings_initializer='glorot_uniform')(input_class)
    embedded_class = Flatten(name='embedded_class')(e)

    # latent var
    input_z = Input(shape=(Z_SIZE, ), name='input_z')

    # hadamard product
    h = multiply([input_z, embedded_class], name='h')

    # cnn part
    x = Dense(1024)(h)
    x = LeakyReLU()(x)

    x = Dense(128 * 7 * 7)(x)
    x = LeakyReLU()(x)
    x = Reshape((7, 7, 128))(x)

    x = UpSampling2D(size=(2, 2))(x)
    x = Conv2D(256, (5, 5), padding='same', kernel_initializer=weight_init)(x)
    x = LeakyReLU()(x)

    x = UpSampling2D(size=(2, 2))(x)
    x = Conv2D(128, (5, 5), padding='same', kernel_initializer=weight_init)(x)
    x = LeakyReLU()(x)

    x = Conv2D(
        1, (2, 2),
        padding='same',
        activation='tanh',
        name='output_generated_image',
        kernel_initializer=weight_init)(x)

    return Model(inputs=[input_z, input_class], outputs=x, name='G')

# with tf.device('/gpu:0'):

D = create_D()

D.compile(
    optimizer=RMSprop(lr=0.00005),
    loss=[wasserstein_dist, 'sparse_categorical_crossentropy'])

input_z = Input(shape=(Z_SIZE, ), name='input_z_')
input_class = Input(shape=(1, ),name='input_class_', dtype='int32')

# with tf.device('/gpu:0'):

G = create_G()

# create combined D(G) model
# D.trainable = False
output_is_fake, output_class = D(G(inputs=[input_z, input_class]))
DG = Model(inputs=[input_z, input_class], outputs=[output_is_fake, output_class])
DG.get_layer('D').trainable = False # freeze D in generator training faze

DG.compile(
    optimizer=RMSprop(lr=0.00005),
    loss=[wasserstein_dist, 'sparse_categorical_crossentropy']
)

# get our mnist data, and force it to be of shape (..., 28, 28, 1) with
# range [-1., 1.]
(X_train, y_train), (X_test, y_test) = mnist.load_data()
X_train = (X_train.astype(np.float32) - 127.5) / 127.5
X_train = np.expand_dims(X_train, axis=3)

X_test = (X_test.astype(np.float32) - 127.5) / 127.5
X_test = np.expand_dims(X_test, axis=3)

train_num_samples, test_num_samples = X_train.shape[0], X_test.shape[0]

train_history = defaultdict(list)
test_history = defaultdict(list)

def save_generated_samples(epoch):

    # new batch of z's
    zz = np.random.normal(0., 1., (100, Z_SIZE))

    # new batch of classes [0..9]
    generated_classes = np.array(list(range(0, 10)) * 10, dtype=np.int32)

    generated_images = G.predict([zz, generated_classes.reshape(-1, 1)])

    rr = []
    for c in range(10):
        rr.append(
            np.concatenate(generated_images[c * 10:(1 + c) * 10]).reshape(
                280, 28))
    img = np.hstack(rr)

    plt.imsave(OUT_DIR + '/generated-e%d.png' % epoch, img, cmap=plt.cm.gray)

# fake = 1
# real = -1

for epoch in range(EPOCHS):

    print('\nEpoch {} / {}:'.format(epoch + 1, EPOCHS))

    batches_num = int(train_num_samples / BATCH_SIZE)
    progress_bar = Progbar(target=batches_num)

    epoch_DG_losses = []
    epoch_D1_losses = []
    epoch_D2_losses = []

    for batch_i in range(batches_num):

        if len(epoch_D1_losses) > 0:
            progress_bar.update(
                batch_i,
                values=[
                        ('D1_is_fake', np.mean(epoch_D1_losses[-5:], axis=0)[1]),
                        ('D1_class', np.mean(epoch_D1_losses[-5:], axis=0)[2]),
                        ('D2_is_fake', np.mean(epoch_D2_losses[-5:], axis=0)[1]),
                        ('D2_class', np.mean(epoch_D2_losses[-5:], axis=0)[2]),
                        ('D(G)_is_fake', np.mean(epoch_DG_losses[-5:],axis=0)[1]),
                        ('D(G)_class', np.mean(epoch_DG_losses[-5:],axis=0)[2])
                ]
            )
        else:
            progress_bar.update(batch_i)
            
        # 1: train D on real+generated images
        
        if batch_i < 25 or batch_i % 500 == 0:
            d_iters = 100
        else:
            d_iters = D_ITERS
        
        for d_it in range(d_iters):
            
            D.trainable = True
            for l in D.layers: l.trainable = True

            # clip D weights

            for l in D.layers:
                weights = l.get_weights()
                weights = [np.clip(w, -0.01, 0.01) for w in weights]
                l.set_weights(weights)

            # 1.1: maximize D output on reals === minimize -1*(D(real))
            real_images = X_train[batch_i * BATCH_SIZE:(batch_i + 1) * BATCH_SIZE]
            real_images_classes = y_train[batch_i * BATCH_SIZE:(batch_i + 1) * BATCH_SIZE]
            
            D_loss = D.train_on_batch(real_images, [-np.ones(BATCH_SIZE), real_images_classes])
            epoch_D1_losses.append(D_loss)

            # 1.2: minimize D output on fakes 

            zz = np.random.normal(0., 1., (BATCH_SIZE, Z_SIZE))
            generated_classes = np.random.randint(0, 10, BATCH_SIZE)
            generated_images = G.predict([zz, generated_classes.reshape(-1, 1)])
            
            D_loss = D.train_on_batch(generated_images, [np.ones(BATCH_SIZE), generated_classes])
            epoch_D2_losses.append(D_loss)
            
        # 2: train D(G) (D is frozen)
        # minimize D output while supplying it with fakes, telling it that they are reals (-1)

        D.trainable = False
        for l in D.layers: l.trainable = False
        
        zz = np.random.normal(0., 1., (BATCH_SIZE, Z_SIZE)) 
        generated_classes = np.random.randint(0, 10, BATCH_SIZE)

        DG_loss = DG.train_on_batch(
            [zz, generated_classes.reshape((-1, 1))],
            [-np.ones(BATCH_SIZE), generated_classes])
        
        epoch_DG_losses.append(DG_loss)
    
    save_generated_samples(epoch)



