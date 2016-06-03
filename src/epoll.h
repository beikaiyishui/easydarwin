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
//函数:  
//功能:创建和绑定一个TCP socket  
//参数:端口  
//返回值:创建的socket  
static int  create_and_bind (char *port);

//函数  
//功能:设置socket为非阻塞的  
static int  make_socket_non_blocking (int sfd);

int main_main();