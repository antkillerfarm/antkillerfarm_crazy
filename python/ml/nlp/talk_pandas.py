#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import pandas as pd  
import numpy as np
import nltk
import jieba

df = pd.read_csv('/home/tj/big_data/data/talk/c1.csv', header=None)
fp2 = codecs.open("/home/tj/big_data/data/talk/c2.txt", "w", "utf-8")

#df2 = df[1].groupby(df[1]).count()
#df3 = df2.sort_values(ascending=False)

#for (key, value) in df3.iteritems():
#    if (key.find("发货") != -1):
#        print (key + ":" + str(value))

text = ""
len_df = len(df)
for i in xrange(0,len_df):
    text = text + df[1][i] + " "

seg_list = jieba.cut(text, cut_all=False)

fdist1 = nltk.FreqDist(seg_list)
fdist2 = sorted(fdist1.iteritems(), key=lambda d:d[1], reverse = True)

for i in xrange(0,len(fdist2)):
     fp2.write(fdist2[i][0] + ":" + str(fdist2[i][1]) + "\r\n")

fp2.close()
