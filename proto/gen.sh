#!/bin/sh

Dir=$(cd `dirname $0`; pwd)

FILE=`ls $Dir/cmd_proto/*.proto`


echo "\033[1;32m===============begin================\033[0m"
for line in $FILE
do
	echo $line
	protoc -I=$Dir/cmd_proto/ --cpp_out=$Dir/gen_proto $line
	#protoc -I=$Dir/cmd_proto/ --lua_out=$LUADir  $line
done

echo "\033[1;32m=============== end ================\033[0m"
