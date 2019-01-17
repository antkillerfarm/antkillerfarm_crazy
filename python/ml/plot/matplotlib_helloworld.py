#!/usr/bin/python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

plt.figure(1)
plt.figure(2)

ax1 = plt.subplot(211)
ax2 = plt.subplot(212)
x = np.linspace(0, 3, 100)

for i in range(5):
    plt.figure(1)
    plt.plot(x, np.exp(i*x/3))
    plt.sca(ax1)
    plt.plot(x, np.sin(i*x))
    plt.sca(ax2)
    plt.plot(x, np.cos(i*x))

fig = plt.figure(3)
ax3d=Axes3D(fig)

x = np.arange(-2,2.0,0.1)
y = np.arange(-2,2.0,0.1)

def f(x,y):
    return x ** 2 + y ** 2

x,y = np.meshgrid(x,y)

ax3d.plot_surface(x,y,f(x,y),rstride=1,cstride=1,cmap=plt.cm.spring)

plt.show()

