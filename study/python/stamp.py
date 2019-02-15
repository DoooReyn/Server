#!/usr/bin/python
# coding:utf-8

import time
from optparse import OptionParser

def Opt():
   parser=OptionParser("Usage: %prog -d stamp -s strtime")
   parser.add_option("-d",
                      "--stamp",
                      dest="stamp",
                      default=0,
                      type=int,
                      help="timestamp",)
   parser.add_option("-s",
                      "--strtime",
                      dest="strtime",
                      default="",
                      type=str,
                      help="strtime",)
   options, args = parser.parse_args()
   return options, args, parser



def Parse():
    options, args, parser =  Opt()
    #print(options, args, parser)
    stamp = options.stamp
    strtime = options.strtime
    if stamp == 0 and strtime == "":
        stampcurtime = int(time.time())
        structcurtime = time.localtime(stampcurtime)
        strcurtime = time.strftime('%Y-%m-%d %H:%M:%S', structcurtime)
        print "NOW TIMESTAMP: %d ==> %s " % (stampcurtime, strcurtime)

    if stamp > 0:
        structcurtime = time.localtime(stamp)
        strdesttime = time.strftime('%Y-%m-%d %H:%M:%S', structcurtime)
        print "TIMESTAMP:\t %d ==> %s" % (stamp, strdesttime)

    if strtime != "":
        strdesttime = time.strptime(strtime,'%Y-%m-%d %H:%M:%S')
        deststamp = time.mktime(strdesttime)
        print  "TIMESTR:\t %s ==> %d" % (strtime, deststamp)


Parse()



