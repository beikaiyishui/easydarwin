#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <iostream>  
#include <string>  
#include "Main.h"
#include <openssl/aes.h>
#include <list>
#include <limits.h>

extern "C"
{
#include "../include/http/h3.h"
}
using namespace std;  

#define MYPORT 60002
#define BACKLOG 40
#define BUF_SIZE 1024
int fd_A[BACKLOG];
int conn_amount;

//list表中对应没一路socket连接的fd值和对应的拉流格式
typedef struct fdstringtime
{
	int fd;
	string url;
	unsigned int times;
} httpclientnode;
std::list<httpclientnode> httptalklist;
char* aes_decrypt(char* string,char* key,int len)
{
	printf("aes_decrypt string : %s \n",string);
	char *key_string = key;
    AES_KEY  aes;
    unsigned char out[strlen(string)];

    if (AES_set_decrypt_key((unsigned char*)key_string, len, &aes) < 0) 
	{
        fprintf(stderr, "Unable to set encryption key in AES\n");
		return NULL;
    }
    AES_decrypt((unsigned char*)string,out,&aes);
	
	return (char*)out;
}
char* aes_encrypt(char* string,char* key,int len)
{
	char *key_string = key;
    AES_KEY  aes;
	printf("strlen : %d \n",strlen(string));
    unsigned char encryptout[strlen(string)];
    if (AES_set_encrypt_key((unsigned char*)key_string, len, &aes) < 0) 
	{
        fprintf(stderr, "Unable to set encryption key in AES\n");
		return NULL;
    }
    AES_encrypt((unsigned char*)string,encryptout,&aes);
	return (char*)encryptout;
}

void showclient()
{
    int i;
    printf("client amount: %d\n", conn_amount);
    for (i = 0; i < BACKLOG; i++) {
        printf("%d  ",fd_A[i]);
    }
    printf("\n\n");
}
void insertnode(int fd,std::string url,unsigned int time)
{
	//确定fd是否已经存在
	std::list<httpclientnode>::iterator it;
	for(it=httptalklist.begin();it!=httptalklist.end();it++){
		if((fd == (*it).fd)||((*it).url==url)){
			printf("insert fd or url has in list !\n");
			return;
		}
	}
	httpclientnode node = {fd,url,time};
	httptalklist.push_back(node);
	string src = "rtsp://admin:admin12345@192.168.1.64:554/h264/ch1/main/av_stream";
	fork_stream(src,url);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>.httplistinsert: %d,%s,%u \n",fd,url.c_str(),time);
}

