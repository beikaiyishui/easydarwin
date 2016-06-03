/*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºCapPicture.cpp
* Discription£º
* Version    £º1.0
* Author     £ºpanyd
* Create Date£º2010_3_25
* Modification History£º
*/

#include "public.h"
#include "CapPicture.h"
#include <stdio.h>

#include <time.h>
#include <string>

/*******************************************************************
      Function:   Demo_Capture
   Description:   Capture picture.
     Parameter:   (IN)   none 
        Return:   0--success£¬-1--fail.   
**********************************************************************/
int Demo_Capture()
{
    NET_DVR_Init();
    long lUserID;
    //login
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    lUserID = NET_DVR_Login_V30("192.168.1.2", 8000, "admin", "12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //
    NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 2;
    strPicPara.wPicSize = 0;
    int iRet;
	int iRet1;

	/*
    iRet = NET_DVR_CaptureJPEGPicture(lUserID, struDeviceInfo.byStartChan, &strPicPara, "pic/ssss.jpeg");
    if (!iRet)
    {
        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
	*/
	while(1)
	{
		time_t cur_t;  
	    struct tm* cur_tm;
		time(&cur_t);  
	    cur_tm=localtime(&cur_t);
		
		char pic_path[100];
		sprintf(pic_path,"pic/%d%d%d%d%d%d.jpeg",cur_tm->tm_year,cur_tm->tm_mon,cur_tm->tm_mday,cur_tm->tm_hour,cur_tm->tm_min,cur_tm->tm_sec);
		printf("pic_path : %s \n",pic_path);

		iRet1 = NET_DVR_CaptureJPEGPicture(lUserID, struDeviceInfo.byStartChan, &strPicPara, pic_path);
	}
    //logout
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();

    return HPR_OK;

}
