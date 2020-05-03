#ifndef __RS_ORION_DECODER_H__
#define __RS_ORION_DECODER_H__

#ifdef _WIN32
#define WIN32_LEAN_ADN_MEAN
#include <WinSock2.h>
#include <Windows.h>

#ifdef USE_STATIC_LIB
#define RS_API
#else
#define RS_API  __declspec(dllexport)
#endif // USE_STATIC_LIB

//#define CALLSPEC    __stdcall
#define CALLSPEC
#else
#define RS_API __attribute__((visibility("default")))
#define CALLSPEC
#endif // _WIN32

#include <string>
#include <list>

#include "rs_filter_policy.h"

typedef void* RSHandle;

//#ifdef __cplusplus
//extern "C" {
//#endif

//typedef struct RegionRatio {
//	float xRatio = 0.0;
//	float yRatio = 0.0;
//	float widthRatio = 1.0;
//	float heightRatio = 1.0;
//};
//
//typedef struct FilterPolicy {
//	RegionRatio _regionRatio;
//	int	_faceWidthThreshold = 64;
//	int	_faceHeightThreshold = 64;
//	int	_faceQualityThreshold = 8;
//	int	_faceAngleThreshold = 90;
//	int	_onlySnapMaxFace = 0;
//	int	_snapIntervalFrame = 2;
//	int	_maxPituresPerTrack = 3;
//};

RS_API int CALLSPEC rsOrionDecoderInit(RSHandle* pRSHandle);
RS_API int CALLSPEC rsOrionDecoderUninit(RSHandle* pRSHandle);
RS_API int CALLSPEC rsOrionDecoderStart(RSHandle rsHandle);
RS_API void CALLSPEC rsOrionDecoderStop(RSHandle rsHandle);

RS_API int CALLSPEC AddVideoSource(RSHandle rsHandle,
								   std::string videoSource, 
								   bool bNetwork = true);
RS_API int CALLSPEC DelVideoSource(RSHandle rsHandle, std::string videoSource);
RS_API std::list<std::string> CALLSPEC GetVideoSourceList(RSHandle rsHandle);

RS_API void	CALLSPEC SetDeviceInfo(RSHandle rsHandle);
RS_API void	CALLSPEC SetDecodeMode(RSHandle rsHandle, int decodeMode);
RS_API int	CALLSPEC SetFaceFilterPolicy(RSHandle rsHandle, 
										readsense::FilterPolicy &fileter_policy);
RS_API void	CALLSPEC SetServerAddress(RSHandle rsHandle,
									  std::string address, 
								      std::string appKey,
									  std::string appSecret);
RS_API void	CALLSPEC SetRtmpServer(RSHandle rsHandle,
									const std::string &rtmp_server,
									const std::string &path,
									int index);


//#ifdef __cplusplus
//}
//#endif

#endif	// __RS_ORION_DECODER_H__