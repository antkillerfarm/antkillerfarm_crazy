#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
from datetime import datetime
import matplotlib.pyplot as plt

class statistics_info:
    def __init__(self):
        self.num = 0
        self.sum = 0.0
        self.weeks = []
        self.user_sum = [0 for i in range(7)]

class week_info:
    def __init__(self):
        self.sum = 0.0
        self.max = 0.0
        self.min = 0.0
        self.mean = 0.0

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

def handle_week(days, x_cnt):
    week = week_info()
    np_x_cnt = np.array(x_cnt)
    week.min = np.min(np_x_cnt)
    week.max = np.max(np_x_cnt)
    week.mean = np.mean(np_x_cnt)
    week.sum = np.sum(np_x_cnt)
    return week

def handle_weeks(days, flags, x_cnt):
    date_len = len(days)
    total_info = statistics_info()
    day_num = 0
    i = 0
    while (i < date_len):
        if (flags[i] == True):
            days0 = days[i : i + 7]
            x_cnt0 = x_cnt[i : i + 7]
            week = handle_week(days0, x_cnt0)
            total_info.weeks.append(week)
            total_info.sum = total_info.sum + week.sum
            total_info.num = total_info.num + 7
            for j in xrange(7):
                total_info.user_sum[j] = total_info.user_sum[j] + x_cnt0[j]
            i = i + 7
        else:
            i = i + 1
    return total_info

def show_total_info(total_info):
    mean = []
    week_len = len(total_info.weeks)
    for i in xrange(week_len):
        mean.append(total_info.weeks[i].mean)

def output_predict(fp, shop_id, user_predict):
    output_str = str(shop_id)
    predict_len = len(user_predict)
    for i in xrange(predict_len):
        output_str = output_str + "," + str(int(user_predict[i]))
    output_str = output_str + "\r\n"
    fp.write(output_str)

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()
fp = open("predict.csv","w")

for id in xrange(1, 2001):
    print "shop_id: %d" % id
    (date_item, x_cnt_item) = get_user_count(cursor, id)
    
    date_len = len(date_item)
    flags = [True for i in range(date_len)]
    filter_days=[datetime.date(datetime(2016, 5, 1)), datetime.date(datetime(2016, 5, 3)), datetime.date(datetime(2016, 10, 1)),datetime.date(datetime(2016, 10, 7)),datetime.date(datetime(2016, 2, 5)),datetime.date(datetime(2016, 2, 12)),datetime.date(datetime(2016, 2, 19)),datetime.date(datetime(2016, 1, 1))]
    filter_some_days(date_item, flags, filter_days)
    filter_whole_week(date_item, flags)
    #for i in xrange(0, date_len):
    #print "%s:%s:%d" % (date_item[i], flags[i], x_cnt_item[i])
    total = handle_weeks(date_item, flags, x_cnt_item)
    np_user_sum = np.array(total.user_sum)
    user_mean = np.mean(np_user_sum)
    np_user_sum_ratio = np.array(total.user_sum) / user_mean
    
    weeks_len = len(total.weeks)
    last_week_sum = total.weeks[weeks_len - 1].sum
    last_week_mean = last_week_sum / 7.0
    last_week_sum2 = (total.weeks[weeks_len - 3].sum + total.weeks[weeks_len - 2].sum) / 2.0
    last_week_mean2 = last_week_sum2 / 7.0
    gradient = (last_week_mean - last_week_mean2) / (3.0 * last_week_mean)
    gradient_threshold = 0.15
    ratio_threshold = 2.0
    if (abs(gradient) > gradient_threshold):
        #print "xxx: %d" % id
        total_mean = total.sum / total.num
        ratio = last_week_mean2 / total_mean
        if (ratio > ratio_threshold or ratio < (1 / ratio_threshold)):
            #print "yyy: %d" % id
            last_week_mean = (total_mean + last_week_mean) / 2.0
        else :
            #print "zzz: %d" % id
            last_week_mean = (last_week_mean2 + last_week_mean) / 2.0
        if (gradient > 0):
            gradient = -gradient_threshold * 0.5
        else :
            gradient = gradient_threshold * 0.5
    else :
        last_week_mean = (last_week_mean2 + last_week_mean) / 2.0
    gradient = 1# - gradient
    #print "%r:%r:%r" % (last_week_mean, last_week_mean2, gradient)
    
    predict = []
    now_week_mean = last_week_mean * gradient
    np_user_predict = now_week_mean * np_user_sum_ratio 
    user_predict = np.rint(np_user_predict)
    predict.extend(user_predict[1:7])
    
    now_week_mean = now_week_mean * gradient
    np_user_predict = now_week_mean * np_user_sum_ratio 
    user_predict = np.rint(np_user_predict)
    predict.extend(user_predict)

    now_week_mean = now_week_mean * gradient
    np_user_predict = now_week_mean * np_user_sum_ratio 
    user_predict = np.rint(np_user_predict)
    predict.append(user_predict[0])

    predict[10] = predict[10] * 1.09
    
    output_predict(fp, id, predict)

fp.close()

cursor.close()
db.close()

