#!/bin/sh
set -x
SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-build}
mkdir -p $BUILD_DIR \
	&& cd $BUILD_DIR \
	&& cmake -DCMAKE_BUILD_TYPE=Release $SOURCE_DIR \
	&& make $*

cd $SOURCE_DIR/lib/test && make
