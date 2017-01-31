#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
import matplotlib.pyplot as plt

db = MySQLdb.connect("localhost","root","kkkkkk","ml")
cursor = db.cursor()

cursor.execute("select name from csdn group by name")
results = cursor.fetchall()

fname = results[25][0]
print "fname:%s" % (fname)

sql_str = "select * from csdn where name=\"" + fname + "\" order by s_time"

cursor.execute(sql_str)

results = cursor.fetchall()

date = []
read_cnt = []
for row in results:
    read_cnt.append(row[3])
    date.append(row[2])

plt.figure(1)
plt.plot(date, read_cnt)
plt.show()
    
cursor.close()
db.close()
