#!/usr/bin/python
# -*- coding: utf-8 -*-

import tensorflow as tf
hello = tf.constant('Hello, TensorFlow!')
sess = tf.Session()
print(sess.run(hello))

a = tf.constant(10)
b = tf.constant(32)
c = a + b
d = a - b
run_list = [c, d]
print(sess.run(run_list))
