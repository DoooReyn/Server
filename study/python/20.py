#!/usr/bin/python
# -*- coding: UTF-8 -*-
# 题目：一球从100米高度自由落下，每次落地后反跳回原高度的一半；再落下，求它在第10次落地时，共经过多少米？第10次反弹多高？

hei = 100.0  # 起始高度
tim = 10     # 次数

totol = 0.0
last = 0;
for i in range(0, tim):
    if i == 0:
        totol += hei
    else:
        totol += hei *2

    hei /= 2
    if i + 1 == tim:
        last = hei

print(totol)
print(last)
