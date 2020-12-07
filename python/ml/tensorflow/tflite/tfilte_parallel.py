import os
import tensorflow as tf

curr_dir = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(curr_dir, 'parallel.tflite')
kernel = tf.random.normal([1, 1, 21, 42])

@tf.function
def train_one_step(X):
    t = tf.transpose(X, perm=[0, 2, 3, 1]) # nchw -> nhwc
    t1 = tf.nn.conv2d(t, kernel, strides=[1, 1, 1, 1], padding='VALID')
    t2 = tf.nn.relu(t)
    t = tf.concat([t1, t2], 3)
    t = tf.transpose(t, perm=[0, 3, 1, 2]) # nhwc -> nchw
    return t

c_t1 = tf.lite.TFLiteConverter.from_concrete_functions(
    [train_one_step.get_concrete_function(tf.TensorSpec([None, 21, 33, 33],tf.float32))])
c_t1_lm = c_t1.convert()
open(path, 'wb').write(c_t1_lm)
