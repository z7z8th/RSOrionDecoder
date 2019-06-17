#ifndef RS_ORION_PORTAL_H_
#define RS_ORION_PORTAL_H_

#include "RSComDef.h"

#ifdef _WIN32
    #define DLL_API  __declspec(dllexport)
#else
    #define DLL_API __attribute__((visibility("default")))
#endif

class DLL_API RSOrionPortal
{
public:
	RSOrionPortal();
	~RSOrionPortal();

public:

	/*---------------------------------------------------------------------
	@func set domain and port
	@param domain: hive domain
	@param port: hive port
	@return json string
	-----------------------------------------------------------------------*/
	int RSSetSrvaddr(std::string domain, int port);

	/*---------------------------------------------------------------------
	@func login hive platform
	@param appKey: app key
	@param appSecret: app secret
	@return json string
	-----------------------------------------------------------------------*/
	std::string	RSLogin(std::string appKey, std::string appSecret);

	/*---------------------------------------------------------------------
	@func logout hive platform 
	-----------------------------------------------------------------------*/
	void RSLogout();
	
	/*---------------------------------------------------------------------
	@func Get current user details
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetCurrentUserDetails();

	/*---------------------------------------------------------------------
	@func update notification address of  gate state
	@param gateAuthUrl: url
	@param gateAuthToken: token
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSUpdateNotificationAddressOfGateState(std::string gateAuthUrl, std::string gateAuthToken);

	/*---------------------------------------------------------------------
	@func create group
	@param groupName: group name  
	@param groupDescription: group description
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSCreateGroup(std::string groupName, std::string groupDescription);

	/*---------------------------------------------------------------------
	@func update group
	@param groupuuid: group uuid
	@param groupName: group name
	@param groupDescription: group description
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSUpdateGroup(std::string groupuuid, std::string groupName, std::string groupDescription);

	/*---------------------------------------------------------------------
	@func delete group
	@param groupuuid: group uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeleteGroup(std::string groupuuid);

	/*---------------------------------------------------------------------
	@func group add person
	@param groupuuid: group uuid
	@param personuuidArray[]: person uuid array
	@param personuuidCount: number of array elements  
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGroupAddPerson(std::string groupuuid, std::string personuuidArray[], int personuuidCount);

	/*---------------------------------------------------------------------
	@func group remove person
	@param groupuuid: group uuid
	@param personuuidArray[]: person uuid array
	@param personuuidCount: number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGroupRemovePerson(std::string groupuuid, std::string personuuidArray[], int personuuidCount);

	/*---------------------------------------------------------------------
	@func group clear person
	@param groupuuid: group uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGroupClearPerson(std::string groupuuid);

	/*---------------------------------------------------------------------
	@func get group details
	@param groupuuid: group uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetGroupDetails(std::string groupuuid);

	/*---------------------------------------------------------------------
	@func get group list
	@param GroupListReq: Request information for group list 
	@param pageReq: Paging request information 
	@param pageResp:Paging reply message 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetGroupList(GroupListReq& listReq, PageReq& pageReq, PageResp& pageResp);


	/*---------------------------------------------------------------------
	@func get person of group
	@param groupuuid: group uuid
	@param personName: perosn name for query
	@param personuuid: personuuid  for query
	@param pageReq: Paging request information
	@param pageResp:Paging reply message
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetPersonOfGroup(std::string groupuuid, std::string personName, std::string personuuid, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func create person
	@param baseInfo: Basic information of person 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSCreatePerson(PersonBaseInfo& baseInfo);

	/*---------------------------------------------------------------------
	@func update person
	@param baseInfo: Basic information of person
	@param personuuid: person uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSUpdatePerson(PersonBaseInfo& baseInfo, std::string personuuid);

	/*---------------------------------------------------------------------
	@func delete person
	@param personuuid: person uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeletePerson(std::string personuuid);

	/*---------------------------------------------------------------------
	@func get person details
	@param personuuid: person uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetPersonDetails(std::string personuuid);

	/*---------------------------------------------------------------------
	@func get person list
	@param listReq: Request information for person list 
	@param pageReq: Paging request information 
	@param pageResp:Paging reply message 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetPersonList(PersonListReq & listReq, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func add image to person 
	@param image: image information
	@param personuuid: person uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSPersonAddImage(XImage& image, std::string personuuid);

	/*---------------------------------------------------------------------
	@func delete image of person
	@param personuuid: person uuid
	@param imageuuid: image uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSPersonRemoveImage(std::string personuuid, std::string imageuuid);

	/*---------------------------------------------------------------------
	@func clear images of person
	@param personuuid: person uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSPersonClearImage(std::string personuuid);


	/*---------------------------------------------------------------------
	@func register device
	@param registerReq: device registration information 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSRegisterDevice(DeviceRegisterReq& registerReq);
	
	/*---------------------------------------------------------------------
	@func unregister device
	@param deviceuuid: device uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSUnregisterDevice(std::string deviceuuid);

	/*---------------------------------------------------------------------
	@func update device
	@param macAddress: device mac address
	@param deviceName: device name
	@param description: description of device
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSUpdateDevice(std::string deviceuuid, std::string deviceName, std::string description);

	/*---------------------------------------------------------------------
	@func get device details by mac
	@param macAddress: device mac address
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetDeviceDetailsByMac(std::string macAddress);

	/*---------------------------------------------------------------------
	@func get device list
	@param deviceListReq: Request information for device list 
	@param pageReq: Paging request information 
	@param pageResp:Paging reply message 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetDeviceList(DeviceListReq& deviceListReq, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func get device identity log
	@param deviceListReq: Request information for identity log
	@param pageReq: Paging request information
	@param pageResp:Paging reply message
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetDeviceIdentityLog(DeviceIdentifyLogReq& identityLogReq, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func get device status log
	@param deviceListReq: Request information for status log
	@param pageReq: Paging request information
	@param pageResp:Paging reply message
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetDeviceStatusLog(DeviceStatusLogReq& statusLogReq, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func switch device mode
	@param authMode: authorization mode
	@param recogMode: recognition mode
	@param groupuuidArray: group uuid array
	@param arrayCount: number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceSwitchingMode(std::string deviceuuid, std::string authMode, std::string recogMode, std::string groupuuidArray[], int arrayCount);


	/*---------------------------------------------------------------------
	@func  device capture image
	@param macAddress: mac address
	@param image: image info
	-----------------------------------------------------------------------*/
	std::string RSDeviceCaptureImage(std::string macAddress, XImage& image);


