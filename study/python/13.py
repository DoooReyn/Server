#!/usr/bin/python
# -*- coding: UTF-8 -*-
#利用for循环控制100-999个数，每个数分解出个位，十位，百位

for n in range(100,1000):
    org = n
    nlist = []
    while org > 0:
        num = int(org % 10)
        org = int(org/10)
        if num > 0:
            nlist.append(num)

    targe = 0
    for item in nlist:
        targe += pow(item,3)

    if targe == n:
        print(n)


