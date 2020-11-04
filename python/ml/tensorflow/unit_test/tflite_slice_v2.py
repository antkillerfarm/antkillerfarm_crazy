import tensorflow as tf

@tf.function
def train_one_step(X):
    t1 = tf.slice(X, [0, 10, 10, 0], [1, 10, 10, 3])
    return t1

c_t1 = tf.lite.TFLiteConverter.from_concrete_functions(
    [train_one_step.get_concrete_function(tf.TensorSpec([None, 28, 28, 3],tf.float32))])
c_t1_lm = c_t1.convert()
open('slice.tflite', 'wb').write(c_t1_lm)
