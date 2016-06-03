#ifndef _CAMERALIST_
#define _CAMERALIST_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <json/json.h>
#include "../include/Sadp.h"
#include <vector>

void startsearch();

void stopsearch();

std::string getcameralist();


#endif
