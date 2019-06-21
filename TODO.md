1. [x] ~~CPU usage too high when reach end of file, HThread::doTask shouldn't be empty~~
2. [ ] RTMP publish ignore sps pps caused lag
3. [ ] CUVID hardware accel decode/encode
4. [ ] Function for convert RSAVFrame to RSFrame, format NV12 to BGR24
5. [x] ~~handle child process death, added RSProcManager to manage child by signal SIGCHLD and SIGTERM/SIGINT~~
6. [ ] Audio support
7. [x] std::cout and printf mixed, this is hard to eliminate, since ffmpeg and srs uses a lot of C styles.
8. [ ] Resource leak check
9. [ ] Master process communicate with child for info exchange, by socket?
10. [x] ~~Get frame rate from input file for RTMP publish~~
11. [x] ~~Handle signal TERM INT to stop gracefully~~
12. [ ] Limit HW encoder output bit rate, default rate too high roughly 70Mbits/s.
