#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "CTimer.h"
#include <vector>

std::vector<TimeCallbackfun> list;
std::vector<TimeCallbackfun>::iterator fun_time;
int delay_time = 0;


void CTimercallback(TimeCallbackfun fun,int time_s)
{
	fun(time_s);
	return;
}
bool RegTimeCallback(TimeCallbackfun fun,int times)
{
	list.push_back(fun);
	delay_time = times;
	return true;
}
int start(int time_s)
{  
   struct itimerval tv, otv;  
   signal(SIGALRM, signalHandler);  
   //how long to run the first time  
   tv.it_value.tv_sec = 3;  
   tv.it_value.tv_usec = 0;  
   //after the first time, how long to run next time  
   tv.it_interval.tv_sec = 1;  
   tv.it_interval.tv_usec = 0;  
  
   if (setitimer(ITIMER_REAL, &tv, &otv) != 0)  
    printf("setitimer err\n");  
  
   while(1)  
   {  
    sleep(1);  
	}
}
void signalHandler(int signo)
{  
    switch (signo){  
        case SIGALRM:  
            printf("Caught the SIGALRM signal!\n");
			for(fun_time=list.begin();fun_time!=list.end();fun_time++)
			{
				CTimercallback(*fun_time,delay_time);
			}
            break;
   }
}  



