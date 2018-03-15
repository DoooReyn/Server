#!/usr/bin/python
# -*- coding: UTF-8 -*-
#将一个列表的数据复制到另一个列表中。
import copy

a = [1,2,3]
b = a;
c = a[:]
d = copy.copy(a)
a[0] = 4
print a
print b
print c
print d
