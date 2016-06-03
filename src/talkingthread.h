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
#include "../include/Sadp.h"

#ifndef _TALKINGTHREAD_

#define _TALKINGTHREAD_

class session
{
public:
	session();
	~session();
	int socket_fun();
	int RevInfomation(char *buffer);
	int creattimer(int delay);

	//static int client_socket;
};

#endif
