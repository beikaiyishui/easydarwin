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
#include <json/json.h>


typedef int (*CallBackFun)(Json::Value config, char *configname);

bool getConfig(char *name,Json::Value &m_config);

bool setConfig(char *name,Json::Value m_config);

int addcallback(CallBackFun fun);


