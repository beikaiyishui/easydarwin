#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"
#include "../include/EasyPusherAPI.h"
#include "../include/EasyRTSPClientAPI.h"

int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr);

int Easy_APICALL __RTSPSourceCallBack( int _chid, int *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo);

int rtsppush(char *src_url, char *dst_url);
int pushrtsp();