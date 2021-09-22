import os
import tensorflow as tf
import numpy as np

curr_dir = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(curr_dir, 'multi_output_quant_x.tflite')

kernel_in = np.random.uniform(-1.0, 1.0, size=[3, 3, 3, 16])
kernel_in2 = np.random.uniform(-1.0, 1.0, size=[3, 3, 16, 32])

@tf.function
def train_one_step(X):
    kernel = tf.constant(kernel_in, dtype=tf.float32)
    kernel2 = tf.constant(kernel_in2, dtype=tf.float32)
    t1 = tf.nn.conv2d(X, kernel, strides=[1, 1, 1, 1], padding='VALID')
    t2 = tf.nn.conv2d(t1, kernel2, strides=[1, 1, 1, 1], padding='VALID')
    # return [t1, t2]
    return t2


converter = tf.lite.TFLiteConverter.from_concrete_functions(
    [train_one_step.get_concrete_function(tf.TensorSpec([1, 224, 224, 3], tf.float32))])
converter.optimizations = [tf.lite.Optimize.DEFAULT]
def representative_dataset_gen():
  # Get sample input data as a numpy array in a method of your choosing.
  input_in = np.random.uniform(-1.0, 1.0, size=[1, 224, 224, 3])
  input = tf.constant(input_in, dtype=tf.float32)
  yield [input]
converter.representative_dataset = representative_dataset_gen
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.uint8  # or tf.uint8
converter.inference_output_type = tf.uint8  # or tf.uint8

c_t1_lm = converter.convert()
open(path, 'wb').write(c_t1_lm)
