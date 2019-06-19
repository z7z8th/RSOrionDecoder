#ifndef __RS_AVFRAME_H__
#define __RS_AVFRAME_H__

#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"

const int VIDEO_FALL_BACK_FPS = 25;

class RSAVFramePacket {
public:
    RSAVFramePacket() = default;
    RSAVFramePacket(AVFrame* frame)
        : frame_(frame)
    {
    }
    RSAVFramePacket(AVPacket* packet)
        : packet_(packet)
    {
    }
    /*
    RSAVFramePacket(int width, int height, enum AVPixelFormat format)
    {
        frame_ = av_frame_alloc();
        frame_->width = width;
        frame_->height = height;
        frame_->format = format;
        av_frame_get_buffer(frame_, 32);
    }
    */
    ~RSAVFramePacket()
    {
        av_frame_free(&frame_);
        av_packet_free(&packet_);
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
    RSAVFramePacket& AllocFrame(){
        frame_ = av_frame_alloc();
        return *this;
    }
    RSAVFramePacket& AllocPacket(){
        packet_ = av_packet_alloc();
        return *this;
    }
    void FrameMoveRefTo(AVFrame *dst) {
        av_frame_move_ref(dst, frame_);
    }
    void FrameMoveRefFrom(AVFrame *src) {
        av_frame_move_ref(frame_, src);
    }
    void PacketMoveRefTo(AVPacket *dst) {
        av_packet_move_ref(dst, packet_);
    }
    void PacketMoveRefFrom(AVPacket *src) {
        av_packet_move_ref(packet_, src);
    }
    std::string FramePixelFormat() {
        if (frame_)
            return std::string(av_get_pix_fmt_name(static_cast<enum AVPixelFormat>(frame_->format)));
        return "";
    }
    bool IsInvalidForEOF() {
        return !frame_ && !packet_;
    }

    std::string src_;
    int fps_ = VIDEO_FALL_BACK_FPS;
    AVRational time_base = { 0, 0 };

//protected:
    AVFrame* frame_ = NULL;
    AVPacket* packet_ = NULL;
};

#endif //__RS_AVFRAME_H__