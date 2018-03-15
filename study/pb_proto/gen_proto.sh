#!/bin/sh

FILE=`ls ./cmd_proto/*.proto`

for line in $FILE
do
	echo $line
	protoc -I=./cmd_proto/. --cpp_out=./gen_proto $line
done


