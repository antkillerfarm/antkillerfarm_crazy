#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt

def get_user_count(cursor, shop_id):
    #sql_str = "select count(shop_id),date(time_stamp) as dates from user_pay where shop_id='"+str(shop_id)+"' group by dates order by dates asc"
    sql_str = "select * from shop1629_1"
    cursor.execute(sql_str)
    results = cursor.fetchall()
    date = []
    x_cnt = []
    for row in results:
        x_cnt.append(row[0])
        date.append(row[1])
    return (date, x_cnt)

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()

date = []
x_cnt = []

(date_item, x_cnt_item) = get_user_count(cursor, 1629)
date.append(date_item)
x_cnt.append(x_cnt_item)
(date_item, x_cnt_item) = get_user_count(cursor, 58)
date.append(date_item)
x_cnt.append(x_cnt_item)
(date_item, x_cnt_item) = get_user_count(cursor, 517)
date.append(date_item)
x_cnt.append(x_cnt_item)

plt.figure(1)
plt.plot(date[0], x_cnt[0])
plt.plot(date[1], x_cnt[1])
plt.plot(date[2], x_cnt[2])
plt.show()

cursor.close()
db.close()
