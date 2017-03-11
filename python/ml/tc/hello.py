#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()

#cursor.execute("select * from shop_info limit 5")
#cursor.execute("select city_name,count(*) as city_count from shop_info group by city_name order by city_count desc")
cursor.execute("select count(shop_id),date(time_stamp) as dates from user_pay where shop_id='1629' group by dates order by dates asc")
#cursor.execute("select shop_id,per_pay from shop_info order by per_pay desc,shop_id desc limit 50")
#cursor.execute("select shop_id,count(*) as shop_count from user_view group by shop_id order by shop_count desc limit 10")
#cursor.execute("select user_view.shop_id,count(user_view.shop_id) as shop_count,shop_info.cate_3_name,shop_info.city_name from user_view,shop_info where shop_info.shop_id=user_view.shop_id group by user_view.shop_id order by shop_count desc limit 20")
#cursor.execute("select user_pay.shop_id,count(user_pay.shop_id)*shop_info.per_pay as shop_count,shop_info.cate_2_name,shop_info.cate_3_name,shop_info.city_name from user_pay,shop_info where shop_info.shop_id=user_pay.shop_id group by user_pay.shop_id order by shop_count desc limit 20")
cursor.execute("select user_pay.shop_id,count(user_pay.shop_id) as shop_count,shop_info.cate_2_name,shop_info.cate_3_name,shop_info.city_name from user_pay,shop_info where shop_info.shop_id=user_pay.shop_id group by user_pay.shop_id order by shop_count asc limit 20")
cursor.execute("select shop_count,dates from shopcount where shop_id='1629' order by dates asc")
cursor.execute("select cate_2_name,count(*) as cate_count from shop_info where city_name='上海' group by cate_2_name order by cate_count desc")
cursor.execute("select shop_id from shop_info where city_name='上海' and cate_2_name='快餐'")
cursor.execute("select * from city_weather limit 10")
cursor.execute("select weather from city_weather group by weather")
cursor.execute("select * from city_weather where weather='东南风'")
cursor.execute("select shop_info.shop_id,shopcount.shop_count from shopcount,shop_info where shop_info.city_name='上海' and shop_info.shop_id=shopcount.shop_id order by shopcount.shop_count desc limit 20" )
cursor.execute("select count(distinct user_id) from user_pay")


results = cursor.fetchall()

date = []
x_cnt = []
for row in results:
    print "%s:%r:%r:%r:%s:%s:%s" % (row[0], row[1], row[2], row[3], row[4], row[5], row[6])
    x_cnt.append(row[0])
    date.append(row[1])
    #print "%d:" % (row[0])+row[1].strftime('%Y-%m-%d')
    #print "%d:" % (row[0])+row[1]
    #print "%d:%d" % (row[0],row[1])
    #print "%d:%d:" % (row[0],row[1])+row[2]+":"+row[3]+":"+row[4]
    #print row[0]+":%d" % row[1]
    #print "%d:%d:" % (row[0],row[1])+row[2]+":"+row[3]

cursor.execute("select count(shop_id),date(time_stamp) as dates from user_pay where shop_id='58' group by dates order by dates asc")

results1 = cursor.fetchall()
date1 = []
x_cnt1 = []
for row in results1:
    x_cnt1.append(row[0])
    date1.append(row[1])

plt.figure(1)
plt.plot(date, x_cnt)
plt.plot(date1, x_cnt1)
plt.show()

cursor.close()
db.close()
