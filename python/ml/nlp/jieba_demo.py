#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import jieba

#seg_list = jieba.cut("我来到北京清华大学", cut_all=False)
#print("/".join(seg_list))

fp1 = codecs.open("/home/tj/big_data/data/jinyong/utf8/1.txt", "r", "utf-8")
fp2 = codecs.open("/home/tj/big_data/data/jinyong/utf8/x1.txt", "w", "utf-8")

#line = "我来到北京清华大学".decode('utf-8')
#seg_list = []
#for i in range(0,len(line)):
#    seg_list.append(line[i])
    
#print len(line)
#print " ".join(seg_list)

line =  fp1.readline()
while line:
    line1 = line.strip()
    seg_list = jieba.cut(line1, cut_all=False)
    #seg_list = []
    #for i in range(0,len(line)):
    #    seg_list.append(line[i])
    if (line1 != ""):
        fp2.write(" ".join(seg_list) + " ")
    #fp2.write(str(len(line)) +" ")
    
    line =  fp1.readline()

fp2.close()
fp1.close()

#for i in range(0,len(line)):
#    seg_list.append(line[i])
