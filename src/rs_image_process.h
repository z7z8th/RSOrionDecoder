#ifndef __RS_IMAGE_PROCESS_H__
#define __RS_IMAGE_PROCESS_H__

#include <string>

#include "hw/hdef.h"
#include "libreadsense/rs_base.h"

namespace readsense {

static rs_rect CutoutFaceRect(rs_rect frame_rect,
	rs_rect face_rect, float ratio = 0.5);

static int CutoutFaceImage(uint8 *frame_image, uint8 *face_image,
	rs_rect face_rect, rs_rect frame_rect);



rs_rect CutoutFaceRect(rs_rect frame_rect,
	rs_rect face_rect, float ratio) {
	int face_x = face_rect.x;
	int face_y = face_rect.y;
	int face_width = face_rect.width;
	int face_height = face_rect.height;

	int frame_x = 0;
	int frame_y = 0;
	int frame_width = frame_rect.width;
	int frame_height = frame_rect.height;

	face_x = face_x - face_width * ratio;
	face_y = face_y - face_height * ratio;
	face_x = std::max(face_x, frame_x);
	face_y = std::max(face_y, frame_y);

	float padding_ratio = 1 + ratio * 2;
	face_width = static_cast<int>(face_width * padding_ratio);
	face_height = static_cast<int>(face_height * padding_ratio);

	if (face_x + face_width >= frame_width) {
		face_width = frame_width - face_x;
	}

	if (face_y + face_height >= frame_height) {
		face_height = frame_height - face_y;
	}

	face_width = face_height = std::min(face_width, face_height);
	rs_rect rect = { face_x, face_y, face_width, face_height };

	return rect;
}

int CutoutFaceImage(
	uint8 *frame_image, uint8 *face_image,
	rs_rect face_rect, rs_rect frame_rect) {

	int x = face_rect.x;
	int y = face_rect.y;
	int width = face_rect.width;
	int height = face_rect.height;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width * 3; ++j) {
			face_image[i * width * 3 + j] = \
				frame_image[(y + i) * frame_rect.width * 3 + x * 3 + j];
		}
	}

	return 0;
}

}	// namespace readsense


#endif // __RS_IMAGE_PROCESS_H__
