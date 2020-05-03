#ifndef __RS_AVFRAME_H__
#define __RS_AVFRAME_H__

#include <memory>
#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"

const int VIDEO_FALL_BACK_FPS = 25;

struct RSAVInfo {
    std::string   src_;
    AVRational    time_base = { 0, 0 };
    int fps_ = VIDEO_FALL_BACK_FPS;
};

class RSAVFrame {
public:
    RSAVFrame() = default;
    RSAVFrame(AVFrame* frame)
        : frame_(frame)
    {
    }
    /*
    RSAVFrame(int width, int height, enum AVPixelFormat format)
    {
        frame_ = av_frame_alloc();
        frame_->width = width;
        frame_->height = height;
        frame_->format = format;
        av_frame_get_buffer(frame_, 32);
    }
    */
    ~RSAVFrame()
    {
        av_frame_free(&frame_);
    }
    /*
    AVFrame* operator->() const
    {
        return frame_;
    }
    void moveDataTo(AVFrame *dst) {
        dst->data[0] = frame_->data[0];
        dst->data[1] = frame_->data[1];
        dst->linesize[0] = frame_->linesize[0];
        dst->linesize[1] = frame_->linesize[1];

        frame_->data[0] = 0;
        frame_->data[1] = 0;
        frame_->linesize[0] = 0;
        frame_->linesize[1] = 0;
    }
    */
    RSAVFrame& AllocFrame(){
        frame_ = av_frame_alloc();
        return *this;
    }
    void MoveRefTo(AVFrame *dst) {
        av_frame_move_ref(dst, frame_);
    }
    void MoveRefFrom(AVFrame *src) {
        av_frame_move_ref(frame_, src);
    }
    std::string PixelFormat() {
        if (frame_)
            return std::string(av_get_pix_fmt_name(static_cast<enum AVPixelFormat>(frame_->format)));
        return "NoFrame";
    }

    RSAVInfo info_;
//protected:
    AVFrame* frame_ = NULL;
};
using spRSAVFrame = std::shared_ptr<RSAVFrame>;

class RSAVPacket {
public:
    RSAVPacket() = default;
    RSAVPacket(AVPacket* packet)
        : packet_(packet)
    {
    }
    ~RSAVPacket()
    {
        av_packet_free(&packet_);
    }
    RSAVPacket& AllocPacket(){
        packet_ = av_packet_alloc();
        return *this;
    }
    void MoveRefTo(AVPacket *dst) {
        av_packet_move_ref(dst, packet_);
    }
    void MoveRefFrom(AVPacket *src) {
        av_packet_move_ref(packet_, src);
    }

    RSAVInfo info_;

//protected:
    AVPacket* packet_ = NULL;
};
using spRSAVPacket = std::shared_ptr<RSAVPacket>;

struct RSAVEvent {
    RSAVEvent() = default;
    RSAVEvent(RSAVFrame *frame) {
        evt_ = EVENT_FRAME;
        data_ = std::shared_ptr<RSAVFrame>(frame);
    }
    RSAVEvent(RSAVPacket *packet) {
        evt_ = EVENT_PACKET;
        data_ = std::shared_ptr<RSAVPacket>(packet);
    }
    virtual ~RSAVEvent() {}
    spRSAVFrame GetFrame() {
        if (evt_ != EVENT_FRAME)
            return nullptr;
        return std::static_pointer_cast<RSAVFrame>(data_);
    }
    spRSAVPacket GetPacket() {
        if (evt_ != EVENT_PACKET)
            return nullptr;
        return std::static_pointer_cast<RSAVPacket>(data_);
    }
/*
    bool IsFrameEvent() {
        return evt_ == EVENT_FRAME;
    }
    bool IsPacketEvent() {
        return evt_ == EVENT_PACKET;
    }
*/
    bool IsExitEvent() {
        return evt_ == EVENT_EXIT;
    }
    bool IsAbortEvent() {
        return evt_ == EVENT_ABORT;
    }
    enum EventType{
        EVENT_NONE = 0,
        EVENT_FRAME,
        EVENT_PACKET,
        EVENT_EXIT,
        EVENT_ABORT,
    };
    enum EventType evt_ = EVENT_NONE;
    std::shared_ptr<void> data_;
};

using spRSAVEvent = std::shared_ptr<RSAVEvent>;

#endif //__RS_AVFRAME_H__