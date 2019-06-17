#ifndef RS_ORION_DEVICE_H_
#define RS_ORION_DEVICE_H_

#include "rs_def.h"
#include "rs_image.h"
#include "rs_face.h"
#include "rs_device.h"

// readface sdk
#include "rsFaceSDKv1.h"
// facesearch sdk
#include "rsFaceSearch.h"
// facedevice sdk
#include "facedevice_client.h"

// readface sdk
typedef void* HLicense;
typedef void* HFaceDetect;
typedef void* HFaceTrack;
typedef void* HFaceAttribute;
typedef void* HFaceQuality;

// facesearch sdk
typedef void* HFaceSearch;

// facedevice sdk
typedef void* HFaceDevice;

STRUCT(RSOrionHandle) {
    HLicense    hLicense;
    HFaceSearch hFaceSearch;
    HFaceDevice hFaceDevice;

    RShandle    hInner;
};

STRUCT(RSOrionInitParam) {
    // readface license
    char appId[BIG_STRING_LEN];
    char appSecret[BIG_STRING_LEN];
    // facesearch database
    char facesearch_dbfile[PATH_LEN];
    // facedevice database
    char facedevice_dbfile[PATH_LEN];
};

STRUCT(RSOrionDBFace) {
    int  face_id;
    char face_uuid[UUID_LEN];

    int  person_id;
    char person_uuid[UUID_LEN];

    int  group_id;
    char group_uuid[UUID_LEN];

    char name[STRING_LEN];
    int  age;
    int  gender;

    char phone[STRING_LEN];
    char email[STRING_LEN];

    char image_url[PATH_LEN];
    char avatar_url[PATH_LEN];
};

BEGIN_EXTERN_C

RS_API const char* CALLSPEC rsOrionSdkVersion();
RS_API const char* CALLSPEC rsOrionErrString(int errcode);

RS_API int CALLSPEC rsOrionInit(RSOrionHandle* ph, RSOrionInitParam* param);
RS_API int CALLSPEC rsOrionCleanup(RSOrionHandle* ph);

//===============================facedevice sdk=================================================
RS_API int CALLSPEC rsOrionDeviceSetSrvaddr(RSOrionHandle* ph, const char* ip, int port);
RS_API int CALLSPEC rsOrionDeviceSetDeviceInfo(RSOrionHandle* ph, rs_device* device);
RS_API int CALLSPEC rsOrionDeviceSetConnectStateCB(RSOrionHandle* ph, connect_state_cb cb, void* userdata);
RS_API int CALLSPEC rsOrionDeviceSetGateControlCB(RSOrionHandle* ph, gate_control_cb cb, void* userdata);

RS_API int CALLSPEC rsOrionDeviceSetRecogMode(RSOrionHandle* ph, rs_recog_mode mode);

RS_API int CALLSPEC rsOrionDeviceConnect(RSOrionHandle* ph);
RS_API int CALLSPEC rsOrionDeviceDisconnect(RSOrionHandle* ph);

RS_API int CALLSPEC rsOrionDeviceSendRecogRecord(RSOrionHandle* ph, rs_recog_record* record);

//==============================readface sdk=====================================================

// FaceDetect
RS_API int CALLSPEC rsOrionFaceDetectInit(HLicense hLicense, HFaceDetect* phFaceDetect);
RS_API int CALLSPEC rsOrionFaceDetect(HFaceDetect hFaceDetect, rs_frame* frame,
                                    rs_face_info** faces, int* faces_num);
RS_API int CALLSPEC rsOrionFaceDetectUninit(HFaceDetect* phFaceDetect);

// FaceTrack
RS_API int CALLSPEC rsOrionFaceTrackInit(HLicense hLicense, HFaceTrack* phFaceTrack);
RS_API int CALLSPEC rsOrionFaceTrack(HFaceTrack hFaceTrack, rs_frame* frame,
                                    rs_face_info** faces, int* faces_num);
RS_API int CALLSPEC rsOrionFaceTrackUninit(HFaceTrack* phFaceTrack);

RS_API int CALLSPEC rsOrionFaceInfoFree(rs_face_info* faces, int faces_num);

// FaceAttribute
RS_API int CALLSPEC rsOrionFaceAttributeInit(HLicense hLicense, HFaceAttribute* phFaceAttr);
RS_API int CALLSPEC rsOrionFaceAttributeDetect(HFaceAttribute hFaceAttr,
                                    rs_frame* frame, rs_face_info* face,
                                    rs_face_attr* face_attr);
RS_API int CALLSPEC rsOrionFaceAttributeUninit(HFaceAttribute* phFaceAttr);

// FaceQuality
RS_API int CALLSPEC rsOrionFaceQualityInit(HLicense hLicense, HFaceQuality* phFaceQuality);
RS_API int CALLSPEC rsOrionFaceQualityScore(HFaceQuality hFaceQuality,
                                    rs_frame* frame, rs_face_info* face,
                                    int* quality);
RS_API int CALLSPEC rsOrionFaceQualityUninit(HFaceQuality* phFaceQuality);

//=============================facesearch sdk=========================================
RS_API int CALLSPEC rsOrionFaceSearch(RSOrionHandle* ph,
                                    rs_frame* frame, rs_face_info* face,
                                    int top, float threshold,
                                    rsFaceSearchResult* results, int* results_num);

// rsOrionFaceSearch(top=1, threshold=0.7)
RS_API int CALLSPEC rsOrionFaceRecognize(RSOrionHandle* ph,
                                    rs_frame* frame, rs_face_info* face,
                                    FaceID* faceID, float* similarity);

//============================local database===========================================
RS_API int CALLSPEC rsOrionDeviceDBGetFace(RSOrionHandle* ph, FaceID faceID, RSOrionDBFace* face);

END_EXTERN_C

#endif // RS_FACE_DEVICE_H_

