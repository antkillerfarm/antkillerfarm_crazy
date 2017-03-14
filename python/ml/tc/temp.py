#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb
import numpy as np
from datetime import datetime
import matplotlib.pyplot as plt


for i in xrange(0, date_len):
    print "%s:%s:%d" % (date_item[i], flags[i], x_cnt_item[i])

(date_item, x_cnt_item) = get_user_count(cursor, 517)
date_len = len(date_item)
flags = [True for i in range(date_len)]
filter_days=[datetime.date(datetime(2016, 5, 1)), datetime.date(datetime(2016, 5, 3)), datetime.date(datetime(2016, 10, 1)),datetime.date(datetime(2016, 10, 7)),datetime.date(datetime(2016, 2, 5)),datetime.date(datetime(2016, 2, 12)),datetime.date(datetime(2016, 2, 19)),datetime.date(datetime(2016, 1, 1))]
filter_some_days(date_item, flags, filter_days)
filter_whole_week(date_item, flags)

total = handle_weeks(date_item, flags, x_cnt_item)
np_user_sum = np.array(total.user_sum)
user_mean = np.mean(np_user_sum)
np_user_sum_ratio = np_user_sum / user_mean

weeks_len = len(total.weeks)
np_user_sum2 = np.array(total.weeks[weeks_len - 1].x_cnt) + np.array(total.weeks[weeks_len - 1].x_cnt) + np.array(total.weeks[weeks_len - 1].x_cnt)
user_mean2 = np.mean(np_user_sum2)
np_user_sum_ratio2 = np_user_sum2 / user_mean2
last_week_sum = total.weeks[weeks_len - 1].sum
last_week_mean = last_week_sum / 7.0
    
np_user_predict = last_week_mean * np_user_sum_ratio
user_predict = np.rint(np_user_predict)

mean = []
mean1 = []
mean2 = []
mean3 = []
mean4 = []
week_len = len(total.weeks)
for i in xrange(week_len):
    mean.append(total.weeks[i].mean)
    #mean1.append(total.weeks[i].max-total.weeks[i].min)
    mean2.append(total.weeks[i].min/mean[i]*(total.sum/(week_len*7.0)))
    mean3.append(total.weeks[i].min)
    mean4.append(total.weeks[i].max)

plt.figure(1)
plt.plot(mean)
#plt.plot(mean1)
plt.plot(mean2)
plt.plot(mean3)
plt.plot(mean4)
plt.show()
