#!/usr/bin/python
# -*- coding: UTF-8 -*-
#题目：一个数如果恰好等于它的因子之和，这个数就称为"完数"。例如6=1＋2＋3.编程找出1000以内的所有完数。
from functools import reduce
for i in range(2,1001):
    org = i
    nlist = []
    for j in range(1,int(org / 2) + 1):
        mod = org % j
        if mod == 0:
            nlist.append(j)

    #print(nlist)
    #print(org)
    if len(nlist) == 0:
        continue

    sum = reduce(lambda x,y : x + y,nlist)
    if sum == org:
        print(i,"=>",nlist)

