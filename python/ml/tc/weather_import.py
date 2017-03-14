#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

path = "/home/data/tj/my/tianchi/city_weather/"

for my_file in os.listdir(path):
    os.rename(path + my_file, path + my_file + ".csv")
    
