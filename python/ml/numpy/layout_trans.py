import numpy as np

path = "/media/ubuser/DATA/"

src = "2_ConvolutionLayer_operation_2.txt"
dst = "2_ConvolutionLayer_operation_2_nhwc.txt"

n1 = np.loadtxt(path + src, dtype=np.float32)
n2 = n1.reshape((1, 32, 112, 112))
#n3 = n2.transpose((0, 3, 1, 2)) # nhwc -> nchw
n3 = n2.transpose((0, 2, 3, 1)) # nchw -> nhwc

np.savetxt(path + dst, n3.flatten(), fmt='%f')
