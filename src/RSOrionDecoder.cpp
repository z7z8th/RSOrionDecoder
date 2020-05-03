#include "RSOrionDecoder.h"
#include "rs_orion_face_defence.h"

RS_API int CALLSPEC rsOrionDecoderInit(RSHandle* pRSHandle) {
	RSOrionDefence* pObj = new RSOrionDefence;
	*pRSHandle = (RSOrionDefence*)pObj;
	return 0;
}

RS_API int CALLSPEC rsOrionDecoderUninit(RSHandle* pRSHandle) {
	if (pRSHandle == NULL) return -1;

	RSOrionDefence* pObj = (RSOrionDefence*)*pRSHandle;
	pObj->Stop();

	delete pObj;
	*pRSHandle = NULL;

	return 0;
}

RS_API int CALLSPEC rsOrionDecoderStart(RSHandle rsHandle) {
	if (rsHandle == NULL) return -1;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->Start();
}

RS_API void CALLSPEC rsOrionDecoderStop(RSHandle rsHandle) {
	if (rsHandle == NULL) return;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->Stop();
}

RS_API int CALLSPEC AddVideoSource(RSHandle rsHandle,
	std::string videoSource, bool bNetwork) {
	if (rsHandle == NULL) return -1;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->AddVideoSource(videoSource, bNetwork);
}

RS_API int CALLSPEC DelVideoSource(RSHandle rsHandle, std::string videoSource) {
	if (rsHandle == NULL) return -1;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->DelVideoSource(videoSource);
}

RS_API std::list<std::string> CALLSPEC GetVideoSourceList(RSHandle rsHandle) {
	std::list<std::string> list;
	if (rsHandle == NULL) return list;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->GetVideoSourceList();
}


RS_API void	CALLSPEC SetDeviceInfo(RSHandle rsHandle) {
	if (rsHandle == NULL) return;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	pObj->SetDeviceInfo();
}

RS_API void	CALLSPEC SetDecodeMode(RSHandle rsHandle, int decodeMode) {
	if (rsHandle == NULL) return;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	pObj->SetDecodeMode(decodeMode);
}

RS_API int	CALLSPEC SetFaceFilterPolicy(RSHandle rsHandle,
	readsense::FilterPolicy &fileter_policy) {
	if (rsHandle == NULL) return -1;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	return pObj->SetFaceFilterPolicy(fileter_policy);
}

RS_API void	CALLSPEC SetServerAddress(RSHandle rsHandle,
	std::string address,
	std::string appKey,
	std::string appSecret) {
	if (rsHandle == NULL) return;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	pObj->SetServerAddress(address, appKey, appSecret);
}

RS_API void	CALLSPEC SetRtmpServer(RSHandle rsHandle, const std::string &rtmp_server, const std::string &path, int index) {
	if (rsHandle == NULL) return;

	RSOrionDefence* pObj = (RSOrionDefence*)rsHandle;
	pObj->SetRtmpServer(rtmp_server, path, index);
}