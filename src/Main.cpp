#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "GetStream.h"
#include "public.h"
#include "ConfigParams.h"
#include "Alarm.h"
#include "CapPicture.h"
#include "playback.h"
#include "Voice.h"
#include "tool.h"
#include "epoll.h"
#include "config.h"
#include "pushstream.h"
#include "Main.h"
#include "talkingthread.h"
#include "CTimer.h"
#include "camera_config.h"
#include "cameralist.h"
#include "file.h"
#include "streamclient.h"
#include <list>

using namespace std;

#define MAXSTREAM 60

char *stopstring = "stop this push stream!";

pthread_mutex_t mutex ;

//初始化服务器线程池大小
struct threadpool *pool = threadpool_init(80, 300);
void creat_thread(void* (*callback_function)(void *arg), void *arg)
{
	threadpool_add_job(pool,callback_function, arg);
	return;
}
void *session_thread(void *arg)
{
	session *linkline = new session;
}
void *session_http(void *name)
{
	httpclientinit();
}
void *streamthread(void *arg)
{
	printf("strart get stream ! arg:%s \n",arg);
	printf("creat a stream thread :%lu \n",pthread_self());
	
	char cUserChoose = 'r';
	NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};

	LONG lUserID = NET_DVR_Login_V30((char*)arg, 8000, "admin", "admin12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd---Login error, %d\n", NET_DVR_GetLastError());
        printf("Press any key to quit...\n");
        cin>>cUserChoose;
        NET_DVR_Cleanup();
    }
	Demo_GetStream_V30(lUserID);

	NET_DVR_Logout_V30(lUserID);
}
void *snapthread(void *arg)
{
	printf("strart snap picture ! \n");
	Demo_Capture();
}
void *alarmthread(void *arg)
{
	Demo_Alarm(); 
}

//map中添加管道数据的视频url和对应的pipe写句柄
//对应推送流的url与easydarwin拉流的url的对应
typedef  struct urltopipestruct
{
	std::string pushurl;
	std::string geturl;
	int fd;
} urltopipe;
std::list<urltopipe> maplist;
char* get_netip()
{
	system("wget http://members.3322.org/dyndns/getip");
	char *path = "getip";
	readfile(path);
}
char *get_uuid()
{
	char *path = "/sys/class/dmi/id/product_uuid";
	return readfile(path);
}

int fork_stream(std::string src_url,std::string dst_url)
{
	std::list<urltopipe>::iterator opt;
	for(opt = maplist.begin();opt != maplist.end();opt++)
	{
		if((*opt).geturl == dst_url)
		{
			printf("url has exist ,please change another ! \n");
			return false;
		}
	}
	int pipe_fd[2];
	if(pipe(pipe_fd) < 0)
    {
        perror("Fail to pipe");
        return false;
    }
	pid_t child;
	if((child = fork()) == -1)  
    {  
        perror("fork");  
    }  
    else if(child == 0)                 //子进程中  
    {  
        //printf("\tchild pid = %d,ppid = %d\n",getpid(),getppid());  
        int src_len = src_url.length();
        char src[src_len+1];
		memset(src,'\0',src_len+1);
		src_url.copy(src,src_len,0);
		int dst_len = dst_url.length();
        char dst[dst_len+1];
		memset(dst,'\0',dst_len+1);
		dst_url.copy(dst,dst_len,0);
		
		rtsppush(src,dst,pipe_fd[0]);
    }
	else
	{
		//写管道数据src_url后，子进程检测到后结束推送
		//写入对应的推送和拉流的url对应的表
		printf("++++++++++add map : %s,%s,%d \n",src_url.c_str(),dst_url.c_str(),pipe_fd[1]);
		urltopipe m_map = {src_url,dst_url,pipe_fd[1]};
		maplist.push_back(m_map);
	}
	return true;
}
//从拉流的sdp定位到push端，并停止推送该流
void stoppushstream_fromclienturl(std::string clienturl)
{
	std::list<urltopipe>::iterator opt;
	if(maplist.empty()==true)
	{
		printf("maplist  is null !\n");
		return;
	}
	for(opt = maplist.begin();opt != maplist.end();opt++)
	{
		printf("-----------%s,%s,%d\n",(*opt).pushurl.c_str(),(*opt).geturl.c_str(),(*opt).fd);
	}
	pthread_mutex_lock(&mutex);
	
	for(opt = maplist.begin();opt != maplist.end();)
	{
		if((*opt).geturl == clienturl)
		{
			write((*opt).fd,stopstring,50);
			opt = maplist.erase(opt);
			break;
		}
		else
			++opt;
	}
	pthread_mutex_unlock(&mutex);
}
int main()
{
	//printf("%s \n",get_uuid());
	//pthread_mutex_init(&mutex,NULL);
    NET_DVR_Init();
    Demo_SDK_Version();

	/*
    NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};
	LONG lUserID = NET_DVR_Login_V30("172.168.1.223", 8000, "test", "test123456", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd---Login error, %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    Demo_GetStream_V30(lUserID);
    */
    Demo_Alarm();
	//startsearch();
	//creat_thread(session_http, (void*)"http_server");
	//creat_thread(session_thread, (void*)"session_thread");
	
	pause();
	/*
	pthread_t snap_thread;
	pthread_t stream_thread[MAXSTREAM];
	Json::Value push_list;
	getConfig("pushstream",push_list);
	int push_stream_id;
	printf("camera_list.size() : %d \n",push_list.size());
	if(push_list.size() > MAXSTREAM)
	{
		printf("max cameras is 60 !!! \n");
		return false;
	}
	//memset(obj , 0 ,sizeof(obj));
	for(push_stream_id = 0;push_stream_id < push_list.size();push_stream_id++ )
	{	
		//obj[push_stream_id]->dst_url = push_list[push_stream_id]["dst"].asCString();
		//obj[push_stream_id]->src_url = push_list[push_stream_id]["src"].asCString();
		//threadpool_add_job(pool, push, (void*)obj[push_stream_id]);
	}
	*/
	/*
	for(int channel_stream_id = 0;channel_stream_id < camera_list.size();channel_stream_id++ )
	{
		//开启视频服务线程
		pthread_create(&stream_thread[channel_stream_id],NULL,streamthread,(void*)camera_list[channel_stream_id].asCString());
	}
	//开启抓图线程
	//threadpool_add_job(pool, snapthread, (void*)"snapthread");
	*/
    NET_DVR_Cleanup();
    return 0;
}