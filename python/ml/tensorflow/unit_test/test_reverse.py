import tensorflow as tf
import numpy as np


def test_reverse():
    input = np.arange(0, 60, 1)
    input = tf.constant(input)
    input = tf.reshape(input, [2, 3, 2, 5])
    print(input)
    output = tf.reverse(input, [0, 1, 2, 3])
    print(output)
    output = tf.reverse(input, [1, 2, 3])
    print(output)
    output = tf.reverse(input, [2, 3])
    print(output)
    output = tf.reverse(input, [0, 1])
    print(output)

    t0 = tf.reshape(input, [1, 2, 3, 2 * 5])
    output = tf.reverse(t0, [1, 2])
    output = tf.reshape(output, [2, 3, 2, 5])
    print(output)


test_reverse()
