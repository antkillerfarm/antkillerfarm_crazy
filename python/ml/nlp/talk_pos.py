#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import pandas as pd
import numpy as np
import nltk
import jieba
import jieba.posseg as pseg

def get_keyword(str):
    fw_list = ["亲","亲亲"]
    for i in xrange(0,len(fw_list)):
        fw_list[i] = fw_list[i].decode('utf-8')
    words = pseg.cut(str)
    keyword = ""
    for word, flag in words:
        if (flag in ["n","vn","l"] and word not in fw_list):
            #print('%s %s' % (word, flag))
            keyword = keyword + word + " "
    return keyword

df = pd.read_csv('/home/tj/big_data/data/talk/c2.csv', header=None)
fp2 = codecs.open("/home/tj/big_data/data/talk/c2c.csv", "w", "utf-8")


len_df = len(df)
kw_list = []
for i in xrange(0,len_df):
    keyword = get_keyword(df[1][i])
    kw_list.append(keyword)
    if (i % 500 == 0):
        print str(i) + "/" + str(len_df)

for i in xrange(0,len_df):
    s = df[0][i] + "," + df[1][i].decode('utf-8') + "," + kw_list[i] + "\r\n"
    fp2.write(s)

fp2.close()

#text = ""
#for i in xrange(0,len_df):
#    text = text + kw_list[i] + " "

#seg_list = text.split(' ')
#fdist1 = nltk.FreqDist(seg_list)
#fdist2 = sorted(fdist1.iteritems(), key=lambda d:d[1], reverse = True)

#for i in xrange(0,200):
#    print (fdist2[i][0] + ":" + str(fdist2[i][1]))


df0 = pd.read_csv('/home/tj/big_data/data/talk/c2c.csv', header=None)
fp3 = codecs.open("/home/tj/big_data/data/talk/c2cf.csv", "w", "utf-8")

kw_list = df0[2]
s = pd.Series(kw_list)
df2 = s.groupby(s).count()
df3 = df2.sort_values(ascending=False)

for (key, value) in df3.iteritems():
    k_list = key.split(' ')
    if (len(k_list) > 3 and value > 8):
        str0 = key + "," + str(value)
        print (str0)
        fp3.write(str0.decode('utf-8') + "\r\n")
        

fp3.close()


