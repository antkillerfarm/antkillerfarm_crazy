import tensorflow as tf

tf.compat.v1.disable_v2_behavior()
x = tf.compat.v1.placeholder(tf.float32, shape=[None, 28, 28, 3], name='x')

t1 = tf.slice(x, [0, 10, 10, 0], [1, 10, 10, 3])

sess = tf.compat.v1.Session()
sess.run(tf.compat.v1.global_variables_initializer())

c_t1 = tf.compat.v1.lite.TFLiteConverter.from_session(sess, [x], [t1])
c_t1_lm = c_t1.convert()
open('slice.tflite', 'wb').write(c_t1_lm)

sess.close()
