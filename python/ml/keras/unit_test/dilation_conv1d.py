#!/usr/bin/python
# -*- coding: utf-8 -*-

import keras
from keras.models import Sequential
from keras.layers import Dense
import numpy as np


model = Sequential()
model.add(Dense(1, input_shape=(1,), activation='linear'))

#model.compile(optimizer='sgd', loss='mse')
#model.fit(trX, trY, nb_epoch=200, verbose=1)

model.save('linear_regression_model.h5')

