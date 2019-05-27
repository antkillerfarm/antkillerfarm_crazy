import tensorflow as tf

x = tf.placeholder(tf.float32, shape=[None, 28, 28, 3], name='x')

t1 = tf.slice(x, [0, 10, 10, 0], [1, 10, 10, 3])

sess = tf.Session()
sess.run(tf.global_variables_initializer())

c_t1 = tf.contrib.lite.TFLiteConverter.from_session(sess, [x], [t1])
c_t1_lm = c_t1.convert()
open('slice.tflite', 'wb').write(c_t1_lm)

sess.close()
