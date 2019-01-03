import tensorflow as tf 
'''
sample = tf.constant(
    [[[11, 12, 13], [21, 22, 23]],
    [[31, 32, 33], [41, 42, 43]],
    [[51, 52, 53], [61, 62, 63]]])
'''
sample = tf.constant(
    [[[11, 12, 13]],
    [[31, 32, 33]],
    [[51, 52, 53]]])

slice = tf.strided_slice(sample, begin=[0, 0, 0], end=[3, -1, 3], strides=[2, 1, 2])

with tf.Session() as sess:
    print(sess.run(slice))

