#!/usr/bin/python
# -*- coding: UTF-8 -*-
#输出 9*9 乘法口诀表
from __future__ import print_function
for i in range(1,10):
    for j in range(1,i+1):
        print ("%d * %d = %d " % (j,i,i*j),end='')
    print("")

