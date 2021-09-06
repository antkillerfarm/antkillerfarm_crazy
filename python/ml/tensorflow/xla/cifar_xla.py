"""
the demo which come from:
https://tensorflow.google.cn/xla/tutorials/autoclustering_xla
"""

import os
input("pid: " + str(os.getpid()) +", press enter after attached")
import tensorflow as tf
#input("pid: " + str(os.getpid()) +", press enter after set breakpoints")
tf.keras.backend.clear_session()
tf.config.optimizer.set_jit(False)

MODEL_FILE = "cifar.json"
MODEL_DATA_FILE = "cifar.h5"

def load_data():
  (x_train, y_train), (x_test, y_test) = tf.keras.datasets.cifar10.load_data()
  x_train = x_train.astype('float32') / 256
  x_test = x_test.astype('float32') / 256

  # Convert class vectors to binary class matrices.
  y_train = tf.keras.utils.to_categorical(y_train, num_classes=10)
  y_test = tf.keras.utils.to_categorical(y_test, num_classes=10)
  return ((x_train, y_train), (x_test, y_test))

(x_train, y_train), (x_test, y_test) = load_data()

x_train = x_train[0:1,:,:,:]
y_train = y_train[0:1,:]

x_test = x_test[0:1,:,:,:]
y_test = y_test[0:1,:]

def generate_model():
  return tf.keras.models.Sequential([
    tf.keras.layers.Conv2D(32, (3, 3), padding='same', input_shape=x_train.shape[1:]),
    tf.keras.layers.Activation('relu'),
    tf.keras.layers.Conv2D(32, (3, 3)),
    tf.keras.layers.Activation('relu'),
    tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    #tf.keras.layers.Dropout(0.25),

    tf.keras.layers.Conv2D(64, (3, 3), padding='same'),
    tf.keras.layers.Activation('relu'),
    tf.keras.layers.Conv2D(64, (3, 3)),
    tf.keras.layers.Activation('relu'),
    tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    #tf.keras.layers.Dropout(0.25),

    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(512),
    tf.keras.layers.Activation('relu'),
    #tf.keras.layers.Dropout(0.5),
    tf.keras.layers.Dense(10),
    tf.keras.layers.Activation('softmax')
  ])

def compile_model(model):
  opt = tf.keras.optimizers.SGD(lr=0.0001)
  model.compile(loss='categorical_crossentropy',
                optimizer=opt,
                metrics=['mse'])
  return model

def train_model(model, x_train, y_train, x_test, y_test, epochs=1):
  model.fit(x_train, y_train, batch_size=1, epochs=epochs, validation_data=(x_test, y_test), shuffle=False)

def warmup(model, x_train, y_train, x_test, y_test):
  # Warm up the JIT, we do not wish to measure the compilation time.
  initial_weights = model.get_weights()
  train_model(model, x_train, y_train, x_test, y_test, epochs=1)
  model.set_weights(initial_weights)

if os.path.exists(MODEL_FILE):
  json_string = open(MODEL_FILE, 'r').read() 
  model = tf.keras.models.model_from_json(json_string)
  model.load_weights(MODEL_DATA_FILE)
else:
  model = generate_model()

model = compile_model(model)

# warmup(model, x_train, y_train, x_test, y_test)
# initial_weights = model.get_weights()
# train_model(model, x_train, y_train, x_test, y_test)
# model.set_weights(initial_weights)
train_model(model, x_test, y_test, x_test, y_test)

if not os.path.exists(MODEL_FILE):
  json_string = model.to_json()
  open(MODEL_FILE, 'w').write(json_string) 
  model.save_weights(MODEL_DATA_FILE)
  print("RRR : save model.")

print("RRR : job finish.")
