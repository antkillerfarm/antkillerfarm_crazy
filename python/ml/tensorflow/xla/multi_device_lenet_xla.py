"""
the demo which come from:
https://www.jianshu.com/p/6beedc7f83da
"""

import os
# input("pid: " + str(os.getpid()) +", press enter after attached")
import numpy as np
import tensorflow as tf
# from tensorflow.keras.mixed_precision import experimental as mixed_precision
# policy = mixed_precision.Policy('mixed_bfloat16')
# mixed_precision.set_policy(policy)
print("tf verison: " + tf.__version__)
# input("pid: " + str(os.getpid()) +", press enter after set breakpoints")
tf.keras.backend.clear_session()
tf.config.optimizer.set_jit(True) # Start with XLA disabled.
tf.debugging.set_log_device_placement(True)

MODEL_FILE = "lenet.json"
MODEL_DATA_FILE = "lenet.h5"

FULL_TEST=False

def load_data():
  (x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()
  x_train = x_train.reshape(x_train.shape[0], 28, 28, 1)
  x_test = x_test.reshape(x_test.shape[0], 28, 28, 1)
  x_train = x_train.astype('float32') / 255
  x_test = x_test.astype('float32') / 255

  # Convert class vectors to binary class matrices.
  y_train = tf.keras.utils.to_categorical(y_train, num_classes=10)
  y_test = tf.keras.utils.to_categorical(y_test, num_classes=10)
  return ((x_train, y_train), (x_test, y_test))

(x_train, y_train), (x_test, y_test) = load_data()

BATCH_SIZE = 16
DUMP_DIR = "npu"

if FULL_TEST:
  EPOCHS = 13
else:
  EPOCHS = 2
  TRAIN_SIZE = BATCH_SIZE
  x_train = x_train[0:TRAIN_SIZE, :, :, :]
  y_train = y_train[0:TRAIN_SIZE, :]

  x_test = x_test[0:2,:,:,:]
  y_test = y_test[0:2,:]

strategy = tf.distribute.MirroredStrategy(cross_device_ops=tf.distribute.ReductionToOneDevice())

def generate_model():
  return tf.keras.models.Sequential([
    tf.keras.layers.Conv2D(6, kernel_size=(5, 5), activation='relu',
      input_shape=x_train.shape[1:]),
    # tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    # tf.keras.layers.Conv2D(16, kernel_size=(5, 5), activation='relu'),
    # tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    tf.keras.layers.Flatten(),
    # tf.keras.layers.Dense(120, activation='relu'),
    # tf.keras.layers.Dense(84, activation='relu'),
    tf.keras.layers.Dense(10, activation='softmax', dtype='float32'),
  ])

def compile_model(model):
  opt = tf.keras.optimizers.SGD(lr=0.0001)
  # opt = tf.keras.optimizers.RMSprop(lr=0.0001, decay=1e-6)
  model.compile(loss='categorical_crossentropy',
                optimizer=opt,
                # run_eagerly=True,
                metrics=['accuracy'])
  return model

def train_model(model, x_train, y_train, x_test,
  y_test, epochs=1, batch_size=1):
  model.fit(x_train, y_train, batch_size=batch_size, epochs=epochs,
    validation_data=(x_test, y_test), shuffle=False)

def warmup(model, x_train, y_train, x_test, y_test):
  # Warm up the JIT, we do not wish to measure the compilation time.
  initial_weights = model.get_weights()
  train_model(model, x_train, y_train, x_test, y_test, epochs=1)
  model.set_weights(initial_weights)

with strategy.scope():
    if os.path.exists(MODEL_FILE):
        json_string = open(MODEL_FILE, 'r').read() 
        model = tf.keras.models.model_from_json(json_string)
        model.load_weights(MODEL_DATA_FILE)
    else:
        model = tf.keras.models.Sequential([
        tf.keras.layers.Conv2D(6, kernel_size=(5, 5), activation='relu',
        input_shape=x_train.shape[1:]),
        tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
        tf.keras.layers.Conv2D(16, kernel_size=(5, 5), activation='relu'),
        tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
        tf.keras.layers.Flatten(),
        tf.keras.layers.Dense(120, activation='relu'),
        tf.keras.layers.Dense(84, activation='relu'),
        tf.keras.layers.Dense(10, activation='softmax', dtype='float32'),
    ])
model = compile_model(model)
model.summary()

train_model(model, x_train, y_train, x_test, y_test,
  epochs=EPOCHS, batch_size=BATCH_SIZE)

print("RRR : job finish.")
