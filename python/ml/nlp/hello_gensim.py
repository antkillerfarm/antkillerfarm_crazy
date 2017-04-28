#!/usr/bin/python
# -*- coding: utf-8 -*-

from  gensim.models import KeyedVectors

model = KeyedVectors.load_word2vec_format('/home/tj/big_data/data/talk/2j3s.vec', binary=False) 
model.save_word2vec_format('/home/tj/big_data/data/talk/2j3s.vec.bin', binary=True)

