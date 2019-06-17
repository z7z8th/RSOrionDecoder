#ifndef __RS_IMAGE_UPLOAD_H__
#define __RS_IMAGE_UPLOAD_H__

#include "hw/hdef.h"
#include "hw/hlog.h"
#include "librsorionportal/RSComDef.h"
#include "librsorionportal/RSOrionPortal.h"

#pragma comment(lib, "libRSOrionPortald.lib")

#define DEFAULT_SERVER_PORT		3030

class RSImageUpload {
public:
	RSImageUpload() {}
	~RSImageUpload() { Logout(); }

public:
	int UploadImage(uint8 *image, int width,
		int height, std::string macAddress);

	void Login(std::string address,
		std::string appKey, std::string appSecret);
	void Logout();

private:
	RSOrionPortal rsOrionPortal_;
};

void RSImageUpload::Login(std::string address, std::string appKey,
	std::string appSecret)
{
	int ret = rsOrionPortal_.RSSetSrvaddr(address, DEFAULT_SERVER_PORT);
	hlogi("RSSetSrvaddr ret:%d", ret);
	std::string str_ret = rsOrionPortal_.RSLogin(appKey, appSecret);
}

void RSImageUpload::Logout()
{
	rsOrionPortal_.RSLogout();
}

int RSImageUpload::UploadImage(uint8 *image, int width,
	int height, std::string macAddress)
{
	XImage xImage;
	xImage.nWidth = width;
	xImage.nHeight = height;
	xImage.nPitch = width * 3;
	xImage.nBufLength = height * width * 3;
	xImage.nFormat = XPF_PIX_FORMAT_BGR888;
	xImage.pData = image;

	std::string str = rsOrionPortal_.RSDeviceCaptureImage(macAddress, xImage);
	//printf("###########RSDeviceCaptureImage : %s", str.c_str());
	//hlogi("RSDeviceCaptureImage : %s", str.c_str());
	return 0;
}

#endif // __RS_IMAGE_UPLOAD_H__