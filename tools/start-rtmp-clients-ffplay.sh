#!/bin/bash

CLIENT=ffplay

killall $CLIENT

sleep 1

for ((i=1; i<=2; i++)); do
    for ((j=1; j<=4; j++)); do
        $CLIENT -loglevel warning rtmp://127.0.0.1:1935/live/$i/$j &
    done
done
