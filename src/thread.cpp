#include<stdlib.h>   
#include<stdio.h>   
#include<string.h>   
#include<errno.h>   
#include<netdb.h>   
#include<sys/types.h>   
#include<netinet/in.h>   
#include<sys/socket.h>   
#include<sys/wait.h>   
#include<unistd.h>   
#include<pthread.h>   
#include<fcntl.h>
#include <json/json.h>
  
#define __DEBUG  
#ifdef __DEBUG  
#define DBG(fmt,args...) fprintf(stdout,  fmt,  ##args)  
#else  
#define DBG(fmt,args...)  
#endif  
#define ERR(fmt,args...) fprintf(stderr,  fmt,  ##args)  
  

static int isStreamQuit    = 0;  
static int gStreamServerFd = -1;  
int gStreamClientFd;  
int clientfdlist[20] = {0};
#define PORT_SOCKET          50002   //server监听端口  
#define RECV_BUF_MAX_LEN     512  
#define TIME_WAIT            200000     /*200ms*/  


unsigned char *videobuf;
unsigned int buflen;
int stream_type;

FILE *g_pFile1 = NULL;


int SocketInit();
int ScketSend(int sock_fd);

int getvideobuf(unsigned char *buf,unsigned int len,int type)
{
/*
#define NET_DVR_SYSHEAD			1	//系统头数据
#define NET_DVR_STREAMDATA		2	//视频流数据（包括复合流和音视频分开的视频流数据）
#define NET_DVR_AUDIOSTREAMDATA	3	//音频流数据
#define NET_DVR_STD_VIDEODATA	4	//标准视频流数据
#define NET_DVR_STD_AUDIODATA	5	//标准音频流数据
#define NET_DVR_SDP             6   //SDP信息(Rstp传输时有效)
*/
	if(type == 6)
	printf("callback stream type: %s\n",buf);
	videobuf = buf;
	buflen = len;
	stream_type = type;
	for(int i=0;i<20;i++)
	{
		if(clientfdlist[i] == 0)
			continue;
		else
			ScketSend(clientfdlist[i]);
	}
	/*
	// 打印fdlist的表
	for(int i=0;i<20;i++)
	{
		printf("%d, ",clientfdlist[i]);
		if(i == 19)
		printf("\n");
	}
	*/
	
}
int ScketSend(int sock_fd)  
{  
    int ret = 0;  
    if(sock_fd <= 0)  
        return -1;  
    ret = send(sock_fd,videobuf,buflen,0);  
    if(ret <= 0 ){  
        return -1;  
    }
    return 0;  
}  
bool checkfdlist()
{
	for(int j=0;j<20;j++)
	{
		if(clientfdlist[j] == 0)
			return false;
	}
		return true;
}
int indsertfdlist(int fd)
{
	printf("insertfd -fd:%d \n",fd);
	int i;
	for(i=0;i<20;i++)
	{
		if(clientfdlist[i] == 0)
		{
			clientfdlist[i] = fd;
			break;
		}
	}
	return i;
}
void parser(char *info,char *head,char *describe)
{
	
	if(strstr(info,"PLAY") != NULL)
		{
			strcpy(head,"PLAY");
		}
	else if(strstr(info,"OPTIONS") != NULL)
		{
			strcpy(head,"OPTIONS");
		}
	else if(strstr(info,"DESCRIBE") != NULL)
		{
			strcpy(head,"DESCRIBE");
		}
	else if(strstr(info,"SETUP") != NULL)
		{
			strcpy(head,"SETUP");
		}
}
char* replay(char *info,char* head,char*describe)
{
	
}
/* 
socket连接线程，阻塞等待客户端的连接， 
连接后循环处理客户端发来的数据， 
*/  
void *SocketThread(void *arg)  
{  		
		printf("creat a thread :%lu \n",pthread_self());

		long temp_fd = long(arg);
		
		if(!checkfdlist)
		{
			printf("fdlist all is 0 \n");
			clientfdlist[19] = 0;
		}
		
		struct timeval TimeOut; 
		fd_set SockRead;  
		int ret;  
		int rcvLen;  
		unsigned char *rcvBuf = (unsigned char *)malloc(RECV_BUF_MAX_LEN); 

		Json::Reader reader;
		Json::Value value;
		std::string rec_string = "";

		int list = 0;
		list = indsertfdlist(temp_fd);
        /*客户端已连接，循环处理*/  
        while(1){
		
            FD_ZERO(&SockRead);  
            FD_SET(temp_fd,&SockRead);  
            TimeOut.tv_sec = 0;  
            TimeOut.tv_usec = TIME_WAIT;  

            /* 
            200ms超时判断是否有数据接收，有则接收，没有继续循环， 
            总是让程序不能一直阻塞在这里 */
            ret = select(temp_fd+1,&SockRead,NULL,NULL,&TimeOut);  
            if(ret == -1){  
                ERR("ERROR in select\n");  
                break;  
            }else if(ret == 0){  
                continue;  
            }
			
            /*接收客户端发来的数据*/  
            rcvLen = recv(temp_fd,rcvBuf,RECV_BUF_MAX_LEN,0);  
			printf("rcvlen :%d \n",rcvLen);
            if(rcvLen < 0){  
                ERR("ERROR in stream accept: %s\n",strerror(errno));   
                break;  
            }else if(rcvLen == 0){  
                DBG("stream client close\n");
                break;  
            }
			else
			{
				char *rtsp_replay ;
				char head[50] = {0};
				char describe[20] = {0};
				parser((char*)rcvBuf,head,describe);
				DBG("rcvLen: %d,rcvBuf:\n %s\n",rcvLen,rcvBuf);
				
			}
				/*
				if(!strcmp(head,"PLAY"))
				{
					list = indsertfdlist(temp_fd);
				}
				else if(!strcmp(head,"OPTIONS"))
					{
						rtsp_replay ="RTSP/1.0 200 OK \r\n CSeq: 2 \r\n Public: OPTIONS, DESCRIBE, PLAY, PAUSE, SETUP \r\nDate:  Wed, Dec 16 2015 14:36:30 GMT";
					}
				else if(!strcmp(head,"DESCRIBE"))
					{
					rtsp_replay = "RTSP/1.0 200 OK\r\nCSeq: 3\r\nContent-Type: application/sdp";
					}
				else if(!strcmp(head,"SETUP"))
					{
						rtsp_replay = "RTSP/1.0 200 OK\r\nCSeq: 4\r\nTransport: RTP/AVP;unicast;client_port=53084-53085;server_port=8256-8257;ssrc=73f3ed3c";

					}
				printf("rtsp_replay:\n%s\n",rtsp_replay);
				send(temp_fd,rtsp_replay,sizeof(rtsp_replay),0);
				*/

				
				
				/*
				//判断连接请求中的json结构，需要进行身份验证，如果不在信任列表，需要将对应的fd设置为0
				if (reader.parse((const char*)rcvBuf , value)){
					printf("recv socket json: %s \n",value.toStyledString().c_str());
				}
				else{
					printf("json parse error !!!\n");
				}
				*/
			}
			memset(rcvBuf,0,RECV_BUF_MAX_LEN);
        DBG("Close Stream Socket\n");
		//printf("make clientfdlist[%d] = 0 \n",i);
        close(temp_fd);
		
		clientfdlist[list] = 0;

		DBG("SocketThread exit\n");  
		free(rcvBuf);
		pthread_exit(NULL);  
    
}  
  
