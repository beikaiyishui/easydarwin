#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <netdb.h>  
#include <fcntl.h>  
#include <sys/epoll.h>  
#include <string.h>  

#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "HCNetSDK.h"
//����:  
//����:�����Ͱ�һ��TCP socket  
//����:�˿�  
//����ֵ:������socket  
static int  create_and_bind (char *port);

//����  
//����:����socketΪ��������  
static int  make_socket_non_blocking (int sfd);

int main_main();