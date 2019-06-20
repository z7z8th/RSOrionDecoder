#ifndef __RS_FFMPEG_ENCODER_TASK_H__
#define __RS_FFMPEG_ENCODER_TASK_H__

#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"
#include "libutils/hmedia.h"
#include "rs_frame.h"
#include "rs_date.h"
#include "rs_chained_io_if.h"

class RSFFMpegEncoderTask : 
    public RSChainedIOTask<spRSAVFramePacket>
{
public:
	RSFFMpegEncoderTask();
	virtual ~RSFFMpegEncoderTask();
protected:
	virtual void Run();
public:
	void	Init();
	int		Encode();
	int		GetCodec();

	int		GetFps();
	void	SetMedia(HMedia hMedia);
    int     set_hwframe_ctx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx);
    int     encode_write(AVCodecContext *avctx, AVFrame *frame, FILE *fout);

private:
	void	cleanup();
	int		getVideoSource();
protected:
    AVFrame *sw_frame = NULL;
    AVFrame *hw_frame = NULL;
    AVCodecContext *avctx = NULL;
    AVCodec *codec = NULL;
    AVBufferRef *hw_device_ctx = NULL;
    const char *enc_name = "h264_vaapi";

    int width_;
    int height_;
    enum AVPixelFormat input_format_;
    std::string src_;
    int fps_ = VIDEO_FALL_BACK_FPS;
    AVRational time_base_;
};

RSFFMpegEncoderTask::RSFFMpegEncoderTask()
{
}

RSFFMpegEncoderTask::~RSFFMpegEncoderTask()
{
	cleanup();
}

void RSFFMpegEncoderTask::Init()
{
/* 	av_register_all();
	avcodec_register_all();
 */
}

void RSFFMpegEncoderTask::cleanup()
{
    av_frame_free(&sw_frame);
    av_frame_free(&hw_frame);
    avcodec_free_context(&avctx);
    av_buffer_unref(&hw_device_ctx);
}

void RSFFMpegEncoderTask::SetMedia(HMedia hMedia)
{
}

int RSFFMpegEncoderTask::set_hwframe_ctx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx)
{
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx = NULL;
    int err = 0;

    if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx))) {
        fprintf(stderr, "Failed to create VAAPI frame context.\n");
        return -1;
    }
    frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
    frames_ctx->format    = AV_PIX_FMT_VAAPI;
    frames_ctx->sw_format = AV_PIX_FMT_NV12;
    frames_ctx->width     = width_;
    frames_ctx->height    = height_;
    frames_ctx->initial_pool_size = 20;
    if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0) {
        fprintf(stderr, "Failed to initialize VAAPI frame context."
                "Error code: %s\n",av_err2str(err));
        av_buffer_unref(&hw_frames_ref);
        return err;
    }
    ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
    if (!ctx->hw_frames_ctx)
        err = AVERROR(ENOMEM);

    av_buffer_unref(&hw_frames_ref);
    return err;
}

int RSFFMpegEncoderTask::GetCodec() {
    int err;
    err = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_VAAPI,
                                 NULL, NULL, 0);
    if (err < 0) {
        fprintf(stderr, "Failed to create a VAAPI device. Error code: %s\n", av_err2str(err));
        goto close;
    }

    if (!(codec = avcodec_find_encoder_by_name(enc_name))) {
        fprintf(stderr, "Could not find encoder.\n");
        err = -1;
        goto close;
    }

    if (!(avctx = avcodec_alloc_context3(codec))) {
        err = AVERROR(ENOMEM);
        goto close;
    }

    avctx->width     = width_;
    avctx->height    = height_;
    avctx->time_base = (AVRational){1, 25};
    avctx->framerate = (AVRational){25, 1};
    avctx->sample_aspect_ratio = (AVRational){1, 1};
    avctx->pix_fmt   = AV_PIX_FMT_VAAPI;

    /* set hw_frames_ctx for encoder's AVCodecContext */
    if ((err = set_hwframe_ctx(avctx, hw_device_ctx)) < 0) {
        fprintf(stderr, "Failed to set hwframe context.\n");
        goto close;
    }

    if ((err = avcodec_open2(avctx, codec, NULL)) < 0) {
        fprintf(stderr, "Cannot open video encoder codec. Error code: %s\n", av_err2str(err));
        goto close;
    }

    return 0;

close:
    av_frame_free(&sw_frame);
    av_frame_free(&hw_frame);
    avcodec_free_context(&avctx);
    av_buffer_unref(&hw_device_ctx);
    return err;
}


