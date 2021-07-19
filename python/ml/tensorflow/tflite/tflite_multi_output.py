import os
import tensorflow as tf
import numpy as np

curr_dir = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(curr_dir, 'multi_output.tflite')

kernel_in = np.random.uniform(-1.0, 1.0, size=[3, 3, 3, 16])
kernel_in2 = np.random.uniform(-1.0, 1.0, size=[3, 3, 16, 32])

@tf.function
def train_one_step(X):
    kernel = tf.constant(kernel_in, dtype=tf.float32)
    kernel2 = tf.constant(kernel_in2, dtype=tf.float32)
    t1 = tf.nn.conv2d(X, kernel, strides=[1, 1, 1, 1], padding='VALID')
    t2 = tf.nn.conv2d(t1, kernel2, strides=[1, 1, 1, 1], padding='VALID')
    return [t1, t2]


c_t1 = tf.lite.TFLiteConverter.from_concrete_functions(
    [train_one_step.get_concrete_function(tf.TensorSpec([1, 224, 224, 3], tf.float32))])

c_t1_lm = c_t1.convert()
open(path, 'wb').write(c_t1_lm)
