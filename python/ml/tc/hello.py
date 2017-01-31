#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb

db = MySQLdb.connect("localhost","root","kkkkkk","tc")
cursor = db.cursor()

cursor.execute("select * from shop_info limit 5")
results = cursor.fetchall()

cursor.close()
db.close()
