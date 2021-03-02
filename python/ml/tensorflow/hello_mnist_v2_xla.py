#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import tensorflow as tf

from tensorflow.python.client import device_lib
from tensorflow.profiler.experimental import Profile

#input("pid: " + str(os.getpid()) +", press enter after attached")
#input("pid: " + str(os.getpid()) +", press enter after set breakpoints")

#print(device_lib.list_local_devices())
fit_flag = True
current_dir = os.path.dirname(os.path.abspath(__file__))

def test():
  mnist = tf.keras.datasets.mnist

  (x_train, y_train), (x_test, y_test) = mnist.load_data()
  x_train, x_test = x_train / 255.0, x_test / 255.0

  model = tf.keras.models.Sequential([
    tf.keras.layers.Flatten(input_shape=(28, 28)),
    tf.keras.layers.Dense(128, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(10)
  ])

  predictions = model(x_train[:1]).numpy()

  tf.nn.softmax(predictions).numpy()

  loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

  loss_fn(y_train[:1], predictions).numpy()

  model.compile(optimizer='adam',
                loss=loss_fn,
                metrics=['accuracy'])

  if fit_flag:
    with Profile(current_dir + '/logdir_path1'):
      model.fit(x_train, y_train, epochs=1)

    model.evaluate(x_test,  y_test, verbose=2)

    probability_model = tf.keras.Sequential([
      model,
      tf.keras.layers.Softmax()
    ])

    probability_model(x_test[:5])

def test1():
  mnist = tf.keras.datasets.mnist

  (x_train, y_train), (x_test, y_test) = mnist.load_data()
  x_train, x_test = x_train / 255.0, x_test / 255.0

  with tf.device("/device:XLA_CPU:0"):
    model = tf.keras.models.Sequential([
      tf.keras.layers.Flatten(input_shape=(28, 28)),
      tf.keras.layers.Dense(128, activation='relu'),
      tf.keras.layers.Dropout(0.2),
      tf.keras.layers.Dense(10)
    ])

  predictions = model(x_train[:1]).numpy()

  tf.nn.softmax(predictions).numpy()

  loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

  loss_fn(y_train[:1], predictions).numpy()

  model.compile(optimizer='adam',
                loss=loss_fn,
                metrics=['accuracy'])

  if fit_flag:
    model.fit(x_train, y_train, epochs=1)

    model.evaluate(x_test,  y_test, verbose=2)

    probability_model = tf.keras.Sequential([
      model,
      tf.keras.layers.Softmax()
    ])

    probability_model(x_test[:5])

def test2():
  mnist = tf.keras.datasets.mnist

  (x_train, y_train), (x_test, y_test) = mnist.load_data()
  x_train, x_test = x_train / 255.0, x_test / 255.0

  with tf.device("/device:XLA_CPU:0"):
    model = tf.keras.models.Sequential([
      tf.keras.layers.Flatten(input_shape=(28, 28)),
      tf.keras.layers.Dense(128, activation='relu'),
      tf.keras.layers.Dropout(0.2),
      tf.keras.layers.Dense(10)
    ])

    predictions = model(x_train[:1]).numpy()

    tf.nn.softmax(predictions).numpy()

    loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

    loss_fn(y_train[:1], predictions).numpy()

    model.compile(optimizer='adam',
                  loss=loss_fn,
                  metrics=['accuracy'])

    if fit_flag:
      model.fit(x_train, y_train, epochs=1)

      model.evaluate(x_test,  y_test, verbose=2)

      probability_model = tf.keras.Sequential([
        model,
        tf.keras.layers.Softmax()
      ])

      probability_model(x_test[:5])

@tf.function
def train_one_step(X, Y, Z):
  with tf.xla.experimental.jit_scope(compile_ops=True):
    t1 = tf.add(X, Y)
    t1 = tf.multiply(t1, Z)
    return t1

def test3():
  mnist = tf.keras.datasets.mnist

  (x_train, _), (x_test, _) = mnist.load_data()
  x_train, x_test = x_train / 255.0, x_test / 255.0
  with Profile(current_dir + '/logdir_path1'):
    res = train_one_step(x_train[0:100], x_train[100:200], x_train[200:300])

#test()
#test1()
#test2()
test3()
