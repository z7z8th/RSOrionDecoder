#!/bin/bash

killall vlc
killall srs

sleep 1

SCRIPT_DIR=`readlink -m $PWD/$(dirname $0)`
SRS_DIR=$SCRIPT_DIR/../../srs/trunk
cd $SRS_DIR
./objs/srs -c conf/rtmp.conf &
sleep 2
cd -

for ((i=1; i<=2; i++)); do
    for ((j=1; j<=4; j++)); do
        vlc rtmp://127.0.0.1:1935/live/$i/$j &
    done
done
