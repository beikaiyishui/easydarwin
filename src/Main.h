#include "threadpool.h"
#include <string.h>
#include <string>

void creat_thread(void* (*callback_function)(void *arg), void *arg);
int fork_stream(std::string src_url,std::string dst_url);

int get_writepipe(std::string url);

char *get_uuid();

void stoppushstream_fromclienturl(std::string clienturl);