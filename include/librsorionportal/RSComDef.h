#ifndef RS_COMDEF_H_
#define RS_COMDEF_H_

#include <string>

typedef enum x_pixel_format_t {
	XPF_PIX_FORMAT_GRAY,
	XPF_PIX_FORMAT_BGR888,
	XPF_PIX_FORMAT_NV21,
	XPF_PIX_FORMAT_BGRA8888,
	XPF_PIX_FORMAT_NV12,
	XPF_PIX_FORMAT_YUV_I420,
	XPF_PIX_FORMAT_YV12,
	XPF_PIX_FORMAT_YUV_422P,
	XPF_PIX_FORMAT_BIN_JPEG,
	XPF_PIX_FORMAT_BIN_PNG
}x_pixel_format;

typedef struct XImage {
	int nWidth;
	int nHeight;
	int nPitch;
	x_pixel_format nFormat;
	int nBufLength;
	unsigned char *pData;
}XImage, *LPXImage;

struct PersonBaseInfo
{
	std::string name;
	std::string phone;
	std::string email;
	int age;
	int gender;
	XImage ximage;
};

typedef struct DeviceIdentifyLogReq
{
	std::string macAddress;
	std::string personuuid;
	std::string startTime; //起始时间
	std::string endTime;   //结束时间
	std::string identifyStatus;
	std::string channel;
	std::string identifyMode;
	int identifyResult;
	
	DeviceIdentifyLogReq()
	{
		macAddress = "";
		personuuid = "";
		startTime = "";
		endTime = "";
		identifyStatus = "";
		channel = "";
		identifyMode = "";
		identifyResult = 0;
	}
};

typedef struct DeviceStatusLogReq
{
	std::string macAddress;
	std::string startTime; //起始时间
	std::string endTime;   //结束时间
	int online;
};

typedef struct PageReq
{
	int currentPage;
	int itemCount;
	std::string orderByElement;
	std::string sortByMethod;
	PageReq()
	{
		currentPage = 1;
		itemCount = 10;
		orderByElement = "created_at";
		sortByMethod = "desc";
	}
};

typedef struct PageResp
{
	int totalCount;
	int currentPage;
	int itemCount;
	PageResp()
	{
		totalCount = 0;
		currentPage = 0;
		itemCount = 0;
	}
};

typedef struct DeviceListReq
{
	std::string deviceName;
	std::string deviceuuid;
	std::string macAddress;
	std::string authMode;
	std::string recogMode;
	std::string onlineState;
	std::string deviceType;
	DeviceListReq()
	{
		deviceName = "";
		deviceuuid = "";
		macAddress = "";
		authMode = "";
		recogMode = "";
		onlineState = "";
		deviceType = "";
	}
};

typedef struct DeviceRegisterReq
{
	std::string macAddress;
	std::string deviceIp;
	std::string deviceType;
	std::string deviceName;
	std::string gateSn;
	std::string description;
	DeviceRegisterReq()
	{
		macAddress = "";
		deviceIp = "";
		deviceType = "";
		deviceName = "";
		gateSn = "";
		description = "";
	}
};

typedef struct DetectPicReq
{
	XImage image;
	bool isAttributeDet;
	bool isFeatureDet;
	bool isLiveDet;
	DetectPicReq()
	{
		isAttributeDet = 0;
		isFeatureDet = 0;
		isLiveDet = 0;
	}
};

typedef struct PersonListReq
{
	std::string personName;
	std::string personuuid;
	PersonListReq()
	{
		personName = "";
		personuuid = "";
	}
};

typedef struct GroupListReq
{
	std::string groupName;
	std::string groupuuid;
	GroupListReq()
	{
		groupName = "";
		groupuuid = "";
	}
};

typedef struct OnlineIdentifyLogReq
{
	std::string personuuid;
	std::string startTime; //起始时间
	std::string endTime;   //结束时间
	int identifyResult;

	OnlineIdentifyLogReq()
	{
		personuuid = "";
		startTime = "";
		endTime = "";
		identifyResult = 0;
	}
};
#endif //RS_COMDEF_H_