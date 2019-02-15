#!/bin/sh
for((i=0;i<5000;i++))
do
	./build/bin/client &
	sleep 0.01
done
