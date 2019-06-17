#!/bin/bash

killall vlc

for ((i=1; i<=2; i++)); do
    for ((j=1; j<=4; j++)); do
        vlc rtmp://127.0.0.1:1935/live/$i/$j &
    done
done
