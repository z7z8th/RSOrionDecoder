#!/bin/bash

[ $# -lt 1 ] && {
    echo "usage: $0 kill_interval_in_seconds";
    exit 1;
}
KINTERVAL=$1
MPID=`ps -C libOrionDecoder -o pid= | sed -n 1p`
[ -z "$MPID" ] && {
    echo "no libOrionDecoder running";
    exit 1;
}

kCnt=0
while true; do
    kCnt=$((kCnt+1))
    echo kill test count $kCnt 
    CPID=`ps -C libOrionDecoder -o pid= | grep -v $MPID`
    for pid in $CPID; do
        echo kill -TERM $pid
        kill -TERM $pid
    done
    echo
    sleep ${KINTERVAL}s
done
