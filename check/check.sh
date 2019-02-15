#!/bin/sh
FILE="tmp.xml"
XML="result.xml"
set -x
cppcheck --enable=all --xml 2  -U SO_REUSEPORT  -i../study -i../build ../  2>$FILE
egrep -v 'gen_proto|r3c|tinyxml2|cjson|test|unusedFunction|missingInclude' $FILE > $XML
rm $FILE

