#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def get_user_count(cursor, shop_id):
    sql_str = "select * from shop"+str(shop_id)+"_1"
    cursor.execute(sql_str)
    results = cursor.fetchall()
    date = []
    x_cnt = []
    for row in results:
        x_cnt.append(row[0])
        date.append(row[1])
    return (date, x_cnt)

def filter_some_days(days, flags, filter_days):
    date_len = len(days)
    for i in xrange(0, date_len):
        day = days[i]
        if day in filter_days:
            flags[i] = False

def filter_whole_week(days, flags):
    year = 0
    weeknumber = 0
    weekday = 0
    date_len = len(days)
    day_num = 0
    for i in xrange(0, date_len):
        if (flags[i] == False):
            continue
        day = days[i]
        (year0, weeknumber0, weekday0) = day.isocalendar()
        if (year == 0):
            if (weekday0 == 1):
                year = year0
                weeknumber = weeknumber0
                day_num = 1
            else :
                flags[i] = False
        else :
            if (weeknumber0 != weeknumber):
                if (day_num != 7):
                    day_num0 = 0
                    for j in xrange(1, 8):
                        if (flags[i-j] == True and day_num0 < day_num):
                            flags[i-j] = False
                            day_num0 = day_num0 + 1
                            
                weeknumber = weeknumber0
                day_num = 1
            else :
                day_num = day_num + 1
    if (day_num != 7):
        day_num0 = 0
        for j in xrange(0, 7):
            if (flags[i-j] == True and day_num0 < day_num):
                flags[i-j] = False
                day_num0 = day_num0 + 1

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()

date = []
x_cnt = []

(date_item, x_cnt_item) = get_user_count(cursor, 1010)
date.append(date_item)
x_cnt.append(x_cnt_item)

date_len = len(date[0])
flags = [True for i in range(date_len)]
filter_days=[datetime.date(datetime(2016, 5, 1)), datetime.date(datetime(2016, 5, 2)), datetime.date(datetime(2016, 10, 1)),datetime.date(datetime(2016, 10, 7)),datetime.date(datetime(2016, 2, 5)),datetime.date(datetime(2016, 2, 12)),datetime.date(datetime(2016, 2, 19)),datetime.date(datetime(2016, 1, 1))]
filter_some_days(date[0], flags, filter_days)
filter_whole_week(date[0], flags)

for i in xrange(0, date_len):
    print "%s:%s:%d" % (date[0][i], flags[i], x_cnt[0][i])

user_sum = [0 for i in range(7)]
for i in xrange(0, date_len):
    if (flags[i] == True):
        weekday =  date[0][i].isoweekday()
        user_sum[weekday-1] = user_sum[weekday-1] + x_cnt[0][i]

np_user_sum = np.array(user_sum)
user_mean = np.mean(np_user_sum)
np_user_sum_ratio = np.array(user_sum) / user_mean
#np_real = np.array([2426,2180,2033,2433,2723,4008,3942])
np_real = np.array([1533,1536,1418,1432,1600,1527,1650])
np_real_mean = np.mean(np_real)
np_real1 = np.array([1370,1111,1384,1331,1465,1361,1576])
np_real_mean1 = np.mean(np_real1)

np_user_predict = (2426 / np_user_sum_ratio[0]) * np_user_sum_ratio
np_user_predict_err = np_user_predict - np_real
np_user_predict_std = np.std(np_user_predict_err)

np_user_predict1 = np_real_mean * np_user_sum_ratio
np_user_predict_err1 = np_user_predict1 - np_real
np_user_predict_std1 = np.std(np_user_predict_err1)

np_user_predict2 = np_real_mean1 * np_user_sum_ratio
np_user_predict_err2 = np_user_predict2 - np_real
np_user_predict_std2 = np.std(np_user_predict_err2)

#array([ 2426.,  2336.98353667,  2395.35498803,  2481.80183856,2653.36314778,  3632.57596799,  3751.47607652])

#>>> np_user_predict_std
#220.31426454471986
#>>> np_user_predict_std1
#218.72190949859953

#>>> np_user_predict_std
#201.08887418666424
#>>> np_user_predict_std1
#199.88795605992908

#>>> np_user_predict_std
#102.99318854058407
#>>> np_user_predict_std1
#71.670812728980621
#>>> np_user_predict_std2
#68.157180711796244

#1010:4321:小吃:其它小吃:北京
#706:4495:快餐:中式快餐:杭州
#45:4727:中餐:川菜:宁波
#308:4778:小吃:米粉/米线:杭州

cursor.close()
db.close()

fp = open("test.txt","w")
fp.write("123,234\n")
fp.write("123,234\n")
fp.close()
