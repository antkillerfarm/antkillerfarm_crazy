#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt

def get_blog_name(cursor):
    cursor.execute("select name from csdn group by name")
    results = cursor.fetchall()
    blog_name = []
    for row in results:
        blog_name.append(row[0])
    return blog_name

def get_read_cnt(cursor, blog_name):
    sql_str = "select * from csdn where name=\"" + blog_name + "\" order by s_time"
    cursor.execute(sql_str)
    results = cursor.fetchall()
    date = []
    read_cnt = []
    for row in results:
        read_cnt.append(row[3])
        date.append(row[2])
    return (date, read_cnt)

db = MySQLdb.connect("localhost","root","kkkkkk","ml")
cursor = db.cursor()

blog_name = get_blog_name(cursor)

plt.figure(1)
for i in xrange(8):
    (date, read_cnt) = get_read_cnt(cursor, blog_name[i*5])
    plt.plot(date, read_cnt)
plt.show()

cursor.close()
db.close()
