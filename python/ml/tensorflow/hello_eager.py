from __future__ import absolute_import, division, print_function
import tensorflow as tf

tf.enable_eager_execution()
tf.executing_eagerly()        # => True

x = [[2.]]
m = tf.matmul(x, x)
print("hello, {}".format(m))  # => "hello, [[4.]]"
