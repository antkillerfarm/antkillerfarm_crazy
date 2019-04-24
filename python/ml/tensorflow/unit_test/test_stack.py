import tensorflow as tf 

x = tf.constant([[11, 12, 13], [21, 22, 23]])
y = tf.constant([[31, 32, 33], [41, 42, 43]])
z = tf.constant([[51, 52, 53], [61, 62, 63]])

t1 = tf.stack([x, y, z], axis=1)

x = tf.constant(
    [[[11, 12, 13], [21, 22, 23]],
    [[31, 32, 33], [41, 42, 43]],
    [[51, 52, 53], [61, 62, 63]]])
print(x)

y = tf.constant(
    [[[71, 72, 73], [81, 82, 83]],
    [[91, 92, 93], [101, 102, 103]],
    [[111, 112, 53], [121, 122, 123]]])
print(y)

t2 = tf.stack([x, y], axis=1)
print(t2)

with tf.Session() as sess:
    print(sess.run(t2))
