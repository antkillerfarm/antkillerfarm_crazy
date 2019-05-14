import tensorflow as tf

x = tf.placeholder(tf.float32, shape=[None, 28, 28, 3], name='x')

t1 = tf.space_to_batch_nd(x, [2, 2], [[0, 0], [0, 0]])
t2 = tf.batch_to_space_nd(t1, [2, 2], [[0, 0], [0, 0]])

sess = tf.Session()
sess.run(tf.global_variables_initializer())

c_t1 = tf.contrib.lite.TFLiteConverter.from_session(sess, [x], [t2])
c_t1_lm = c_t1.convert()
open('batch_space.tflite', 'wb').write(c_t1_lm)

sess.close()
