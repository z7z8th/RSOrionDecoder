#ifndef __RS_FFMPEG_UTIL_H__
#define __RS_FFMPEG_UTIL_H__

#include "hw/hframe.h"
#include "hw/hgl.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
}

#endif // __RS_FFMPEG_UTIL_H__