#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>

#ifndef _CTIMER_
#define _CTIMER_

typedef void (*TimeCallbackfun)(int time_s);
void signalHandler(int signo);
int start(int time_s);
bool RegTimeCallback(TimeCallbackfun fun,int times);


#endif
