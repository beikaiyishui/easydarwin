#include <json/json.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "config.h"


FILE *pFile = NULL;
char *buffer;
long lSize;
size_t result;

std::vector<CallBackFun> vv;
std::vector<CallBackFun>::iterator it;


bool getConfig(char *name,Json::Value &m_config)
{	
	Json::Reader reader_config;
	Json::Value value_config;
	 pFile = fopen("config_hongxin/config", "rb");
	 if (pFile == NULL) 
	 	{fputs("File error", stderr); exit(1);}    // 如果文件错误，退出1 
	 fseek(pFile , 0 , SEEK_END); // 指针移到文件末位   
    lSize = ftell(pFile);  // 获得文件长度   
    rewind(pFile);  // 函数rewind()把文件指针移到由stream(流)指定的开始处, 同时清除和流相关的错误和EOF标记 
    buffer = (char*) malloc(sizeof(char) * lSize); // 分配缓冲区，按前面的 lSize   
    if (buffer == NULL) 
		{fputs("Memory error", stderr); exit(2);}  // 内存分配错误，退出2 
    result = fread(buffer, 1, lSize, pFile); // 返回值是读取的内容数量   
    

	 std::string config_buf = buffer;
	 //printf("%s\n",config_buf.c_str());
	 if(reader_config.parse(config_buf,value_config))
	 {
	 	//printf(" %s \n",value_config.toStyledString().c_str());
	 }
	 m_config = value_config[name];

	//printf("getconfig: %s \n",m_config.toStyledString().c_str());
	 
	 fclose(pFile);  
    free(buffer); 
	return true;
}
int addcallback(CallBackFun fun)
{
	vv.push_back(fun);
}
int callback(CallBackFun fun, Json::Value config ,char *configname )
{
	fun(config ,configname);
	return true;
}
bool setConfig(char *name,Json::Value m_config)
{
	//通知回调函数
	for(it = vv.begin();it != vv.end();it++)
	{
		callback(*it,m_config,name);
	}	
	Json::Reader reader_config;
		Json::Value value_config;
		 pFile = fopen("config_hongxin/config", "rb");
		 if (pFile == NULL) 
			{fputs("File error", stderr); exit(1);}    // 如果文件错误，退出1 
		 fseek(pFile , 0 , SEEK_END); // 指针移到文件末位	
		lSize = ftell(pFile);  // 获得文件长度	 
		rewind(pFile);	// 函数rewind()把文件指针移到由stream(流)指定的开始处, 同时清除和流相关的错误和EOF标记 
		buffer = (char*) malloc(sizeof(char) * lSize + sizeof(m_config)); // 分配缓冲区，按前面的 lSize   
		if (buffer == NULL) 
			{fputs("Memory error", stderr); exit(2);}  // 内存分配错误，退出2 
		result = fread(buffer, 1, lSize, pFile); // 返回值是读取的内容数量	 
		
		 std::string config_buf = buffer;
		 if(reader_config.parse(config_buf,value_config))
		 {
			printf(" %s \n",value_config.toStyledString().c_str());
		 }
		 value_config[name] = m_config;

		 strcpy(buffer ,value_config.toStyledString().c_str());
		 printf("buffer : %s \n",buffer);

		 pFile = fopen("config_hongxin/config", "wb");
		 fwrite(buffer,1,strlen(buffer), pFile);
		
		 fclose(pFile); 
		 //free(buffer); //buffer 大小发送变化，直接free会导致死机
		 buffer = NULL;
		 return true;
	
}
