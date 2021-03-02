#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as dates
import matplotlib.ticker as ticker

current_dir = os.path.dirname(os.path.abspath(__file__))

df = pd.read_csv(current_dir + "/time_series.csv", header=None)
df_time = pd.to_datetime(df.iloc[:, 0])
df_time_diff = df_time.diff() / pd.Timedelta(1, 'D')
df_power = df.iloc[:, 1]
df_power_diff = df_power.diff() / df_time_diff
plt.figure(figsize=(12, 8))
plt.plot(df_time[1:], df_power_diff[1:])
plt.plot(df_time[1:], df_power_diff[1:].rolling(30).mean())
plt.xlabel('Time')
plt.ylabel('Power')

ax = plt.gca()
ax.xaxis.set_major_locator(dates.DayLocator(interval=200))
ax.yaxis.set_major_locator(ticker.MultipleLocator(200))
plt.show()
