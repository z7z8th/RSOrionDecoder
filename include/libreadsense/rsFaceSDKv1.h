#ifndef RS_FACE_SDK_V1_H_
#define RS_FACE_SDK_V1_H_

#include "rs_def.h"
#include "rs_base.h"
#include "rs_image.h"
#include "rs_face.h"

BEGIN_EXTERN_C

RS_API char * CALLSPEC rsGetRSFaceSDKVersion();

/*
 * licence manager
 */
RS_API int CALLSPEC rsInitLicenseManager(RShandle *handle, const char *appId, const char* appSecret);
RS_API const char *rsGetSDKLicenseDeviceKey(); // must be called after init
RS_API void CALLSPEC rsUnInitLicenseManager(RShandle *handle);

/*
 * face detect
 */
RS_API void CALLSPEC rsInitFaceDetect(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsRunFaceDetect(RShandle handle, const unsigned char *image,
    rs_pixel_format pix_format,
    int width,
    int height,
    int stride,
    rs_image_rotation img_rotate,
    rs_face_info **pFaceArray,
    int *pFaceCount);
RS_API void CALLSPEC rsUnInitFaceDetect(RShandle *handle);
RS_API void CALLSPEC releaseFaceDetectResult(rs_face_info *pFaceArray, int faceCount);

/*
 * face track
 */
RS_API void CALLSPEC rsInitFaceTrack(RShandle *handle, RShandle lm);
RS_API void CALLSPEC rsInitSingleFaceTrack(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsRunFaceTrack(RShandle handle, const unsigned char *image,
    rs_pixel_format pix_format,
    int width,
    int height,
    int stride,
    rs_image_rotation img_rotate,
    rs_face_info **pFaceArray,
    int *pFaceCount);
RS_API void CALLSPEC rsUnInitFaceTrack(RShandle *handle);
RS_API void CALLSPEC releaseFaceTrackResult(rs_face_info *pFaceArray, int faceCount);

/*
 * face quality
 */
RS_API void CALLSPEC rsInitFaceQuality(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsGetFaceQualityScore(RShandle handle, const unsigned char *image,
    rs_pixel_format pix_format,
    int width,
    int height,
    int stride,
    rs_face_info *pFace);
RS_API void CALLSPEC rsUnInitFaceQuality(RShandle *handle);

/*
 * face attr
 */
RS_API void CALLSPEC rsInitAttributeDetect(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsRunAttributeDetect(RShandle handle, const unsigned char *image,
                                        rs_pixel_format pix_format,
                                        int width,
                                        int height,
                                        int stride,
                                        rs_face_info *pFace,
                                        rs_face_attr *pAttrVal);
RS_API void CALLSPEC rsUnInitAttributeDetect(RShandle *handle);

/*
 * face recognition
 */
RS_API void CALLSPEC rsInitFaceRecognition(RShandle *handle, RShandle lm, const char* dbPath);
RS_API int CALLSPEC rsRecognitionSetConfidence(RShandle handle, int confidence);
RS_API int CALLSPEC rsRecognitionResetAlbum(RShandle handle);

RS_API int CALLSPEC rsRecognitionGetFaceFeature(RShandle handle, const unsigned char *image,
                                                rs_pixel_format pix_format,
                                                int width,
                                                int height,
                                                int stride,
                                                rs_face_info *pFace,
                                                rs_face_feature *pFeature);

RS_API int CALLSPEC rsRecognitionPersonCreate(RShandle handle, rs_face_feature *pFeature);

RS_API int CALLSPEC rsRecognitionPersonAddFace(RShandle handle, rs_face_feature *pFeature, int personId);

RS_API int CALLSPEC rsRecognitionPersonDelete(RShandle handle, int personId);

// find the similairy of two faces
RS_API float CALLSPEC rsRecognitionFaceVerification(RShandle handle, rs_face_feature *pFeature1, rs_face_feature *pFeature2);

// identify person from database
RS_API int CALLSPEC rsRecognitionFaceIdentification(RShandle handle, rs_face_feature *pFeature);

// verification person from database
RS_API float CALLSPEC rsRecognitionFaceVerificationByPersonId(RShandle handle, int personId, rs_face_feature *pFeature);

// get identify confidence
RS_API float CALLSPEC rsRecognitionGetConfidence(RShandle handle);

// get how many people stored
RS_API int CALLSPEC rsRecognitionGetAlbumSize(RShandle handle);

RS_API int CALLSPEC rsRecognitionGetFaceFeaturesByPersonId(RShandle handle, int personId, rs_face_feature **pFeatureArray, int *featureCount);
RS_API int CALLSPEC rsRecognitionReleaseFaceFeatureResult(RShandle handle, rs_face_feature *pFeatureArray, int featureCount);
RS_API void CALLSPEC rsUnInitFaceRecognition(RShandle *handle);

/*
 * live check
 */
RS_API void CALLSPEC rsInitLivenessDetect(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsRunLivenessDetect(RShandle handle, const unsigned char *image,
                                        rs_pixel_format pix_format,
                                        int width, int height, int stride,
                                        rs_face_info *pFace, int *livenessState);
RS_API void CALLSPEC rsResetLivenessDetect(RShandle handle);
RS_API void CALLSPEC rsUnInitLivenessDetect(RShandle *handle);

/*
 * depth live check
 */
RS_API void CALLSPEC rsInitDepthLivenessDetect(RShandle *handle, RShandle lm);
RS_API int CALLSPEC rsRunDepthLivenessDetect(RShandle handle, const unsigned char *image,
                                            int width, int height, int stride,
                                            rs_face_info *pFace, int *livenessState);
RS_API void CALLSPEC rsUnInitDepthLivenessDetect(RShandle *handle);


/*
 * face crop
 */
RS_API void CALLSPEC rsInitFaceCrop(RShandle *handle, float scale);
RS_API int CALLSPEC rsRunFaceCropWithFaceTrack(RShandle handle, const unsigned char *image_data,
                                            int width, int height, int stride, rs_pixel_format pix_format,
                                            rs_face_info *face);
RS_API int CALLSPEC rsGetCroppedFaceByTrackId(RShandle handle, int trackId, rs_frame* image);
RS_API void CALLSPEC rsUnInitFaceCrop(RShandle *handle);

END_EXTERN_C

#endif // RS_FACE_SDK_V1_H_
