import numpy as np
import tensorflow as tf

i_fl = -1
o_fl = -3
w_fl = 7
b_fl = i_fl + w_fl
height = 28
width = 28
channel = 192
weights = 32
ksize = 1
stride = 1
pad = 0
p_size = 3
p_stride = 1
sess = tf.Session()


i = np.loadtxt("input.txt", dtype = np.float32)
#i = np.reshape(i, [1, 64, 56, 56])
# channel width height
i = np.reshape(i, [1, channel, height, width])
i = np.transpose(i, [0, 2, 3, 1])

# i = np.fromfile("input.bin", dtype = np.int8)
# # i = np.fromfile("input.bin", dtype = np.float16)
# i = np.reshape(i, [1, channel, height, width])
# i = np.transpose(i, [0, 2, 3, 1]).astype(np.float32)


'''
w = np.loadtxt("weight.txt", dtype = np.float32);
w = np.reshape(w, [weights, channel, ksize, ksize])
w = np.transpose(w, [2, 3, 1, 0])
w /= np.power(float(2), w_fl)

'''
#w = np.load("weight.data")
w = np.fromfile("weight.data", dtype = np.int8)
w = np.reshape(w, [weights, channel, ksize, ksize])
w = np.transpose(w, [2, 3, 1, 0]).astype(np.float32)
w.tofile("weight_origin.txt", '\n')
ofst = np.power(float(2), 0 - w_fl)
dmax = (np.power(float(2), 7) - 1)
dmin = -(np.power(float(2), 7))
#w = tf.divide(w, ofst)
#w = tf.rint(w)
w = tf.maximum(tf.minimum(w, dmax.astype(np.float32)), dmin.astype(np.float32))
w = tf.multiply(w, ofst)
ow = sess.run(w)
ow.tofile("weight_qnt.txt", '\n')


'''
b = np.loadtxt("bias.txt", dtype = np.float32)
b = np.reshape(b, [channel])
b /= np.power(float(2), b_fl)
'''
#b = np.load("bias.data")
b = np.fromfile("bias.data", dtype = np.int32).astype(np.float32)
ofst = np.power(float(2), 0 - b_fl)
dmax = (np.power(float(2), 26) - 1)
dmin = -(np.power(float(2), 26))
#b = tf.divide(b, ofst)
#b = tf.rint(b)
b = tf.maximum(tf.minimum(b, dmax.astype(np.float32)), dmin.astype(np.float32))
b = tf.multiply(b, ofst)
ob = sess.run(b)
ob.tofile("bias_qnt.txt", '\n')

ofst = np.power(float(2), 0 - i_fl)
dmax = (np.power(float(2), 7) - 1)
dmin = -(np.power(float(2), 7))

i = tf.divide(i, np.power(float(2), 0 - i_fl))
i = tf.rint(i)
i = tf.maximum(tf.minimum(i, dmax.astype(np.float32)), dmin.astype(np.float32))
# input save
# ini = sess.run(i)
# ini = ini.astype(np.int8)
# ini.tofile("i.bin")
i = tf.multiply(i, np.power(float(2), 0 - i_fl))
i = tf.pad(i, [[0,0], [pad,pad],[pad,pad], [0,0]], "CONSTANT")

out = tf.nn.max_pool(i, [1,p_size,p_size,1], [1,p_stride,p_stride,1], padding = 'SAME')
out = tf.nn.conv2d(out, w, strides = [1,stride,stride,1], padding = 'VALID')
# out = tf.nn.conv2d(i, w, strides = [1,stride,stride,1], padding = 'VALID')
out = tf.add(out, b)
out = tf.divide(out, np.power(float(2), 0 - o_fl))
out = tf.rint(out)
out = tf.maximum(tf.minimum(out, dmax.astype(np.float32)), dmin.astype(np.float32))
# out = tf.multiply(out, np.power(float(2), 0 - o_fl))
out = tf.nn.relu(out)
# out = tf.nn.max_pool(out, [1,p_size,p_size,1], [1,p_stride,p_stride,1], padding = 'VALID')

o = sess.run(out)

o = np.transpose(o, [0, 3, 1, 2])
# print(o.dtype)
o.tofile("o.txt", '\n')
# o.tofile("o.bin")
o = o.astype(np.int8, copy=False)
o.tofile("o.bin")


'''
o2 = np.loadtxt("output.txt", dtype = np.float32)
#o2 = np.reshape(o2, [1,weights, height, width])
o2 = np.reshape(o2, o.shape)
res = o - o2
res.tofile('res.txt','\n')
res = np.where(res != 0)
print(res)
'''

