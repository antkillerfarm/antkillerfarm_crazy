import tensorflow as tf
import numpy as np

tensor_in = np.random.rand(1, 1, 16384)
np.savetxt("input.tensor", tensor_in.flatten(), fmt='%f')

weight = np.random.rand(128, 1, 512)
np.savetxt("weight.tensor", weight.flatten(), fmt='%f')

bias = np.random.rand(128)
np.savetxt("bias.tensor", bias.flatten(), fmt='%f')

tf.enable_eager_execution()

'''
# test eager mode
a = tf.constant(10)
b = tf.constant(32)
c = a + b
print(c)
'''

tensor_in = tf.reshape(tensor_in, [1, 16384, 1])
print(tensor_in)
weight = tf.transpose(weight, perm=[2, 1, 0])
print(weight)

out = tf.pad(tensor_in, [[0, 0], [128, 128], [0, 0]], 'CONSTANT')
print(out)

out = tf.nn.conv1d(out, weight, stride = 256, padding = 'VALID')
print(out)

out = tf.add(out, bias)
print(out)

out = tf.transpose(out, perm=[0, 2, 1])
print(out)

np.savetxt("output.tensor", out.numpy().flatten(), fmt='%f')
