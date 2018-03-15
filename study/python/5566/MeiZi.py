#!/usr/bin/python
# -*- coding: UTF-8 -*-


import time
import threadpool

def sayhello(str):
    print("hello", str)
    time.sleep(2)



name_list =['xiaozi','aa','bb','cc']
start_time = time.time()
pool = threadpool.ThreadPool(10)
requests = threadpool.makeRequests(sayhello, name_list)
[pool.putRequest(req) for req in requests]
pool.wait()
print ('%d second'% (time.time()-start_time))

def hello(m1, n, o):
    print ("m = %s, n = %s, o = %s"%(m1, n, o))


lst_vars_1 = ['1', '2', '3']
lst_vars_2 = ['4', '5', '6']
func_var = [(lst_vars_1, None), (lst_vars_2, None)]

dict_vars_1 = {'m1':'1', 'n':'2', 'o':'3'}
dict_vars_2 = {'m1':'4', 'n':'5', 'o':'6'}
func_var2 = [(None, dict_vars_1), (None, dict_vars_2)]

pool = threadpool.ThreadPool(2)
requests = threadpool.makeRequests(hello, func_var2)
for req in requests:
    print(req)

[pool.putRequest(req) for req in requests]
pool.wait()

gen = list(range(0,5))
print(gen)