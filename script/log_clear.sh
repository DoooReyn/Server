#!/bin/bash

current=`date "+%Y-%m-%d 00:00:00"`
echo $current
timeStamp=`date -d "$current" +%s`
echo $timeStamp
let timeStamp=timeStamp-86400*90
echo $timeStamp
date_new=`date -d @$timeStamp "+%Y%m%d"`
echo $date_new

