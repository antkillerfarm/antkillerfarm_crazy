import tensorflow as tf

x = tf.placeholder(tf.float32, shape=[None, 16, 16, 3], name='x')
y = tf.placeholder(tf.float32, shape=[None, 16, 16, 3], name='y')
xl = tf.placeholder(tf.bool, shape=[None, 16, 16, 3], name='xl')
yl = tf.placeholder(tf.bool, shape=[None, 16, 16, 3], name='yl')
c = tf.placeholder(tf.float32, shape=[1], name='c')
b = tf.placeholder(tf.bool, shape=[1], name='b')

g = tf.greater(x, y, name='g')
ge = tf.greater_equal(x, y, name='ge')
l = tf.less(x,y, name='l')
le = tf.less_equal(x,y, name='l3')
ne = tf.not_equal(x, y, name='ne')
p = tf.pow(x, y)
fd = tf.floordiv(x, y, name='fd')
lo = tf.logical_or(xl, yl, name='lo')
s = tf.where(b, x, y, name='s')
m = tf.minimum(x, y, name='m')


sess = tf.Session()
sess.run(tf.global_variables_initializer())

c_g = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [g])
c_g_lm = c_g.convert()
open('greater.tflite', 'wb').write(c_g_lm)

c_ge = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [ge])
c_ge_lm = c_ge.convert()
open('greater_equal.tflite', 'wb').write(c_ge_lm)

c_l = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [l])
c_l_lm = c_l.convert()
open('less.tflite', 'wb').write(c_l_lm)

c_le = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [le])
c_le_lm = c_le.convert()
open('less_equal.tflite', 'wb').write(c_le_lm)

c_ne = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [ne])
c_ne_lm = c_ne.convert()
open('not_equal.tflite', 'wb').write(c_ne_lm)

c_p = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [p])
c_p_lm = c_p.convert()
open('power.tflite', 'wb').write(c_p_lm)

c_fd = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [fd])
c_fd_lm = c_fd.convert()
open('floordiv.tflite', 'wb').write(c_fd_lm)

#c_lo = tf.contrib.lite.TFLiteConverter.from_session(sess, [xl, yl], [lo])
#c_lo_lm = c_lo.convert()
#open('logical_or.tflite', 'wb').write(c_lo_lm)

#c_s = tf.contrib.lite.TFLiteConverter.from_session(sess, [b, x, y], [s])
#c_s_lm = c_s.convert()
#open('select.tflite', 'wb').write(c_s_lm)

c_m = tf.contrib.lite.TFLiteConverter.from_session(sess, [x, y], [m])
c_m_lm = c_m.convert()
open('minimum.tflite', 'wb').write(c_m_lm)

sess.close()