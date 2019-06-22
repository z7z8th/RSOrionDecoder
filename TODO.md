1. [x] ~~CPU usage too high when reach end of file, HThread::doTask shouldn't be empty~~
2. [x] RTMP publish ignore sps pps caused lag, comment out return statement after checking sps/pps duplicated in srs-librtmp
3. [ ] CUVID hardware accel decode/encode
4. [ ] Function for convert RSAVFrame to RSFrame, format NV12 to BGR24
5. [x] ~~Handle child process death, added RSProcManager to manage child by signal SIGCHLD and SIGTERM/SIGINT~~
6. [ ] Audio support
7. [x] std::cout and printf mixed, this is hard to eliminate, since ffmpeg and srs uses a lot of C styles.
8. [ ] Resource leak check
9. [ ] Master process communicate with child for info exchange, by socket?
10. [x] ~~Get frame rate from input file for RTMP publish~~
11. [x] ~~Handle signal TERM INT to stop gracefully~~
12. [ ] Limit HW encoder output bit rate, default rate too high roughly 70Mbits/s.
13. [x] ~~writev() will cause uncaughtable exception if write more than about 5 seconds buffer to rtmp server, solved by ignore SIGPIPE and restart rtmp publish~~
14. [ ] Abort whole task chain when some task in the middle of the chain exit.
