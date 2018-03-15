#!/usr/bin/python
# -*- coding: UTF-8 -*-
#将一个正整数分解质因数。例如：输入90,打印出90=2*3*3*5

def reduceNum(n):
    nlist = []
    while n != 1:
        for i in range(2,n+1):
            #print(i)
            mod = n % i
            if mod == 0:
                nlist.append(i)
                if i == n:
                    return nlist
                else:
                    n = int(n / i)
                    break

    return nlist





n = int(input("请输入一个正整数："))
nlist = reduceNum(n)
print(nlist)
