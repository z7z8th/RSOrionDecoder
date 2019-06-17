#ifndef RS_MISC_H_
#define RS_MISC_H_

#include "rs_def.h"

STRUCT(rs_buf) {
    int     len;
    void*   ptr;
};

STRUCT(rs_custom_request) {
    char    cmd[STRING_LEN];
    rs_buf  param;
};

STRUCT(rs_custom_response) {
    rs_buf  data;
};

STRUCT(rs_custom_event) {
    char    type[STRING_LEN];
    rs_buf  data;
};

#endif // RS_MISC_H_

