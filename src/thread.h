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
socket�����̣߳������ȴ��ͻ��˵����ӣ� 
���Ӻ�ѭ������ͻ��˷��������ݣ� 
����ֻ������һ���ͻ��ˣ��Լ��޸ģ��Ϳ���֧�ֶ�ͻ������� 
*/  
void *SocketThread(void *arg);
/* 
socket���ӳ�ʼ�� 
*/  
int SocketInit();

#endif