/* 
socket连接初始化 
*/  
int SocketInit()  
{  
    struct sockaddr_in server;  
    pthread_t tStreamSocketThr[10];  
    /*创建socket*/  
    gStreamServerFd = socket(AF_INET,SOCK_STREAM,0);   
    if(gStreamServerFd == -1 ){  
        ERR("ERROR in create Stream Socket!\n");  
        return -1;  
    }  
    memset(&server,0,sizeof(server));  
    server.sin_family = AF_INET;  
    server.sin_port = htons(PORT_SOCKET);  
    server.sin_addr.s_addr = htonl(INADDR_ANY);  
    /*绑定监听端口*/  
    if(bind(gStreamServerFd ,(struct sockaddr*)&server,sizeof(server))== -1){  
        ERR("ERROR in bind Stream Socket: %s\n",strerror(errno));  
        return -1;  
    }  
    /*开始监听*/  
    if( listen(gStreamServerFd, 10) == -1){  
        ERR("ERROR in listen Stream Socket: %s\n",strerror(errno));  
        return -1;  
    }
	
	/////////////////////////////////////////////////////////////////////////  
    struct sockaddr clientAddr;   
    socklen_t  len;  
    len = sizeof(clientAddr); 
	int thread_id = 0;

	//设置socket接受超时时间
	struct timeval tv_out;
    tv_out.tv_sec = 0;
    tv_out.tv_usec = 100;
	//setsockopt(gStreamServerFd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
    while(1){  
        //阻塞等待客户端连接
        gStreamClientFd = accept(gStreamServerFd, &clientAddr, &len);  
        if(gStreamClientFd == -1){  
            ERR("ERROR in stream accept: %s\n",strerror(errno));  
            break;  
        }
		else if(thread_id<10)
		{
			pthread_create(&tStreamSocketThr[thread_id],NULL,SocketThread,(void*)gStreamClientFd);
			pthread_detach(tStreamSocketThr[thread_id]);
			thread_id++;

		}
		else
		{}
        DBG("accep a stream client\n");
	}
	printf("exit accept loop !!!\n");
	return 0;  
}  

/*
int main()  
{  
    if(SocketInit() == -1){  
        return -1;  
    }  
	printf("stop all thread!\n");
    //isStreamQuit = -1;  
    sleep(1);  
	close(gStreamServerFd); 
    DBG("SUCCESS in ConnectManageExit\n");  
}  
*/