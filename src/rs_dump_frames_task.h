#ifndef __RS_DUMP_FRAMES_TASK_H__
#define __RS_DUMP_FRAMES_TASK_H__

#include <iostream>
#include <fstream>
#include <sstream>
//#include <experimental/filesystem>
#include "hw/hlog.h"
#include "hw/hframe.h"
#include "rs_ffmpeg_util.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_frame.h"
#include "rs_path.h"

class RSDumpFramesTask : /* public RSDumpFrames, */
	public readsense::CRunnableAloneTaskList<std::shared_ptr<RSAVFramePacket>>
{
public:
	RSDumpFramesTask(const std::string &prefix, const std::string &format);
	~RSDumpFramesTask();
	void DiscardData(bool discard) {
		discardData_ = discard;
	}

public:
	virtual void Run();
protected:
	std::string GetOutputFileName(const std::string &mediaSrc);

	std::string prefix_;
	std::string format_;
	std::string ofname_;
	bool discardData_ = false;
};

RSDumpFramesTask::RSDumpFramesTask(const std::string &prefix, const std::string &format)
	: prefix_(prefix), format_(format) {

}

RSDumpFramesTask::~RSDumpFramesTask() {
}

std::string RSDumpFramesTask::GetOutputFileName(const std::string &mediaSrc){
	if (discardData_)
		return "/dev/null";
	//std::experimental::filesystem::path path(rsFrame.src_);
	std::string tmpName(prefix_);
	//tmpName = path.filename().string();
	tmpName += base_name_no_ext(mediaSrc) + "." + format_;

	std::thread::id tid = std::this_thread::get_id();
	std::stringstream tid_str;
	tid_str << ".tid_" << std::hex << tid;
	tmpName += tid_str.str();
	
	return tmpName;
}

void RSDumpFramesTask::Run() {
	std::ofstream of;
	while (!stop_flag_) {
		std::shared_ptr<RSAVFramePacket> avdata(_Pop());

		if (!of.is_open()) {
			if (avdata->frame_)
				format_ = avdata->FramePixelFormat();
			ofname_ = GetOutputFileName(avdata->src_);
			std::cout << std::endl << "dump frames to " << ofname_ << std::endl;
			if (!discardData_) {
				std::cout << "how to play:\n";
				if (avdata->frame_)
					std::cout << "\tffplay -f rawvideo -pix_fmt " << format_ << " -s " << avdata->frame_->width << "x" << avdata->frame_->height << " " << ofname_ << std::endl;
				else if (avdata->packet_)
					std::cout << "\tffplay "<< ofname_ << std::endl;
				std::cout << std::endl;
			}

			of.open(ofname_, std::ios::binary | std::ios::out);
			if (!of.is_open())
				std::cout << "can't open " << ofname_ << std::endl;
		}

#warning get buffer for frame
		if (avdata->frame_) {
			int ret;
			AVFrame *tmp_frame = avdata->frame_;
			int size = av_image_get_buffer_size(static_cast<enum AVPixelFormat>(tmp_frame->format), tmp_frame->width,
											tmp_frame->height, 1);
			uint8 *buffer = static_cast<uint8 *>(av_malloc(size));
			if (!buffer) {
				fprintf(stderr, "Can not alloc buffer\n");
				ret = AVERROR(ENOMEM);
				goto fail;
			}
#warning probably unnecessary buffer copy
			ret = av_image_copy_to_buffer(buffer, size,
										(const uint8_t * const *)tmp_frame->data,
										(const int *)tmp_frame->linesize, static_cast<enum AVPixelFormat>(tmp_frame->format),
										tmp_frame->width, tmp_frame->height, 1);
			if (ret < 0) {
				fprintf(stderr, "Can not copy image to buffer\n");
				goto fail;
			}
			//std::cout << "dump frame buffer " << static_cast<void *>(buffer) << " size " << size <<std::endl;
			of.write(reinterpret_cast<const char *>(buffer), size);
			if (!of)
				std::cout << "write " << static_cast<void *>(buffer) << " size " << size << " to file error." << std::endl;
			av_freep(&buffer);
		} else if (avdata->packet_) {
			//std::cout << "dump packet" <<std::endl;
			of.write(reinterpret_cast<const char *>(avdata->packet_->data), avdata->packet_->size);
		}

		if (sp_next_task_ != nullptr) {
			sp_next_task_->Push(std::move(avdata));
		}
	}
fail:
	return;
}

#endif // __RS_DUMP_FRAMES_TASK_H__