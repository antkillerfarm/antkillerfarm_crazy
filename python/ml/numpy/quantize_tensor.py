import numpy as np

path = "/home/ubuser/my/work/hardsigmoid/"

f1 = ['pre_hardsigmoid_npu.txt']
f2 = ['pre_hardsigmoid_npu_quantize.txt']

scale = 0.00228914
zp = 128

n1 = np.loadtxt(path + f1[0], dtype=np.float32)
n2 = np.around(n1 / scale + zp).astype(np.uint8)
np.savetxt(path + f2[0], n2, fmt='%d')
