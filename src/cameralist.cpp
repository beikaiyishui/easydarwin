#include "cameralist.h"
#include "camera_config.h"

std::string ip_list = "";
std::vector<SADP_DEVICE_INFO> camera_ip_list;
std::vector<SADP_DEVICE_INFO>::iterator list_it;

bool haveInit = 0;
void m_pDeviceFindCallBack(const SADP_DEVICE_INFO *lpDeviceInfo, void *pUserData)
{
	camera_ip_list.push_back(*lpDeviceInfo);
	if(!haveInit)
	{
		char ip[16] = {0};
		strcpy(ip,lpDeviceInfo->szIPv4Address);
		if(init_camere_config(ip))
			haveInit = 1;
		else
			haveInit = 0;
	}
}
void startsearch()
{
	ip_list = "";
	SADP_Start_V30(m_pDeviceFindCallBack);
	SADP_SendInquiry();
}
void stopsearch()
{
	camera_ip_list.clear();
	SADP_Stop();
}
std::string getcameralist()
{	
	for(list_it=camera_ip_list.begin();list_it != camera_ip_list.end();list_it++)
	{
		char *send_test = (*list_it).szIPv4Address;
		std::string sent_test = send_test;
		ip_list = ip_list+" "+sent_test;
	}
	return ip_list;
}