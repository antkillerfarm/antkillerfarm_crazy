import os
import tensorflow as tf

curr_dir = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(curr_dir, 'resize_nearest_neighbor_2.tflite')

flag = False

@tf.function
def train_one_step(X):
    if flag:
        t1 = tf.transpose(X, perm=[0, 2, 3, 1]) # nchw -> nhwc
        t1 = tf.image.resize(t1, [513, 513], method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)
        t1 = tf.transpose(t1, perm=[0, 3, 1, 2]) # nhwc -> nchw
    else:
        t1 = tf.image.resize(X, [513, 513], method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)
    return t1

if flag:
    c_t1 = tf.lite.TFLiteConverter.from_concrete_functions(
        [train_one_step.get_concrete_function(tf.TensorSpec([None, 21, 33, 33],tf.float32))])
else:
    c_t1 = tf.lite.TFLiteConverter.from_concrete_functions(
        [train_one_step.get_concrete_function(tf.TensorSpec([None, 33, 33, 21],tf.float32))])
c_t1_lm = c_t1.convert()
open(path, 'wb').write(c_t1_lm)
