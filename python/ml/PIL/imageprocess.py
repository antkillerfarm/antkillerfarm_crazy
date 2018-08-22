import numpy as np
import tensorflow as tf
from PIL import Image

def save_img(sess, tensor, name):
    tensor0 = tf.transpose(tensor, perm=[1, 2, 0])
    res = sess.run(tensor0)
    tensor1 = tensor0.eval()
    im = Image.fromarray(tensor1.astype('uint8'))
    im.save(name)

'''
im = Image.open("example_image_227x227.jpg")
im1 = im.resize((300,300))
n0 = np.array(im1)
print(n0.shape)
np.savetxt("input_0_out0_300_300_3.tensor", n0.flatten(), fmt='%f')
'''

n1 = np.loadtxt('input_0_out0_300_300_3.tensor', dtype=np.float32)
input0 = n1.reshape((300, 300, 3))
im2 = Image.fromarray(input0.astype('uint8'))
im2.save("2.jpg")

with tf.Session() as sess:
    input1 = tf.transpose(input0, perm=[2, 0, 1])
    res = sess.run(input1)
    np.savetxt("input_0_out0_3_300_300.tensor", input1.eval().flatten(), fmt='%f')
    out_crop = tf.slice(input1, [0, 75, 75], [3, 150, 150])
    res = sess.run(out_crop)
    out_crop0 = out_crop.eval()
    np.savetxt("out_crop0.tensor", out_crop0.flatten(), fmt='%f')
    save_img(sess, out_crop0, "3.jpg")
    out_reverse = tf.reverse(out_crop0, [0])
    res = sess.run(out_reverse)
    out_reverse0 = out_reverse.eval()
    np.savetxt("out_reverse0.tensor", out_reverse0.flatten(), fmt='%f')
    save_img(sess, out_reverse0, "4.jpg")
    mean = [127, 127, 127]
    scale = 1.0 / 128
    out_mean0 = tf.transpose(out_reverse0, perm=[1, 2, 0])
    out_mean1 = (out_mean0 - mean) * scale
    out_mean2 = tf.transpose(out_mean1, perm=[2, 0, 1])
    sess.run(out_mean2)
    np.savetxt("out_mean2.tensor", out_mean2.eval().flatten(), fmt='%f')



