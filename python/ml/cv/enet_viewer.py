import numpy as np
import math
import cv2

def generate_color(num):
    out = np.zeros((num, 3), dtype=np.uint8)
    n = int(math.ceil(math.log(num, 3)))
    delta = int(math.floor(255 / n))
    for i in range(num):
        num0 = i
        out[i][0] = (num0 % n) * delta
        num0 = math.floor(num0 / n)
        out[i][1] = (num0 % n) * delta
        num0 = math.floor(num0 / n)
        out[i][2] = (num0 % n) * delta
    return out

def find_max_class(n):
    out = np.zeros((n.shape[1], n.shape[2]))
    for i in range(n.shape[2]):
        for j in range(n.shape[1]):
            max_index = -1
            max = 0.0
            for k in range(n.shape[0]):
                if max_index == -1:
                    max = n[k][j][i]
                    max_index = k
                else:
                    if n[k][j][i] > max:
                        max = n[k][j][i]
                        max_index = k
            out[j][i] = max_index
    return out

def output_img(img, color):
    out = np.zeros((img.shape[0], img.shape[1], 3), dtype=np.uint8)
    for i in range(img.shape[1]):
        for j in range(img.shape[0]):
            pixel_class = int(math.floor(img[j][i]))
            out[j][i][0] = color[pixel_class][0]
            out[j][i][1] = color[pixel_class][1]
            out[j][i][2] = color[pixel_class][2]
    cv2.imwrite('./2.jpg', out)

path='./'
filename='deconv6_0_0_209_out0.tensor'

color = generate_color(19)

#img = cv2.imread("./1.jpg")

n1 = np.loadtxt(path + filename, dtype=np.float32)
n2 = n1.reshape((19,512,1024))

n3 = find_max_class(n2)
output_img(n3, color)

