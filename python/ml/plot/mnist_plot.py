#!/usr/bin/python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt

from keras.datasets import mnist

# load mnist data
(X_train, y_train), (X_test, y_test) = mnist.load_data()

img = X_train[0]
plt.imsave('xxx.png', img, cmap=plt.cm.gray)

c = 0
img = np.concatenate(X_train[c * 10:(1 + c) * 10])
plt.imsave('xxx1.png', img, cmap=plt.cm.gray)

img = img.reshape(280, 28)
plt.imsave('xxx2.png', img, cmap=plt.cm.gray)

img = img.T
plt.imsave('xxx3.png', img, cmap=plt.cm.gray)

X_train0 = []
for c in range(10):
    X_train0.append(X_train[c].T)

c = 0
img = np.concatenate(X_train0[c * 10:(1 + c) * 10]).T
plt.imsave('xxx4.png', img, cmap=plt.cm.gray)


X_train1 = []
for i in range(10):
    X_train0 = []
    for c in range(10):
        X_train0.append(X_train[c + i * 10].T)
    X_train1.append(np.concatenate(X_train0[0 : 10]).T)

img = np.concatenate(X_train1[0 : 10])
plt.imsave('xxx5.png', img, cmap=plt.cm.gray)