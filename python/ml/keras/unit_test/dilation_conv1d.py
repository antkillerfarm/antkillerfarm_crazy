#!/usr/bin/python3
# -*- coding: utf-8 -*-

import keras
from keras.models import Sequential
from keras.layers import Conv1D, Dense, Conv2D, AtrousConvolution1D

model = Sequential()
#model.add(Dense(1, input_shape=(1,), activation='linear'))
#model.add(Conv2D(32, kernel_size=(3, 3),
#    activation='relu', input_shape=(28, 28, 1)))
#model.add(Conv1D(64, 2, dilation_rate=2, activation='tanh', input_shape = (128, 32)))
model.add(AtrousConvolution1D(64, 2, atrous_rate=2, activation='tanh', input_shape = (128, 32)))

model.save('dilation_conv1d.h5')

