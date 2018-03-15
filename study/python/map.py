#!/usr/bin/python
# -*- coding: UTF-8 -*-
#map 遍历
d = {'x':1, 'y':2, 'z':3}

print '遍历列表方法1：'
for key in d:
	print "key:" , key, ", ", 'v:', d[key]

print ""

print '遍历列表方法2：'
for key, value in d.items():
	print "key:" , key,  ", ", 'v:',value