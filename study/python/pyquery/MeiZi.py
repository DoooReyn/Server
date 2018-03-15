#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os
import DownHtml
import time
import threading

thradnum = 10

page_num = DownHtml.GetPageNum('ooxx.html')
print(u'当前ooxx总页数:%s' % page_num)
ip_list = DownHtml.GetProxy('daili.html')

pernum = int(page_num / thradnum)
leftnum = page_num % thradnum
leftstart = pernum * thradnum

thread_handle = []
for i in range(0, thradnum):
    start = i * pernum
    end = start + pernum
    t = threading.Thread(target=DownHtml.DownLoadThread, args=(start, end))
    thread_handle.append(t)
    t.setDaemon(True)
    t.start()


thr = threading.Thread(target=DownHtml.DownLoadThread, args=(leftstart, page_num))
thread_handle.append(thr)
thr.setDaemon(True)
thr.start()


for handle in thread_handle:
    handle.join()

print("DownLoad Over")
