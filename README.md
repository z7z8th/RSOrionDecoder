# Multi process/thread Video Decoder/Encoder/RTMP_Publisher
<br />

Introduction
===
Code flow
---
* Dispatch multiple video source (local video or live stream) to threads of child process
* Intel HW VAAPI Decoder decodes input video to YUV in RSFFMpegDecoderTask, pass the decoded frame to next task (Data Dumper)
* Dump decoded video frame to file for review, pass frame to RSFFMpegEncoderTask
* Intel HW VAAPI Encoder encodes video frame to H264, pass the encoded packet to next task (Data Dumper)
* Dump encoded video packet to file for review, pass packet to RTMP publisher
* RTMP publisher publishes raw H264 packet to RTMP server [ossrs/srs](https://github.com/ossrs/srs)

Video Decoder/Encoder
---
* FFMPEG 4.1.3
* Intel VAAPI HW Accel

RTMP Publisher
---
* through srs-librtmp

Build
===
1. build srs RTMP server
    ```shell
    mkdir ~/src
    cd ~/src
    git clone https://github.com/ossrs/srs
    cd srs/trunk
    ./configure --without-http-callback --without-ssl && make -j14
    # ./objs/srs -c conf/rtmp.conf
    ```
2. export srs-librtmp and build
    ```shell
    cd ~/src/srs/trunk
    ./configure --export-librtmp-project=$HOME/src/srs-librtmp
    cd ~/src/srs-librtmp
    # git init .
    # git add .
    # git commit -m "srs-librtmp @ 6e6e996bbaee73473f2ace76b7cb45a065368e7c of srs, Release 2.0.263"
    make -j14
    # research/librtmp/objs/srs_h264_raw_publish ../RSOrionDecoder/encoded-test.h264.tid_xxxxxxxxx rtmp://127.0.0.1:1935/live/1 25
    ```
3. build RSOrionDecoder (depends on srs-librtmp)
    ```shell
    cd ~/src
    git clone https://github.com/z7z8th/RSOrionDecoder
    cd RSOrionDecoder
    # IMPORTANT: modify video source array `vSrcArray' to yours in demo/demo.cpp before make
    make clean
    make all
    ```

Run
===
1. Run srs RTMP server
    ```shell
    cd ~/src
    cd srs/trunk
    ./objs/srs -c conf/rtmp.conf
    ```
2. Test RTMP publish (optional)
    ```
    research/librtmp/objs/srs_h264_raw_publish path/to/encoded-test.h264 rtmp://127.0.0.1:1935/live/1 25`
    ```
3. Run RSOrionDecoder to test Decode/Encode/RTMP_Publish
    ```shell
    cd ~/src/RSOrionDecoder
    ./tools/start-rtmp-server.sh
    ./bin/libOrionDecoder
    ./tools/start-rtmp-clients-mpv.sh
    ```

Test (Video file too large for git)
===
1. Test Environment
    * Hardware:
      * CPU: 4 Core Intel Xeon v3, 2 Logical Threads each Core
      * Memory: 24GB DDR3 2133MHz
      * Disk: Samsung SSD
    * Software
      * Debian buster/testing
      * FFMPEG 4.1.3
      * ossrs/srs 2.0rc6
2. Test Material
    * 8 1080p videos, average bit rate of them are:
      * 2 videos at ~20Mbps
      * 2 videos at ~15Mbps
      * 4 videos at ~9Mbps
3. Result
    * mpv plays smoothly with the later 6 videos, average bit rate at ~15Mbps or ~9Mbps.
    * there's some lag with the first 2 videos, average bit rate at ~20Mbps.
