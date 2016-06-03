#include "talkingthread.h"
#include <signal.h>
#include <sys/time.h>
#include "Main.h"
#include "CTimer.h"
#include <vector>
#include "cameralist.h"

#define HELLO_WORLD_SERVER_PORT    60001
#define BUFFER_SIZE 1024
#define SERVERNAME "192.168.1.109"

char HelloMessage[100] = "It is a keepalive Message !";
int client_socket;
int keeplivetime;
void *SentAliveMessage(void *arg)
{
	while(1)
	{
		if(keeplivetime == 0){
			continue;
		}
		else{
			send(client_socket,HelloMessage,sizeof(HelloMessage),0);
			sleep(keeplivetime);
		}
	}
}
session::session()
{
	keeplivetime = 0;
	//RegTimeCallback(SentAliveMessage,3);
	creat_thread(SentAliveMessage,(void*)"timedelay");
	while(1)
	{
		socket_fun();
		sleep(10);
	}
}
session::~session()
{
}
int session::socket_fun()
{
	printf("start cread sock_fun!\n");
	char m_buffer[BUFFER_SIZE] = {0};
	//设置一个socket地址结构client_addr,代表客户机internet地址, 端口
    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr)); //把一段内存区的内容全部设置为0
    client_addr.sin_family = AF_INET;    //internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口
    //创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        return false;
    }
    //把客户机的socket和客户机的socket地址结构联系起来
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n"); 
        return false;
    }
    //设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(SERVERNAME,&server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        printf("Server IP Address Error!\n");
        return false;
    }
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To server!\n");
        return false;
    }
	keeplivetime = 10;
 	send(client_socket,HelloMessage,sizeof(HelloMessage),0);
    bzero(m_buffer,BUFFER_SIZE);
	//循环去查看服务器发送的数据
	fd_set SockRead;  
	int ret = 0;
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;  
    TimeOut.tv_usec = 2000000; //200毫秒  
	while(1)
	{
		FD_ZERO(&SockRead);  
        FD_SET(client_socket,&SockRead);  
        ret = select(client_socket+1,&SockRead,NULL,NULL,&TimeOut); 
        if(ret == -1){  
            printf("ERROR in select\n");
            break;
        }else if(ret == 0){
			continue;
        }
		int length = 0;
	    length = recv(client_socket,m_buffer,BUFFER_SIZE,0);

	    printf("recv : length %d \n",length);
	    if(length < 0)
	    {
	        printf("Recieve Data From Server Failed!\n");
	        break;
	    }
		/*selet的read为1，recv为0时候判断为断开连接*/
		else if(length ==0 && ret ==1)
		{
			break;
		}
		else
		{
			printf("recv buffer : %s \n",m_buffer);
			RevInfomation(m_buffer);
		}
	    bzero(m_buffer,BUFFER_SIZE);
	}
	printf("close socket with server !!! \n");
    close(client_socket);
	return 0;
}
int session::RevInfomation(char *buffer)
{
	Json::Value rev;
	Json::Reader reader;
	std::string buf = buffer;
	if(reader.parse(buf,rev))
	{	
		printf("parse json success !! \n");
		printf("rev: %s \n",rev.toStyledString().c_str());
		if(!rev["stream"].empty())
		{
			printf("stream is not empty!\n");
			//dst的json中的名字必须以*.sdp结束，否则easydawin会出错
			std::string src = rev["stream"][1u].asString();
			std::string dst = rev["stream"][0u].asString();
			fork_stream(src,dst);
		}
		else if(!rev["stop"].empty())
		{
			char *stop_string = (char*)malloc(strlen(rev["stop"][0u].asCString()));
			//strcpy(stop_string,rev["stop"][0u].asCString());
			std::string clieturl = rev["stop"][0u].asString();
			
			stoppushstream_fromclienturl(clieturl);
		}
		else if(!rev["getdevice_list"].empty())
		{
			printf("rev camera list ! \n");
			startsearch();
			std::string list = getcameralist();
			printf("getcameralist:%s \n",list.c_str());
			send(client_socket,list.c_str(),list.length(),0);

			stopsearch();
		}
		else if(!rev["start_tcp_stream"].empty())
		{
			
		}
		else if(!rev["test"].empty())
		{
			std::string src1 = "rtsp://admin:admin12345@192.168.1.3:554/h264/ch1/main/av_stream";
			std::string src2 = "rtsp://admin:admin12345@192.168.1.3:554/h264/ch1/sub/av_stream";
			std::string src3 = "rtsp://admin:admin12345@192.168.1.64:554/h264/ch1/main/av_stream";
			std::string src4 = "rtsp://admin:admin12345@192.168.1.64:554/h264/ch1/sub/av_stream";
			std::string dst1 = "li0.sdp";
			std::string dst2 = "li1.sdp";
			std::string dst3 = "li2.sdp";
			std::string dst4 = "li3.sdp";
			fork_stream(src1,dst1);
			fork_stream(src2,dst2);
			fork_stream(src3,dst3);
			fork_stream(src4,dst4);
		}
		else
		{
			//fork_stream(src1,src1name); //创建子线程用于推送指定流
			return 0;
		}
	}
	else
	{
		printf("parse json error ! \n");
		return -1;
	}
}






