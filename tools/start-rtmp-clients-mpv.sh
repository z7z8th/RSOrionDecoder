#!/bin/bash

CLIENT=mpv

killall $CLIENT

sleep 1

for ((i=1; i<=2; i++)); do
    for ((j=1; j<=4; j++)); do
        $CLIENT rtmp://127.0.0.1:1935/live/$i/$j &
    done
done
