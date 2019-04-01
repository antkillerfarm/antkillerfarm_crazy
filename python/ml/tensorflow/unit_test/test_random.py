import tensorflow as tf 

def test1():
    t1 = tf.random.uniform([100])

    with tf.Session() as sess:
        print(sess.run(t1))

def test2():
    #t1 = tf.constant([0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0])
    #t1 = tf.reshape(t1, [2, 5])
    t1 = tf.log([[1., 10.]])
    t2 = tf.random.categorical(t1, 5)

    with tf.Session() as sess:
        print(sess.run(t1))
        print(sess.run(t2))

test2()

