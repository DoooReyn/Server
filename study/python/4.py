#!/usr/bin/python
# -*- coding: UTF-8 -*-
#输入某年某月某日，判断这一天是这一年的第几天？
import time

year = int(raw_input('year:'))
month = int(raw_input('month:'))
day = int(raw_input('day:'))

struct_time = (year,month,day,0,0,0,0,0,0)
struct_time2 = (year,1,1,0,0,0,0,0,0)

time1 = time.mktime(struct_time)
time2 = time.mktime(struct_time2)

day =  (time1 - time2) / 86400 + 1

print 'it is the %dth day.' % day

months = (31,28,31,30,31,30,31,31,30,31,30,31)

sum = 0
for i in range(0,month):
    sum += months[month]

sum += day
leap = 0;
if (year % 4 == 0) or ((year % 4 == 0) and (year % 00 != 0)):
    leap = 1;

if (leap == 1 ) and (month > 2):
    sum += 1

print 'it is the %dth day.' % day
#curtime =  time.time()
#print curtime
#struct_time = time.localtime(curtime)
#stamp_time = time.mktime(struct_time)
#print struct_time
#print stamp_time
#print time.strftime('%Y-%m-%d %H:%M:%S',struct_time)
