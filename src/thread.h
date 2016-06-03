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
  
#ifndef _THREAD_
#define _THREAD_
int getvideobuf(unsigned char *buf,unsigned int len,int type);
  
//int ScketSend(unsigned char *buf,unsigned int len);

int ScketSend();
  
/* 
socket连接线程，阻塞等待客户端的连接， 
连接后循环处理客户端发来的数据， 
这里只能连接一个客户端，稍加修改，就可以支持多客户端连接 
*/  
void *SocketThread(void *arg);
/* 
socket连接初始化 
*/  
int SocketInit();

#endif