#!/usr/bin/python
# -*- coding: utf-8 -*-

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('/home/data/tj/my/tensorflow/iris/iris.data', header=None)


X = df.iloc[:, [0, 2]].values
plt.scatter(X[:50, 0], X[:50, 1],color='red', marker='o', label='setosa')
plt.scatter(X[50:100, 0], X[50:100, 1],color='blue', marker='x', label='versicolor')
plt.scatter(X[100:150, 0], X[100:150, 1],color='green', marker='+', label='Iris-virginica')
plt.xlabel('petal length')
plt.ylabel('sepal length')
plt.legend(loc='upper left')
plt.show()

