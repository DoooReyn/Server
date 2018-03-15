#!/usr/bin/python
# -*- coding: UTF-8 -*-
#三种遍历列表里面序号和值的方法

list = ['html', 'js', 'css', 'python']

# 方法1
print '遍历列表方法1：'
for i in list:
	print ("序号：%s   值：%s" % (list.index(i), i))

print '\n遍历列表方法2：'
# 方法2
for i in range(len(list)):
	print ("序号：%s   值：%s" % (i, list[i]))

# 方法3
print '\n遍历列表方法3：'
for i, val in enumerate(list):
	print ("序号：%s   值：%s" % (i, val))

# 方法3
print '\n遍历列表方法3 （设置遍历开始初始位置，只改变了起始序号）：'
for i, val in enumerate(list, 2):
	print ("序号：%s   值：%s" % (i, val))