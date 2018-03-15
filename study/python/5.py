#!/usr/bin/python
# -*- coding: UTF-8 -*-
#排序


L = [6, 5, 1, 3, 4, 2]
L.sort(reverse=True)
print L

def  my_cmp(E1,E2):
	return -cmp(E1[1],E2[1])
	
L = [('a', 0), ('b', 1), ('c', 2), ('d', 3)]
L.sort(my_cmp)
print L