#!/bin/bash

killall srs

sleep 1

SCRIPT_DIR=`readlink -m $PWD/$(dirname $0)`
SRS_DIR=$SCRIPT_DIR/../../srs/trunk
cd $SRS_DIR
./objs/srs -c conf/rtmp.conf &
sleep 2
cd -
