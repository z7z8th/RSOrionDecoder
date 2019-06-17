#ifndef RS_IMAGE_H_
#define RS_IMAGE_H_

#include "rs_def.h"

ENUM(rs_image_type) {
    IMAE_TYPE_UNKNOWN   = 0,

    BMP     = 1,
    JPEG    = 2,
    PNG     = 3,

    PIXELS  = 100,
};

ENUM(rs_pixel_format) {
    PIX_FMT_GRAY        = 0,
    PIX_FMT_BGR888      = 1,
    PIX_FMT_NV21_RS        = 2,
    PIX_FMT_BGRA8888    = 3,
    PIX_FMT_NV12_RS        = 4,
    PIX_FMT_I420        = 5,
    PIX_FMT_YV12        = 6,
    PIX_FMT_422P        = 7,
};

ENUM(rs_image_rotation) {
    RS_IMG_CLOCKWISE_ROTATE_0,
    RS_IMG_CLOCKWISE_ROTATE_90,
    RS_IMG_CLOCKWISE_ROTATE_180,
    RS_IMG_CLOCKWISE_ROTATE_270
};

/*
 * @usage
 * 1.
 * rs_image image1;
 * memset(&image1, 0, sizeof(rs_image));
 * strncpy(image1.filename, "./pictures/test.jpg", sizeof(image1.filename));
 *
 * 2.
 * rs_image image2;
 * memset(&image2, 0, sizeof(rs_image));
 * image2.image_type = JPEG;
 * FILE* fp = fopen("./pictures/test.jpg", "rb");
 * if (fp) {
 *     fseek(fp, 0, SEEK_END);
 *     image2.size = ftell(fp);
 *     fseek(fp, 0, SEEK_SET);
 *     image2.data = malloc(image2.size);
 *     fread(image2.data, 1, image2.size, fp);
 *     fclose(fp);
 * }
 *
 * 3.
 * rs_image image3;
 * memset(&image3, 0, sizeof(rs_image));
 * cv::Mat mat = imread("./pictures/test.jpg");
 * image3.image_type = PIXELS;
 * image3.pix_fmt = PIX_FMT_BGR888;
 * image3.data = mat.data;
 * image3.width = mat.cols;
 * image3.height = mat.rows;
 * image3.pitch = mat.cols * 3;
 *
 * @note
 * if rs_image.data use base64 encode,
 * set image.base64 = 1, image.size = strlen(image.data)
 */
STRUCT(rs_image) {
    rs_image_type   image_type;
    rs_pixel_format pix_fmt;
    int     base64;

    int     size;
    void*   data;

    int     width;
    int     height;
    int     pitch;
    int     bpp;

    char    filename[PATH_LEN];
    char    image_url[PATH_LEN];

    // ...
    RESERVED_FIELD(8)
};

// for rsFaceSDK
STRUCT(rs_frame) {
    int width;
    int height;
    int pitch;
    rs_pixel_format pix_fmt;
    unsigned char* data;
};

#endif // RS_IMAGE_H_