	/*---------------------------------------------------------------------
	@func reset group of the device
	@param deviceuuid: device uuid
	@param groupuuiddArray: group uuid array
	@param groupuuidCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceResetGroup(std::string deviceuuid, std::string groupuuiddArray[], int groupuuidCount);

	/*---------------------------------------------------------------------
	@func add group to the device
	@param deviceuuid: device uuid
	@param groupuuiddArray: group uuid array
	@param groupuuidCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceAddGroup(std::string deviceuuid, std::string groupuuiddArray[], int groupuuidCount);

	/*---------------------------------------------------------------------
	@func remove group of the device
	@param deviceuuid: device uuid
	@param groupuuiddArray: group uuid array
	@param groupuuidCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceRemoveGroup(std::string deviceuuid, std::string groupuuiddArray[], int groupuuidCount);

	/*---------------------------------------------------------------------
	@func add person to the device
	@param deviceuuid: device uuid
	@param personuuidArray: person uuid array
	@param personuuidCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceAddPerson(std::string deviceuuid, std::string personuuidArray[], int personuuidCount);

	/*---------------------------------------------------------------------
	@func remove person of the device
	@param deviceuuid: device uuid
	@param personuuidArray: person uuid array
	@param personuuidCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceRemovePerson(std::string deviceuuid, std::string personuuidArray[], int personuuidCount);

	/*---------------------------------------------------------------------
	@func clear all person and groups of the device
	@param deviceuuid: device uuid
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSDeviceClear(std::string deviceuuid);

	/*---------------------------------------------------------------------
	@func get person list of device
	@param personName: person name
	@param personuuid: perosn uuid
	@param groupuuidArray: group uuid array
	@param arrayCount: number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetDevicePersonList(std::string deviceuuid, std::string personName, std::string personuuid, PageReq& pageReq, PageResp& pageResp);

	/*---------------------------------------------------------------------
	@func batch device reset person
	@param personuuid: person uuid
	@param deviceArray: device uuid array
	@param arrayCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSBatchDeviceResetPerson(std::string personuuid, std::string deviceArray[], int arrayCount);

	/*---------------------------------------------------------------------
	@func batch device add person
	@param personuuid: person uuid
	@param deviceArray: device uuid array
	@param arrayCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSBatchDeviceAddPerson(std::string personuuid, std::string deviceArray[], int arrayCount);

	/*---------------------------------------------------------------------
	@func batch device delete person
	@param personuuid: person uuid
	@param deviceArray: device uuid array
	@param arrayCount:number of array elements
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSBatchDeviceDeletePerson(std::string personuuid, std::string deviceArray[], int arrayCount);

	/*---------------------------------------------------------------------
	@func Face Detection
	@param detectPicReq: Face detection request information
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSFaceDetect(DetectPicReq & detectPicReq);

	/*---------------------------------------------------------------------
	@func Verificate the similarity of two faces
	@param verificateFaceuuid: the face uuid to be verified
	@param faceuuid: face uuid
	@param personuuid: person uuid 
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSFaceVerificate(std::string verificateFaceuuid, std::string faceuuid = std::string(""), std::string personuuid = std::string(""));
	
	/*---------------------------------------------------------------------
	@func Face Recognition
	@param faceuuid: face uuid
	@param groupuuid: group uuid
	@param thresholdValue: threshold Value
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSFaceRecognition(std::string faceuuid, std::string groupuuid, double thresholdValue);

	/*---------------------------------------------------------------------
	@func Face Recognition
	@param image: image information
	@param groupuuid: group uuid
	@param thresholdValue: threshold Value
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSFaceRecognition(XImage& image, std::string groupuuid, double thresholdValue);

	/*---------------------------------------------------------------------
	@func get online identity log
	@param identifyLogReq: Request information for identity log
	@param pageReq: Paging request information
	@param pageResp:Paging reply message
	@return json string
	-----------------------------------------------------------------------*/
	std::string RSGetOnlineIdentifyLog(OnlineIdentifyLogReq identifyLogReq, PageReq& pageReq, PageResp& pageResp);
private:
	void * m_pOrionClient;
};

#endif //RS_ORION_PORTAL_H_