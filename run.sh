#!/bin/sh

sh stop.sh
sh make.sh

./build/bin/accountserver &
sleep 0.2
./build/bin/loginserver &
sleep 0.2
./build/bin/gateserver 1 &
sleep 0.2
./build/bin/gateserver 2 &
sleep 0.2
./build/bin/gameserver 1 &
