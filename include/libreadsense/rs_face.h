#ifndef RS_FACE_H_
#define RS_FACE_H_

#include "rs_def.h"
#include "rs_base.h"

#define FACE_LANDMARKS_NUM   21
#define FACE_FEATURES_NUM    512

STRUCT(rs_face_info) {
    rs_rect     rect;
    rs_point    landmarks[FACE_LANDMARKS_NUM];
    float       yaw;
    float       pitch;
    float       roll;
    int         trackId; // used by FaceTrack
    float       confidence;
    float       score;
};

STRUCT(rs_face_attr) {
    int glasses;
    int mouth_open;
    int gender;
    int age;
    int beauty;
    int eye_close;
    int smile;
};

STRUCT(rs_face_feature) {
    int     version;
    float   features[FACE_FEATURES_NUM];
};

STRUCT(rs_face) {
    int     face_id;
    char    face_uuid[UUID_LEN];

    rs_face_info    face_info;
    rs_face_attr    face_attr;
    rs_face_feature face_feature;

    int     rgb_liveness;
    int     nir_liveness;
    int     depth_liveness;

    char    image_url[PATH_LEN];
    char    avatar_url[PATH_LEN];

    // ...
    RESERVED_FIELD(8)
};

STRUCT(rs_faceid) {
    int     face_id;
    char    face_uuid[UUID_LEN];
};

STRUCT(rs_face_list) {
    /// faces: rs_faceid array pointer
    /// alloc and free by caller
    rs_faceid*  faces;
    int         faces_num;
};

// for FaceSearch
typedef int FaceID;
STRUCT(rsFaceSearchResult) {
    FaceID faceID;
    float  confidence;
};

#endif // RS_FACE_H_

