#!/usr/bin/python
# -*- coding: UTF-8 -*-
#暂停一秒输出

import time

for i in range(10):
    print time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
    time.sleep(1)

