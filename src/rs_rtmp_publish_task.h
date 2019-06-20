#ifndef __RS_RTMP_PUBLISH_TASK_H__
#define __RS_RTMP_PUBLISH_TASK_H__

#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"
#include "rs_avframe_cxx.h"
#include "srs_librtmp.h"
#include "rs_date.h"
#include "rs_chained_io_if.h"

class RSRtmpPublishTask : 
    public RSChainedIOTask<spRSAVFramePacket>
{
public:
	RSRtmpPublishTask();
	virtual ~RSRtmpPublishTask();
	void	SetRtmpServer(const std::string &rtmp_server, const std::string &path, int index);
protected:
    virtual void Run();
	int		ConnectRtmpServer();
	void    PublishStream();
private:
	void	Cleanup();
protected:

    std::string rtmp_url;
	srs_rtmp_t rtmp;

    int width_;
    int height_;
    enum AVPixelFormat input_format_;
    std::string src_;
};

RSRtmpPublishTask::RSRtmpPublishTask()
{
}

RSRtmpPublishTask::~RSRtmpPublishTask()
{
	Cleanup();
}

void RSRtmpPublishTask::Cleanup()
{
}

void RSRtmpPublishTask::SetRtmpServer(const std::string &rtmp_server, const std::string &path, int index) {
	rtmp_url = rtmp_server + path + std::to_string(index);
	std::cout << "rtmp_url " << rtmp_url << std::endl;
}

int RSRtmpPublishTask::ConnectRtmpServer() {
	// connect rtmp context
    rtmp = srs_rtmp_create(rtmp_url.c_str());
    
    if (srs_rtmp_handshake(rtmp) != 0) {
        srs_human_trace("simple handshake failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("simple handshake success");
    
    if (srs_rtmp_connect_app(rtmp) != 0) {
        srs_human_trace("connect vhost/app failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("connect vhost/app success");
    
    if (srs_rtmp_publish_stream(rtmp) != 0) {
        srs_human_trace("publish stream failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("publish stream success");

	return 0;

rtmp_destroy:
    srs_rtmp_destroy(rtmp);
	return -1;
}

//#undef srs_human_trace
//#define srs_human_trace(x...) do {} while(0)

void RSRtmpPublishTask::PublishStream() {
	int64_t count = 0;

	while (!ShouldStop()) {
		std::shared_ptr<RSAVFramePacket> avdata(Input());
        if (!avdata) {
            goto rtmp_destroy;
        }
		AVPacket *pkt = avdata->packet_;
        // send out the h264 packet over RTMP
        int ret = srs_h264_write_raw_frames(rtmp,
                                            reinterpret_cast<char *>(pkt->data),
                                            pkt->size,
                                            pkt->dts*av_q2d(avdata->time_base)*1000,
                                            pkt->pts*av_q2d(avdata->time_base)*1000);
        if (ret != 0) {
            #warning ignore duplicated sps and pps will lead to lag
            if (srs_h264_is_dvbsp_error(ret)) {
                srs_human_trace("ignore drop video error, code=%d", ret);
            } else if (srs_h264_is_duplicated_sps_error(ret)) {
                //srs_human_trace("ignore duplicated sps, code=%d", ret);
            } else if (srs_h264_is_duplicated_pps_error(ret)) {
                //srs_human_trace("ignore duplicated pps, code=%d", ret);
            } else {
                srs_human_trace("send h264 raw data failed. ret=%d", ret);
                goto rtmp_destroy;
            }
        }
#if 0
        // 5bits, 7.3.1 NAL unit syntax,
        // H.264-AVC-ISO_IEC_14496-10.pdf, page 44.
        //  7: SPS, 8: PPS, 5: I Frame, 1: P Frame, 9: AUD, 6: SEI
        //u_int8_t nut = (char)data[nb_start_code] & 0x1f;
        srs_human_trace("sent packet: type=%s, dts=%ld, pts=%ld, size=%d, fps=%.2f" /* ", b[%d]=%#x(%s)" */,
            srs_human_flv_tag_type2string(SRS_RTMP_TYPE_VIDEO), static_cast<int64_t>(pkt->dts*1000/fps), static_cast<int64_t>(pkt->pts*1000/fps), pkt->size, fps
			/* ,
			nb_start_code,
			(char)data[nb_start_code],
            (nut == 7? "SPS":(nut == 8? "PPS":(nut == 5? "I":(nut == 1? "P":(nut == 9? "AUD":(nut == 6? "SEI":"Unknown")))))) */
			);
#endif
        // @remark, when use encode device, it not need to sleep.
        if (count++ == 9) {
            usleep(1000 * 1000 * count / avdata->fps_);
            count = 0;
        }
		Output(std::move(avdata));
	}
rtmp_destroy:
    srs_rtmp_destroy(rtmp);
}


void RSRtmpPublishTask::Run() {
    if (!ConnectRtmpServer()) {
        PublishStream();
    } else {
        std::cout << "ConnectRtmpServer Failed." << std::endl;
    }

	FinishTaskChain();
	OutputDateTime();
    std::cout << "RSRtmpPublishTask::Run exited." << std::endl;
}

#endif //__RS_RTMP_PUBLISH_TASK_H__