#!/usr/bin/python
# -*- coding: UTF-8 -*-
#斐波那契数列
def fib(n):
    if n <= 0:
        return 0
    elif n == 1 or n == 2:
        return 1
    else:
        return fib(n-1) + fib(n-2)

for i in range(20):
    print fib(i)

