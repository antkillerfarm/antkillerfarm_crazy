#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import pandas as pd
import numpy as np

def get_another_index_list(kw_index):
    kw_role = df1[0][kw_index]
    all_str = df1[1][kw_index]
    flag = 0
    if (all_str.find("TAGSTART") != -1):
        flag = 1
    
    i = 0
    another_index_list = []
    for j in xrange(1,21):
        if ( flag == 1):
            #print "TAGSTART"
            break
        role = df1[0][kw_index - j]
        all_str = df1[1][kw_index - j]
        if (role != kw_role):
            #print df1.iloc[kw_index - j : kw_index - j + 1, :]
            another_index_list.append(kw_index - j)
            i = i + 1
            if (all_str.find("TAGSTART") != -1):
                flag = 1
                break
            if (i > 4):
                break
    return (another_index_list,flag)

df1 = pd.read_csv('/home/tj/big_data/data/talk/c2c.csv', header=None)
df2 = pd.read_csv('/home/tj/big_data/data/talk/c2cf.csv', header=None)
#fp1 = codecs.open("/home/tj/big_data/data/talk/c2cd.csv", "w", "utf-8")

for j in xrange(0,len(df2)):
    key_word = df2[0][j]
    kw_index_list = df1[df1[2] == key_word].index
    
    all_index_list = []
    start_flag = 0
    for i in xrange(0,len(kw_index_list)):
        kw_index = kw_index_list[i]
        (index_list, flag) = get_another_index_list(kw_index)
        all_index_list.extend(index_list)
        start_flag = start_flag + flag
    
    df3_list = []
    for i in xrange(0,len(all_index_list)):
        df3_list.append(df1.iloc[all_index_list[i] : all_index_list[i] + 1, :])
    df3 = pd.concat(df3_list)
    
    kw_list = df3[2]
    s = pd.Series(kw_list)
    df4 = s.groupby(s).count()
    df5 = df4.sort_values(ascending=False)
    
    list_len = len(all_index_list)
    factor = start_flag * 1.0 / list_len
    f_flag = "good"
    if (factor > 0.7):
        f_flag = ""
    print str(j) + ":" + str(list_len) + ":" + str(start_flag) + ":" + str(factor) + ":" + f_flag
