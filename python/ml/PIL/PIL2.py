from PIL import Image
import numpy as np
import math

def channel_mean(n):
    for i in range(n.shape[0]):
        for j in range(n.shape[1]):
            n[i][j][0] = n[i][j][0] + 104
            n[i][j][1] = n[i][j][1] + 117
            n[i][j][2] = n[i][j][2] + 124
    return n

path1 = 'D:/my/work/xx3/none_quantized/1/input_0.txt'
path2 = 'D:/my/work/xx3/none_quantized/1/input_0.jpg'

n1 = np.loadtxt(path1, dtype=np.float32)
n2 = n1.reshape((3, 224, 224))
n3 = n2.transpose((1,2,0))
n4 = channel_mean(n3)
n5 = np.round(n4)
n6 = np.flip(n5, 2)
im = Image.fromarray(n6.astype('uint8'))
im.save(path2)

print("OK")