int RSFFMpegEncoderTask::encode_write(AVCodecContext *avctx, AVFrame *frame, FILE *fout)
{
    int ret = 0;
    AVPacket enc_pkt;

    av_init_packet(&enc_pkt);
    enc_pkt.data = NULL;
    enc_pkt.size = 0;

    if ((ret = avcodec_send_frame(avctx, frame)) < 0) {
        fprintf(stderr, "Error code: %s\n", av_err2str(ret));
        goto end;
    }
    while (1) {
        ret = avcodec_receive_packet(avctx, &enc_pkt);
        if (ret)
            break;

        enc_pkt.stream_index = 0;
        if (enc_pkt.pts == AV_NOPTS_VALUE)
            enc_pkt.pts = sw_frame->pts;
        if (enc_pkt.dts == AV_NOPTS_VALUE)
            enc_pkt.dts = sw_frame->pkt_dts;
        if (fout)
            ret = fwrite(enc_pkt.data, enc_pkt.size, 1, fout);
        std::shared_ptr<RSAVFramePacket> avpacket(new RSAVFramePacket);
        avpacket->AllocPacket().PacketMoveRefFrom(&enc_pkt);
        avpacket->src_ = src_;
        avpacket->fps_ = fps_;
        avpacket->time_base = time_base_;
        Output(avpacket);
    }

end:
    ret = ((ret == AVERROR(EAGAIN)) ? 0 : -1);
    return ret;
}


int RSFFMpegEncoderTask::Encode() {
    int err;
    // int size   = width_ * height_;
    FILE *fout = NULL;

    /*     
    fout = fopen("rs_ffmpeg_encoder.h264", "w+");
    if (!fout) {
        std::cout << "error open encoder dump file." << std::endl;
        return -1;
    }
    */

    while (!ShouldStop()) {
        if (!(sw_frame = av_frame_alloc())) {
            err = AVERROR(ENOMEM);
            goto close;
        }
        /* read data into software frame, and transfer them into hw frame */
        /*
        sw_frame->width  = width_;
        sw_frame->height = height_;
        sw_frame->format = AV_PIX_FMT_NV12;
        if ((err = av_frame_get_buffer(sw_frame, 32)) < 0)
            goto close;
        if ((err = fread((uint8_t*)(sw_frame->data[0]), size, 1, fin)) <= 0)
            break;
        if ((err = fread((uint8_t*)(sw_frame->data[1]), size/2, 1, fin)) <= 0)
            break;
        */
        std::shared_ptr<RSAVFramePacket> avdata = Input();
        if (!avdata) {
            goto close;
        }
        fps_ = avdata->fps_;
        time_base_ = avdata->time_base;
        avdata->FrameMoveRefTo(sw_frame);

        //printf("encode frame \n");

        if (!(hw_frame = av_frame_alloc())) {
            err = AVERROR(ENOMEM);
            goto close;
        }
        if ((err = av_hwframe_get_buffer(avctx->hw_frames_ctx, hw_frame, 0)) < 0) {
            fprintf(stderr, "Error code: %s.\n", av_err2str(err));
            goto close;
        }
        if (!hw_frame->hw_frames_ctx) {
            err = AVERROR(ENOMEM);
            goto close;
        }
        if ((err = av_hwframe_transfer_data(hw_frame, sw_frame, 0)) < 0) {
            fprintf(stderr, "Error while transferring frame data to surface."
                    "Error code: %s.\n", av_err2str(err));
            goto close;
        }

        if ((err = (encode_write(avctx, hw_frame, fout))) < 0) {
            fprintf(stderr, "Failed to encode.\n");
            goto close;
        }
        av_frame_free(&hw_frame);
        av_frame_free(&sw_frame);
    }

    /* flush encoder */
    err = encode_write(avctx, NULL, fout);
    if (err == AVERROR_EOF)
        err = 0;

close:
    if (fout)
        fclose(fout);
    av_frame_free(&sw_frame);
    av_frame_free(&hw_frame);
    avcodec_free_context(&avctx);
    av_buffer_unref(&hw_device_ctx);

    return err;
}

void RSFFMpegEncoderTask::Run() {
    {
        std::shared_ptr<RSAVFramePacket> avdata = _Peek();
        width_ = avdata->frame_->width;
        height_ = avdata->frame_->height;
        src_ = avdata->src_;
    }
    
    Init();
    if (!GetCodec()) {
        Encode();
    } else {
        std::cout << "Get encode codec failed.\n" << std::endl;
    }

	FinishTaskChain();
    OutputDateTime();
    std::cout << "RSFFMpegEncoderTaskTask::Run exited." << std::endl;
}

#endif //__RS_FFMPEG_ENCODER_TASK_H__