import tensorflow
main_version = tensorflow.__version__.split('.')[0]
if int(main_version) == 2:
    import tensorflow.compat.v1 as tf
    tf.compat.v1.disable_v2_behavior()
    import tensorflow.compat.v1.lite as tflite
else:
    import tensorflow as tf
    import tensorflow.contrib.lite as tflite

x = tf.placeholder(tf.float32, shape=[None, 28, 28, 3], name='x')

t1 = tf.slice(x, [0, 10, 10, 0], [1, 10, 10, 3])

sess = tf.Session()
sess.run(tf.global_variables_initializer())

c_t1 = tflite.TFLiteConverter.from_session(sess, [x], [t1])
c_t1_lm = c_t1.convert()
open('slice.tflite', 'wb').write(c_t1_lm)

sess.close()
