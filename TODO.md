1. [x] ~~CPU usage too high when reach end of file, HThread::doTask shouldn't be empty~~
2. [x] RTMP publish ignore sps pps caused lag
	* solved by comment out return statement after checking sps/pps duplicated in srs-librtmp
3. [ ] CUVID hardware accel decode/encode, need NVIDIA video card
4. [ ] Function for convert RSAVFrame to RSFrame, format NV12 to BGR24
5. [x] ~~Handle child process death, added RSProcManager to manage child by signal SIGCHLD and SIGTERM/SIGINT~~
6. [ ] Audio support:
	* decode multiple format, decode only one audio track
	* encode to one specific format(aac/ogg/mp3).
7. [x] std::cout and printf mixed, this is hard to eliminate, since ffmpeg and srs uses a lot of C styles.
8. [ ] Resource leak check: close handle, free memory, unlock locks, avoid dead lock, ...
9. [x] ~~Master process communicate with child for info exchange, by socket?~~ see item 'Dynamic stream management'
10. [x] ~~Get frame rate from input file for RTMP publish~~
11. [x] ~~Handle signal TERM INT to stop gracefully~~
12. [ ] Limit HW encoder output bit rate, default rate too high roughly 70Mbits/s.
	* get input video bit rate, pass to encoder
13. [x] ~~writev() will cause uncaughtable exception if write more than about 5 seconds buffer to rtmp server~~
	* solved by ignore SIGPIPE and restart rtmp publish
14. [ ] Abort whole task chain when some task in the middle of the chain exit.
	* By double linked task chain?
	* input queue of next task might be full, and Push to it will block(scheduled out by kernel)
15. [ ] ffplay will be disconnected by srs after some time
	* but mpv is ok.
16. [ ] Implement flow control in RTMP Publisher
	* based on cpu tick, not wall time, since system may suspend and wall time never stop.
	* based on time diff between dts and elapsed time since first packet
17. [ ] **Dynamic stream management (task management across processes and threads)**
	* subprocess/task_threads_chain management
	* subprocess quit if all task done
	* create subprocess if new stream added
	* master process push task to subprocess if subprocess has quota
	* subprocess report to master process if task done
	* master process ask subprocess for status
	* master process listen for external stream management event: add/remove stream, quit
18. [ ] configure with nginx style like config file
19. [ ] web based master process management, not time critical, no need to be implemented in C++
	* prefer python/nodejs for backend
	* prefer Vue/bootstrapi + jQuery for frontend
	* status monitor for each stream: started/ongoing/done, errors, fall behind?
	* stream controller: add/remove


