#ifndef RS_RECOG_H_
#define RS_RECOG_H_

#include "rs_def.h"
#include "rs_image.h"

STRUCT(rs_recog_mode) {
    int mode;
};

STRUCT(rs_recog_record) {
    rs_image    image;
    RSuint64    timestamp;

    int     ok;
    int     person_id;
    char    person_uuid[UUID_LEN];
    int     face_id;
    char    face_uuid[UUID_LEN];
    float   confidence;

    char    fail_reason[STRING_LEN];

    // ...
    RESERVED_FIELD(8)
};

#endif // RS_RECOG_H_

