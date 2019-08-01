import tensorflow as tf 

sample = tf.constant(
    [[[11, 12, 13], [21, 22, 23]],
    [[31, 32, 33], [41, 42, 43]],
    [[51, 52, 53], [61, 62, 63]]])

multiples = tf.constant([3,2,1])

t1 = tf.tile(sample, multiples)

with tf.Session() as sess:
    print(sess.run(t1))
