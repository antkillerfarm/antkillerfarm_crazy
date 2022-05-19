import tensorflow as tf
import numpy as np


def test_scatter():
    tensor = [0, 0, 0, 0, 0, 0, 0, 0]
    indices = [[1], [3], [4], [7]]
    updates = [9, 10, 11, 12]
    print(tf.tensor_scatter_nd_update(tensor, indices, updates))

    shape = tf.constant([8])
    scatter = tf.scatter_nd(indices, updates, shape)
    print(scatter)

    indices = tf.constant([[0], [2]])
    updates = tf.constant([[[5, 5, 5, 5], [6, 6, 6, 6],
                            [7, 7, 7, 7], [8, 8, 8, 8]],
                           [[5, 5, 5, 5], [6, 6, 6, 6],
                            [7, 7, 7, 7], [8, 8, 8, 8]]])
    shape = tf.constant([4, 4, 4])
    scatter = tf.scatter_nd(indices, updates, shape)
    print(scatter)


def test_max_pool_with_argmax_without_overlap():
    # windows without overlap
    input = tf.constant([7, 2, 5, 3, 10, 2, 3, 8, 9, 3, 4, 2,
                         1, 5, 7, 5, 6, 1, 0, 6, 2, 7, 2, 8]
                        )
    input = tf.reshape(input, [1, 4, 6, 1])
    print(input)
    ksize = [2, 3]
    strides = [2, 3]
    padding = "VALID"

    output, argmax = tf.nn.max_pool_with_argmax(
        input, ksize, strides, padding)

    print(output)
    print(argmax)


def test_max_pool_with_argmax_with_overlap():
    # windows with overlap
    input = tf.constant([7, 2, 5, 3, 8, 3, 8, 9, 3, 4,
                         1, 5, 7, 5, 6, 0, 6, 2, 10, 2]
                        )
    input = tf.reshape(input, [1, 4, 5, 1])
    print(input)
    ksize = [2, 3]
    strides = [2, 2]
    padding = "VALID"

    output, argmax = tf.nn.max_pool_with_argmax(
        input, ksize, strides, padding)

    print(output)
    print(argmax)


def test_max_pool_grad_without_overlap():
    input = tf.constant([7, 2, 5, 3, 10, 2, 3, 8, 9, 3, 4, 2,
                         1, 5, 7, 5, 6, 1, 0, 6, 2, 7, 2, 8]
                        )
    input = tf.reshape(input, [1, 4, 6, 1])
    ksize = [2, 3]
    strides = [2, 3]
    padding = "VALID"

    output, argmax = tf.nn.max_pool_with_argmax(
        input, ksize, strides, padding)
    print(argmax)

    tensor = tf.zeros([24], tf.int32)
    updates = tf.constant([2, 6, 3, 1])
    # updates = tf.reshape(updates, [1, 2, 2, 1])
    argmax = tf.reshape(argmax, [4, 1])
    scatter = tf.tensor_scatter_nd_add(tensor, argmax, updates)
    scatter = tf.reshape(scatter, [1, 4, 6, 1])
    print(scatter)


def test_max_pool_grad_with_overlap():
    input = tf.constant([7, 2, 5, 3, 8, 3, 8, 9, 3, 4,
                         1, 5, 7, 5, 6, 0, 6, 2, 10, 2]
                        )
    input = tf.reshape(input, [1, 4, 5, 1])
    ksize = [2, 3]
    strides = [2, 2]
    padding = "VALID"

    output, argmax = tf.nn.max_pool_with_argmax(
        input, ksize, strides, padding)
    print(argmax)

    tensor = tf.zeros([20], tf.int32)
    updates = tf.constant([2, 6, 3, 1])
    # updates = tf.reshape(updates, [1, 2, 2, 1])
    argmax = tf.reshape(argmax, [4, 1])
    scatter = tf.tensor_scatter_nd_add(tensor, argmax, updates)
    scatter = tf.reshape(scatter, [1, 4, 5, 1])
    print(scatter)

def test_max_pool_grad_with_overlap2():
    input = tf.constant([7, 2, 5, 3, 8, 3, 8, 9, 3, 4,
                         1, 5, 7, 5, 6, 0, 6, 2, 10, 2]
                        )
    input = tf.reshape(input, [1, 4, 5, 1])
    ksize = [2, 3]
    strides = [2, 2]
    padding = "VALID"

    output, argmax = tf.nn.max_pool_with_argmax(
        input, ksize, strides, padding)
    print(argmax)

    updates = tf.constant([2, 6, 3, 1])
    # updates = tf.reshape(updates, [1, 2, 2, 1])
    argmax = tf.reshape(argmax, [4, 1])
    shape = tf.constant([20], tf.int64)
    scatter = tf.scatter_nd(argmax, updates, shape)
    scatter = tf.reshape(scatter, [1, 4, 5, 1])
    print(scatter)

test_max_pool_grad_with_overlap2()
