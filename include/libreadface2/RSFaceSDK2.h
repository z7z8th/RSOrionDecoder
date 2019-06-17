#ifndef RS_FACE_SDK_H_
#define RS_FACE_SDK_H_
////////////////////////////
// ReadSense Face SDK
// 2016
////////////////////////////


#include "RSCommon.h"

#ifdef _WIN32
  #define RS_SDK_API __declspec(dllexport)
  // #define STDCALL __stdcall 
	#define STDCALL 
#else
  #define RS_SDK_API 
  #define STDCALL 
#endif

#define RS_OK      (0)
#define RS_HANDLE_CREATE_ERROR (-21)
#define RS_HANDLE_FETCH_ERROR (-22)
#define RS_LICENSE_ERROR (-23)
#define RS_TRIAL_VERSION_LIMIT_ERROR (-24)

#define ERR_FACE_TOO_SMALL (-31)
#define ERR_FACE_NOT_FRONTAL_ENOUGH (-32)

#define DEEPFACE_FEATURE_LENGTH 512

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct rs_face {
		rs_rect rect;
		rs_point landmarks21[21];
		float yaw;
		float pitch;
		float roll;
		int trackId;
    float confidence;
		float score;
	}rs_face;

	typedef struct rs_face_feature {
		int version;
		float feature[DEEPFACE_FEATURE_LENGTH];
	}rs_face_feature;

  typedef struct rs_face_attr
  {
    int  glasses; // 0: without glasses, 1: with glasses
    int  mouth_open; // 0: mouth close, 1: mouth open
    int  gender; // 0: female, 1: male, -1: not sure
    int  age;
    int  beauty;
    int  eye_close; // 0: eye close, 1: eye opened
    int  smile;
  }rs_face_attr;

	////////////////////////////
	// License Manager
	////////////////////////////
	RS_SDK_API int STDCALL rsInitLicenseManagerV2(RSHandle *handle, const char *licenseContent);
        RS_SDK_API int STDCALL rsRunLicenseManagerV2AuthRequest(RSHandle handle, char **authContent);
        RS_SDK_API int STDCALL rsActivateLicenseManagerV2(RSHandle handle, const char *authContent);
	RS_SDK_API const char *rsGetSDKLicenseDeviceKey(); // must be called after init
	RS_SDK_API void STDCALL rsUnInitLicenseManagerV2(RSHandle *handle);

	////////////////////////////
	// Face Detect
	////////////////////////////

	RS_SDK_API void STDCALL rsInitFaceDetect(RSHandle *handle, RSHandle lm);
	RS_SDK_API int STDCALL rsRunFaceDetect(RSHandle handle, const unsigned char *image,
		rs_pixel_format pix_format,
		int width,
		int height,
		int stride,
		rs_image_rotation img_rotate,
		rs_face **pFaceArray,
		int *pFaceCount);

	RS_SDK_API int STDCALL rsRunFaceAlign(RSHandle handle, const unsigned char *image,
		rs_pixel_format pix_format,
		int width,
		int height,
		int stride,
		rs_image_rotation img_rotate,
		rs_rect rect,
		rs_face *pFace);
	RS_SDK_API void STDCALL rsUnInitFaceDetect(RSHandle *handle);
	RS_SDK_API void STDCALL releaseFaceDetectResult(rs_face *pFaceArray, int faceCount);

	////////////////////////////
	// Face Track
	////////////////////////////

	RS_SDK_API void STDCALL rsInitFaceTrack(RSHandle *handle, RSHandle lm);
	RS_SDK_API void STDCALL rsInitSingleFaceTrack(RSHandle *handle, RSHandle lm);
	RS_SDK_API int STDCALL rsRunFaceTrack(RSHandle handle, const unsigned char *image,
		rs_pixel_format pix_format,
		int width,
		int height,
		int stride,
		rs_image_rotation img_rotate,
		rs_face **pFaceArray,
		int *pFaceCount);
	RS_SDK_API void STDCALL rsUnInitFaceTrack(RSHandle *handle);
	RS_SDK_API void STDCALL releaseFaceTrackResult(rs_face *pFaceArray, int faceCount);

    ////////////////////////////
    // Face Quality
    ////////////////////////////
    RS_SDK_API void STDCALL rsInitFaceQuality(RSHandle *handle, RSHandle lm);
    RS_SDK_API int STDCALL rsGetFaceQualityScore(RSHandle handle, const unsigned char *image,
        rs_pixel_format pix_format,
        int width,
        int height,
        int stride,
        rs_face *pFace);
    RS_SDK_API void STDCALL rsUnInitFaceQuality(RSHandle *handle);
    
    // ////////////////////////////
    // // Emotion Detect
    // ////////////////////////////
    // Deprecated, use attribute detect for smile
    // const char *EMOTION_LABELS[] = { "joy", "sad", "surprise", "anger", "normal" };
    // RS_SDK_API void rsInitEmotionDetect(RSHandle *handle, RSHandle lm);
    // RS_SDK_API int rsRunEmotionDetect(RSHandle handle, const unsigned char *image,
    //     rs_pixel_format pix_format,
    //     int width,
    //     int height,
    //     int stride,
    //     rs_face *pFace,
    //     float *pEmotionProbs,
    //     int *pEmotionId
    //     );
    // RS_SDK_API void rsUnInitEmotionDetect(RSHandle *handle);

    ////////////////////////////
    // Face Attribute
    ////////////////////////////
  RS_SDK_API void STDCALL rsInitAttributeDetect(RSHandle *handle, RSHandle lm);
  RS_SDK_API int STDCALL rsRunAttributeDetect(RSHandle handle, const unsigned char *image,
      rs_pixel_format pix_format,
      int width,
      int height,
      int stride,
      rs_face *pFace,
      rs_face_attr *pAttrVal
      );
  RS_SDK_API void STDCALL rsUnInitAttributeDetect(RSHandle *handle);

    ////////////////////////////
    // Face Recognition
    ////////////////////////////
    RS_SDK_API void STDCALL rsInitFaceRecognition(RSHandle *handle, RSHandle lm, const char* dbPath);

    RS_SDK_API int STDCALL rsRecognitionSetConfidence(RSHandle handle, int confidence);

    RS_SDK_API int STDCALL rsRecognitionResetAlbum(RSHandle handle);

    RS_SDK_API int STDCALL rsRecognitionGetFaceFeature(RSHandle handle, const unsigned char *image,
        rs_pixel_format pix_format,
        int width,
        int height,
        int stride,
        rs_face *pFace,
        rs_face_feature *pFeature
        );

    RS_SDK_API int rsRecognitionGetFaceCropFeature(RSHandle handle, const unsigned char *image,
        rs_pixel_format pix_format,
        int width,
        int height,
        int stride,
        rs_face_feature *pFeature
        );

    // Person API
    RS_SDK_API int STDCALL rsRecognitionPersonCreate(RSHandle handle, rs_face_feature *pFeature);

    RS_SDK_API int STDCALL rsRecognitionPersonAddFace(RSHandle handle, rs_face_feature *pFeature, int personId);

    RS_SDK_API int STDCALL rsRecognitionPersonDelete(RSHandle handle, int personId);

    //int  rsRecognitionPersonRemoveFace(RSHandle handle,int faceIndex, int personId);

    //int  rsRecognitionPersonEmptyFace(RSHandle handle,int personId);

    // find the similairy of two faces
    RS_SDK_API float STDCALL rsRecognitionFaceVerification(RSHandle handle, rs_face_feature *pFeature1, rs_face_feature *pFeature2);

    // identify person from database
    RS_SDK_API int STDCALL rsRecognitionFaceIdentification(RSHandle handle, rs_face_feature *pFeature);

    // verification person from database
    RS_SDK_API float STDCALL rsRecognitionFaceVerificationByPersonId(RSHandle handle, int personId, rs_face_feature *pFeature);

    // get identify confidence
    RS_SDK_API float STDCALL rsRecognitionGetConfidence(RSHandle handle);

    // get how many people stored
    RS_SDK_API int STDCALL rsRecognitionGetAlbumSize(RSHandle handle);

    RS_SDK_API int STDCALL rsRecognitionGetFaceFeaturesByPersonId(RSHandle handle, int personId, rs_face_feature **pFeatureArray, int *featureCount);

    RS_SDK_API int STDCALL rsRecognitionReleaseFaceFeatureResult(RSHandle handle, rs_face_feature *pFeatureArray, int featureCount);

    RS_SDK_API void STDCALL rsUnInitFaceRecognition(RSHandle *handle);

    ////////////////////////////
    // Face Liveness Detect
    ////////////////////////////
  RS_SDK_API void STDCALL rsInitLivenessDetect(RSHandle *handle, RSHandle lm);

  RS_SDK_API int STDCALL rsRunLivenessDetect(RSHandle handle, const unsigned char *image, 
      rs_pixel_format pix_format, 
      int width, int height, int stride,
      rs_face *pFace, int *livenessState);

  RS_SDK_API void STDCALL rsResetLivenessDetect(RSHandle handle);

  RS_SDK_API void STDCALL rsUnInitLivenessDetect(RSHandle *handle);


    ////////////////////////////
    // Face Crop
    ////////////////////////////

    RS_SDK_API void STDCALL rsInitFaceCrop(RSHandle *handle, float scale);
    RS_SDK_API int STDCALL rsRunFaceCropWithFaceTrack(RSHandle handle, const unsigned char *image_data, 
                int width, int height, int stride, rs_pixel_format pix_format, rs_face *face);

    RS_SDK_API int STDCALL rsGetCroppedFaceByTrackId(RSHandle handle, int trackId, LP_rs_image image);

    RS_SDK_API void STDCALL rsUnInitFaceCrop(RSHandle *handle);


    ////////////////////////////
    // Face Depth Liveness Detect
    ////////////////////////////
      RS_SDK_API void STDCALL rsInitDepthLivenessDetect(RSHandle *handle, RSHandle lm);

      RS_SDK_API int STDCALL rsRunDepthLivenessDetect(RSHandle handle, const unsigned char *image, 
          int width, int height, int stride,
          rs_face *pFace, int *livenessState);

      RS_SDK_API void STDCALL rsUnInitDepthLivenessDetect(RSHandle *handle);




	RS_SDK_API char * STDCALL rsGetRSFaceSDKVersion();

#ifdef __cplusplus
}
#endif
#endif
