import numpy as np
import tensorflow as tf

path = "/home/ubuser/my/work/hardsigmoid/"

f1 = ['pre_hardsigmoid_npu.txt']
f2 = ['pre_hardsigmoid_npu_quantize.txt']
f3 = ['post_hardsigmoid_npu_quantize.txt']

scale = 0.00228914
zp = 128

n1 = np.loadtxt(path + f1[0], dtype=np.float32)
n2 = np.around(n1 / scale + zp)
np.savetxt(path + f2[0], n2.astype(np.uint8), fmt='%d')

a = tf.constant(n1, dtype = tf.float32)
b = tf.keras.activations.hard_sigmoid(a)
n3 = b.numpy()

scale = 0.005
zp = 128

n4 = np.around(n3 / scale + zp)
np.savetxt(path + f3[0], n4.astype(np.uint8), fmt='%d')
