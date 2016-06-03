#include <stdio.h>
#include <stdlib.h>
#include "public.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"
#include <sys/prctl.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>


#ifndef _PUSHSTREAM_H_
#define _PUSHSTREAM_H_

int rtsppush(char *src_url, char *dst_url ,int pipe_fd);

#endif