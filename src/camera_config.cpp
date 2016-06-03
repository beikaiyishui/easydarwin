#ifndef __cameraconfig__
#define __cameraconfig__

#include "../include/HCNetSDK.h"
#include "camera_config.h"

/*统一配置摄像头的编码相关参数*/
bool init_camere_config(char* ip)
{
	printf("ipaddress : %s \n",ip);
	NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};

	NET_DVR_COMPRESSIONCFG_V30 m_struCompressionCfg;
	
	LONG lUserID = NET_DVR_Login_V30(ip, 8000, "admin", "admin12345", &struDeviceInfo);
	if(lUserID < 0)
	{
		printf("error userid \n");
	}

	DWORD dwReturned = 0;
	DWORD m_return = 0;
	
	NET_DVR_PICCFG_V40 m_pic;

	if(NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30,1,&m_struCompressionCfg, sizeof(m_struCompressionCfg), &dwReturned))
	{
		printf("test:%d ,%d\n",m_struCompressionCfg.struNormHighRecordPara.dwVideoFrameRate ,m_struCompressionCfg.struNormHighRecordPara.dwVideoBitrate);
	}
	
	NET_DVR_COMPRESSIONCFG_V30 m_struCompressionCfg1 = m_struCompressionCfg;
	m_struCompressionCfg1.struNormHighRecordPara.dwVideoFrameRate = 10;
	NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_COMPRESSCFG_V30,1,&m_struCompressionCfg1, sizeof(m_struCompressionCfg1));
	/*设置通道名默认不显示*/
	NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_PICCFG_V40,1,&m_pic, sizeof(m_pic), &m_return);
	m_pic.dwShowChanName = 0;
	m_pic.byDispWeek = 0;
	NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_PICCFG_V40,1,&m_pic, sizeof(m_pic));

	return true;
	
}

#endif
