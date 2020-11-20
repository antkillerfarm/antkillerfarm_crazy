import tensorflow as tf

x = tf.placeholder(tf.float32, shape=[None, 16, 16, 3], name='x')
y = tf.placeholder(tf.float32, shape=[None, 16, 16, 3], name='y')
z = tf.placeholder(tf.float32, shape=[None, 16, 16, 3], name='z')

t1 = tf.stack([x, y, z], axis=1)

sess = tf.Session()
sess.run(tf.global_variables_initializer())

c_t1 = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y, z], [t1])
c_t1_lm = c_t1.convert()
open('stack.tflite', 'wb').write(c_t1_lm)

sess.close()
