import tensorflow as tf 

sample = tf.constant(
    [[[11, 12, 13], [21, 22, 23]],
    [[31, 32, 33], [41, 42, 43]],
    [[51, 52, 53], [61, 62, 63]]])

indices = tf.constant(
    [[0, 1], [1, 0]])

gather = tf.gather(sample, indices)

with tf.Session() as sess:
    print(sess.run(gather))
