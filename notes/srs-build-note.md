https://github.com/ossrs/srs

./configure --without-http-callback --without-ssl && make -j14
./objs/srs -c conf/rtmp.conf

./configure --export-librtmp-project=$HOME/src/srs-librtmp
cd $HOME/src/srs-librtmp
git init .
git add .
git commit -m "srs-librtmp @ 6e6e996bbaee73473f2ace76b7cb45a065368e7c of srs, Release 2.0.263"
make -j14
research/librtmp/objs/srs_h264_raw_publish ../RSOrionDecoder/encoded-test.h264.tid_7f3ae7ea1700 rtmp://127.0.0.1:1935/live/1 25

