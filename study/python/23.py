#!/usr/bin/python
# -*- coding: UTF-8 -*-

line = 20
for i in range(1,line + 1):
    print(' ' * (line - i),"*" *(2*i-1))

for i in range(line - 1,0, -1):
    print(' ' * (line - i), "*" * (2 * i - 1))