//校验码验证
bool check(std::string httpvalue)
{

	printf("UINT_MAX = %u \n",UINT_MAX); //UINT最大值 4294967295
	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>httpvalue length : %d \n",httpvalue.length());
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>httpvalue : %s \n",httpvalue.c_str());
	if(httpvalue.length() != 20)
	{
		return false;
	}
	if((httpvalue[7-1]-'0')>4)
		return false;
	else if((httpvalue[7-1]-'0') ==4){
		string num = httpvalue.substr(7,9);
		if(atoi(num.c_str()) > 294967295)
			return false;
	}
	
	std::string sum = httpvalue.substr(16,3); //17到19位代表校验码
	int chechsum = 0;
	for(int i=0;i<httpvalue.length()-3;i++)
	{
		char b = httpvalue[i];
		int a = b-'0';
		chechsum = chechsum + a;
	}
	printf(">>>>>>>>>>>>>>>>>>>checksum = %d ,sum = %d \n",chechsum,atoi(sum.c_str()));
	if(chechsum%128 == atoi(sum.c_str()))
		return true;
	else
		return false;
}
//获取url中的时间戳
unsigned int gettimestamp(std::string httpvalue)
{
	unsigned int time;
	httpvalue = httpvalue.substr(6,10);
	char time_char[11];
	memset(time_char,'\n',11);
	httpvalue.copy(time_char,10,0);
	time = atoi(time_char);
	return time;
}
//获取系统时间戳
unsigned int getsystime()
{
	time_t t;  
    t = time(NULL);  
    unsigned int ii = time(&t);  
	return ii;
}
std::string creaturl(std::string stamp)
{
	std::string newstamp = string("rtsp://") + string("hongxin") + stamp + string(".sdp");
	return newstamp;
}
//解析收到的html数据，提出sdp的地址
bool getacceptstring(const char *headerBody, int len,int fd)
{
	RequestHeader *header;
    header = h3_request_header_new();
    h3_request_header_parse(header, headerBody, len);

    printf("Method: %s\n",header->RequestMethod);

	if(strcmp(header->RequestMethod,"GET"))
	{
		char* req = const_cast<char*>(header->RequestMethod);
		std::string gethttp = req;
		
		//url格式192.168.1.205:60002/encrypt=1234561234567890040
		std::string::size_type position = gethttp.find("=");
		if(position == std::string::npos)
			return false;
		else
		{
			gethttp = gethttp.substr(position,gethttp.length());
			position = gethttp.find("HTTP");
			gethttp = gethttp.substr(1,position-1);
			printf("*******************: %s \n",gethttp.c_str());
		}
		if(!check(gethttp))
		{
			printf("-------------------check error----------------------------- !\n");
			return false;
		}
		unsigned int times_stamp =  gettimestamp(gethttp);//取到请求数据中的时间戳
		std::string stamp = gethttp.substr(6,10);
		printf("times_stamp : %d \n",times_stamp);
		
		
		std::string replay = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/pain; charset=utf-8\r\n"
					   "Connection:keep-alive\r\n"
					   "\r\n";

		char gethttpstr[gethttp.length()];
		gethttp.copy(gethttpstr,gethttp.length(),0);
		//char* respons = aes_decrypt(aes_encrypt("12345","12345",128),"12345",128);
		std::string rep = replay + string("rtsp://") + stamp + string(".sdp");;
		
		int req_len = rep.length();
		printf("req_len: %d \n",req_len);
		char* rep_http = (char*)malloc(sizeof(char)*req_len);
		rep.copy(rep_http,req_len,0);
		//针对http请求，返回http数据
 		send(fd,rep_http,req_len,0);
		//回复url后，将url和对应的fd保存在map中
		std::string rtspurl = stamp + ".sdp";
		insertnode(fd, rtspurl,times_stamp);
		
		free(rep_http);
		h3_request_header_free(header);
		return true;

	}
	else
	{
		printf("+++++++++not identify http data !++++++++++ \n");
	}
	return true;
}
void printmap()
{
	printf("****************\n");
	std::list<httpclientnode>::iterator it;
	for(it= httptalklist.begin();it!=httptalklist.end();it++)
	{
		printf("%d,%s,%u\n",(*it).fd,(*it).url.c_str(),(*it).times);
	}
	return;
}
bool searchnode(int fd)
{
	std::list<httpclientnode>::iterator it;
	for(it= httptalklist.begin();it!=httptalklist.end();it++)
	{
		if((*it).fd == fd)
		{
			return true;
		}
	}
	return false;
}
void senderror(int fd)
{	
	printf("send error >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	char* error = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/pain; charset=utf-8\r\n"
					   "Connection:keep-alive\r\n"
					   "\r\n"
					   "error\0";
	
	send(fd,error,strlen(error),0);
	return;
}
void delhttplistnode(int fd)
{
	std::list<httpclientnode>::iterator it;
	if(httptalklist.empty()){
		printf("httptalklist is empty ! \n");
		return;
	}
	else{
		for(it=httptalklist.begin();it!=httptalklist.end();){
			if((*it).fd == fd){
				stoppushstream_fromclienturl((*it).url);
				it = httptalklist.erase(it);
			}else
				it++;
		}
	}
}
//定时检查对应各个连接的时间戳是否超时
void *checkstamp(void *arg)
{
	std::list<httpclientnode>::iterator it;
	while(1)
	{
		if(httptalklist.empty()){
			printf("httptalklist is empty ! \n");
		}
		else{
			for(it=httptalklist.begin();it!=httptalklist.end();){
				if((*it).times <= getsystime()){
					printf("time is over: %u >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n",(*it).times);
					printf("systime is: %u >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n",getsystime());
					stoppushstream_fromclienturl((*it).url);
					it = httptalklist.erase(it);
				}else
					it++;
			}
			for(it=httptalklist.begin();it!=httptalklist.end();it++){
					printf("<<<<<<<<<<<<<<<<:%d,%s,%u \n",(*it).fd,(*it).url.c_str(),(*it).times);
			}
		}
		sleep(10);
	}
}
int httpclientinit()
{	
	//开启定时器线程检查时间戳是否过期
	creat_thread(checkstamp,(void*)"checktime");

    int sock_fd, new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
        perror("socket");
        return false;
    }
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
        perror("setsockopt");
        return false;
    }
    
    server_addr.sin_family = AF_INET;         // host byte order
    server_addr.sin_port = htons(MYPORT);     // short, network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
	{
        perror("bind socket failed!\n");
        return false;
    }
    if (listen(sock_fd, BACKLOG) == -1) 
	{
        perror("listen failed !\n");
        return false;
    }
    printf("listen port %d\n", MYPORT);
    fd_set fdsr;
    int maxsock;
    struct timeval tv;
    conn_amount = 0;
    sin_size = sizeof(client_addr);
    maxsock = sock_fd;
    while (1) 
	{
        FD_ZERO(&fdsr);
        FD_SET(sock_fd, &fdsr);
        // timeout setting
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        for (i = 0; i < BACKLOG; i++) 
		{
            if (fd_A[i] != 0)
			{
                FD_SET(fd_A[i], &fdsr);
            }
        }
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) 
		{
            perror("select");
            break;
        } 
		else if (ret == 0) 
        {
            continue;
        }
		else
		{
		}
        // check every fd in the set
        for (i = 0; i < BACKLOG; i++) 
		{
            if (FD_ISSET(fd_A[i], &fdsr))
			{
				ret = recv(fd_A[i], buf, sizeof(buf), 0);
                if (ret <= 0) //有异常链接或者断开链接
				{
                    printf("client[%d] close\n", i);
                    close(fd_A[i]);
                    FD_CLR(fd_A[i], &fdsr);

					delhttplistnode(fd_A[i]);
                    fd_A[i] = 0;
					conn_amount--;
					printmap();
                } 
				else //接收到数据
                {   
					if(!searchnode(fd_A[i]))
					{
						//buf为http请求，需要解析后存入map表中
						if(!getacceptstring(buf,1024,fd_A[i]))
						{
							senderror(fd_A[i]);
						}
						else
						{
							continue;
						}
					}
                }
            }
			else
			{
			}
        }
        // 新的连接
        if (FD_ISSET(sock_fd, &fdsr)) 
		{
			printf("---------------a new session ---------------------------\n");
            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
            if (new_fd <= 0) 
			{
                perror("accept");
                continue;
            }
            // add to fd queue
            if (conn_amount < BACKLOG)
			{
                //fd_A[conn_amount++] = new_fd;
                for(i = 0;i < BACKLOG;i++)
				{
					if(fd_A[i] == 0)
					{
						fd_A[i] = new_fd;
						break;
					}
				}
				conn_amount++;
                printf("new connection client[%d] %s:%d\n", conn_amount,
                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if (new_fd > maxsock)
                    maxsock = new_fd;
            }
            else
			{
                printf("max connections arrive, exit\n");
                send(new_fd, "bye", 4, 0);
                close(new_fd);
                continue;
            }
        }
        showclient();
    }
    for (i = 0; i < BACKLOG; i++) 
	{
        if (fd_A[i] != 0) 
		{
            close(fd_A[i]);
        }
    }
    return 0;
}
