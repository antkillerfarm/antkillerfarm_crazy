#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb

db = MySQLdb.connect("localhost","root","kkkkkk","ml" )
cursor = db.cursor()

cursor.execute("SELECT VERSION()")

data = cursor.fetchone()
print "Database version : %s " % data

cursor.close()
db.close()

