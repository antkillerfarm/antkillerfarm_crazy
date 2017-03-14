#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
from datetime import datetime

def get_user_count(cursor, shop_id):
    sql_str = "select shop_count,dates from shopcount where shop_id='" + str(shop_id) + "' order by dates asc"
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

def output_predict(fp, shop_id, user_predict):
    output_str = str(shop_id)
    predict_len = len(user_predict)
    for i in xrange(1, 7):
        output_str = output_str + "," + str(int(user_predict[i]))
    output_str = output_str + "," + str(int(user_predict[0]))
    for i in xrange(1, 7):
        output_str = output_str + "," + str(int(user_predict[i]))
    output_str = output_str + "," + str(int(user_predict[0]))
    output_str = output_str + "\n"
    fp.write(output_str)

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()
fp = open("predict.csv","w")

for id in xrange(1, 2001):
    print "shop_id: %d" % id
    (date_item, x_cnt_item) = get_user_count(cursor, id)
    
    date_len = len(date_item)
    flags = [True for i in range(date_len)]
    filter_days=[datetime.date(datetime(2016, 5, 1)), datetime.date(datetime(2016, 5, 2)), datetime.date(datetime(2016, 10, 1)),datetime.date(datetime(2016, 10, 7)),datetime.date(datetime(2016, 2, 5)),datetime.date(datetime(2016, 2, 12)),datetime.date(datetime(2016, 2, 19)),datetime.date(datetime(2016, 1, 1))]
    filter_some_days(date_item, flags, filter_days)
    filter_whole_week(date_item, flags)
    
    user_sum = [0 for i in range(7)]
    for i in xrange(0, date_len):
        if (flags[i] == True):
            weekday =  date_item[i].isoweekday()
            user_sum[weekday-1] = user_sum[weekday-1] + x_cnt_item[i]
    
    np_user_sum = np.array(user_sum)
    user_mean = np.mean(np_user_sum)
    np_user_sum_ratio = np.array(user_sum) / user_mean
    
    weekday_num = 0
    last_week_sum = 0
    for i in xrange(0, date_len):
        j = date_len - 1 - i
        if (flags[j] == True):
            if (weekday_num < 7):
                weekday_num = weekday_num + 1
                last_week_sum = last_week_sum +  x_cnt_item[j]
            else :
                break
    
    last_week_mean = last_week_sum / 7.0
    
    np_user_predict = last_week_mean * np_user_sum_ratio
    user_predict = np.rint(np_user_predict)
    output_predict(fp, id, user_predict)

fp.close()

cursor.close()
db.close()

