#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def get_user_count(cursor, shop_id):
    sql_str = "select * from shop1629_1"
    cursor.execute(sql_str)
    results = cursor.fetchall()
    date = []
    x_cnt = []
    for row in results:
        x_cnt.append(row[0])
        date.append(row[1])
    return (date, x_cnt)

a = "2016-01-01"
timeArray = datetime.strptime(a, "%Y-%m-%d")
timeArray.isocalendar()

def is_whole_week(days):
    flag = []
    year = 0
    weeknumber = 0
    weekday = 0
    for day in days:
        (year0, weeknumber0, weekday0) = day.isocalendar()

    return flag

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()

date = []
x_cnt = []

(date_item, x_cnt_item) = get_user_count(cursor, 1629)
date.append(date_item)
x_cnt.append(x_cnt_item)

plt.figure(1)
plt.plot(date[0], x_cnt[0])
plt.show()

flag = []
year = 0
weeknumber = 0
weekday = 0

date_len = len(date[0])
flags = [False for i in range(date_len)]
day_num = 0
for i in xrange(0, date_len):
    day = date[0][i]
    (year0, weeknumber0, weekday0) = day.isocalendar()
    if (year == 0):
        if (weekday0 == 1):
            year = year0
            weeknumber = weeknumber0
            day_num = 1
    else :
        if (weeknumber0 != weeknumber):
            if (day_num == 7):
                for j in xrange(1, 8):
                    flags[i-j] = True;
            weeknumber = weeknumber0
            day_num = 1
        else :
            day_num = day_num + 1

for i in xrange(0, date_len):
    print "%s:%s" % (date[0][i], flags[i])

for j in xrange(1, 8):
    print j

cursor.close()
db.close()
