#!/usr/bin/python
# -*- coding: UTF-8 -*-
# log 模块封装

import time


LEVEL_DEBUG = 1
LEVEL_INFO = 2
LEVEL_WARN = 3
LEVEL_ERROR = 4


def GetTime():
    tbuf = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
    return tbuf


def Log(message, level):

    post = "\033[0m"
    pre = ""

    if level == LEVEL_DEBUG:
        post = ""

    if level == LEVEL_INFO:
        pre += "\033[32;1m"

    if level == LEVEL_WARN:
        pre += "\033[33;1m"

    if level == LEVEL_ERROR:
        pre += "\033[31;1m"

    print "%s%s%s" % (pre, message, post)


def DEBUG(message):
    tbuf = GetTime()
    message = tbuf + " [DEBUG] " + message
    Log(message, LEVEL_DEBUG)


def INFO(message):
    tbuf = GetTime()
    message = tbuf + " [INFO] " + message
    Log(message, LEVEL_INFO)


def WARN(message):
    tbuf = GetTime()
    message = tbuf + " [WARN] " + message
    Log(message, LEVEL_WARN)


def ERROR(message):
    tbuf = GetTime()
    message = tbuf + " [ERROR] " + message
    Log(message, LEVEL_ERROR)
