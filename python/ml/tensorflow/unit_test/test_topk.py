import tensorflow as tf 

sample = tf.constant(
    [[[11, 12, 13], [21, 22, 23]],
    [[31, 32, 33], [41, 42, 43]],
    [[51, 52, 53], [61, 62, 63]]])

values,  indices = tf.math.top_k(sample, k=2)

with tf.Session() as sess:
    print(sess.run(values))
    print(sess.run(indices))
