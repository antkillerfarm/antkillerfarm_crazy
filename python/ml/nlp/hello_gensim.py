#!/usr/bin/python
# -*- coding: utf-8 -*-

from  gensim.models import KeyedVectors

model = KeyedVectors.load_word2vec_format('/home/tj/big_data/data/word2vec_weixin/word2vec_c', binary=False) 
model.save_word2vec_format('/home/tj/big_data/data/word2vec_weixin/word2vec_c.bin', binary=True)

