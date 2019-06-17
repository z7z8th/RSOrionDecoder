#ifndef RS_BASE_H_
#define RS_BASE_H_

#include "rs_def.h"

STRUCT(rs_point) {
    float x, y;
};

STRUCT(rs_size) {
    int     width;
    int     height;
};

STRUCT(rs_rect) {
    int     x;
    int     y;
    int     width;
    int     height;
};

#endif // RS_BASE_H_

