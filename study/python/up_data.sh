#! /usr/bin/expect
set timeout 30

spawn scp -r ../python root@119.23.13.79:/root
expect "*password:"
send "900711,.?\r"
expect eof


