#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"
#include <sys/prctl.h>
#include <signal.h>
#include "Main.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "pushstream.h"


char* ConfigIP		= "127.0.0.1";		//Default EasyDarwin Address
char* ConfigPort	= "60000";				//Default EasyDarwin Port
char* ConfigName	= "li.sdp";//Default Stream Name
char* ConfigRTSPURL	= "rtsp://admin:12345@192.168.1.2:554/h264/ch1/main/av_stream";	//RTSP Source URL(With username:password@)
char* ProgName;		//Program Name

Easy_Pusher_Handle	fPusherHandle = 0;		//libEasyPusher Handle
Easy_RTSP_Handle	fRTSPHandle = 0;		//libEasyRTSPClient Handle

/* EasyPusher Callback */
int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
    if (_state == EASY_PUSH_STATE_CONNECTING)               printf("Connecting...\n");
    else if (_state == EASY_PUSH_STATE_CONNECTED)           printf("Connected\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)      printf("Connect failed\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)       printf("Connect abort\n");
	else if (_state == EASY_PUSH_STATE_PUSHING)             printf("\r Pushing to rtsp://%s:%d/%s ...", ConfigIP, atoi(ConfigPort), ConfigName);
    else if (_state == EASY_PUSH_STATE_DISCONNECTED)        printf("Disconnect.\n");

    return 0;
}

/* EasyRTSPClient Callback */
int Easy_APICALL __RTSPSourceCallBack( int _chid, int *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo)
{
	if (_mediatype == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		if(fPusherHandle == 0 ) return 0;

		if(frameinfo && frameinfo->length)
		{
			EASY_AV_Frame  avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char*)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}	
	}

	if (_mediatype == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		if(fPusherHandle == 0 ) return 0;

		if(frameinfo && frameinfo->length)
		{
			EASY_AV_Frame  avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char*)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}	
	}

	if (_mediatype == EASY_SDK_MEDIA_INFO_FLAG)
	{
		if((pbuf != NULL) && (fPusherHandle == NULL))
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, pbuf, sizeof(EASY_MEDIA_INFO_T));

			fPusherHandle = EasyPusher_Create();
			EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
			EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 0, false);//1M缓冲区
			printf("*** live streaming url:rtsp://%s:%d/%s ***\n", ConfigIP, atoi(ConfigPort), ConfigName);
		}
	}
	return 0;
}
void handle_signal(int signo)
{
	if (signo == SIGHUP)//如果接收到父进程异常退出信号，子进程退出
    {                 
        printf("child recv SIGHUP..\n");
		exit(0);
    }
}
int rtsppush(char *src_url, char *dst_url,int pipe_fd)
{	
	ConfigName = dst_url;
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) 
		return 0;
	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;	//Get Video & Audio
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, src_url, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);

	//处理在父进程异常后的信号函数
	signal(SIGHUP, handle_signal);
    prctl(PR_SET_PDEATHSIG, SIGHUP);
	//子进程通过读管道数据判断是否要停止推送数据
	char buf[50];
	char *m_string = "stop this push stream!";
    while(1)
    {
        read(pipe_fd,buf,50);
        if(strcmp(buf,m_string) == 0)
        {
            break;
        }
    }
	if(fPusherHandle)
	{
		EasyPusher_StopStream(fPusherHandle);
		EasyPusher_Release(fPusherHandle);
		fPusherHandle = NULL;
	}
   
	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    exit(0);
}