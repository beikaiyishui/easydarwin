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
	 	{fputs("File error", stderr); exit(1);}    // ����ļ������˳�1 
	 fseek(pFile , 0 , SEEK_END); // ָ���Ƶ��ļ�ĩλ   
    lSize = ftell(pFile);  // ����ļ�����   
    rewind(pFile);  // ����rewind()���ļ�ָ���Ƶ���stream(��)ָ���Ŀ�ʼ��, ͬʱ���������صĴ����EOF��� 
    buffer = (char*) malloc(sizeof(char) * lSize); // ���仺��������ǰ��� lSize   
    if (buffer == NULL) 
		{fputs("Memory error", stderr); exit(2);}  // �ڴ��������˳�2 
    result = fread(buffer, 1, lSize, pFile); // ����ֵ�Ƕ�ȡ����������   
    

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
	//֪ͨ�ص�����
	for(it = vv.begin();it != vv.end();it++)
	{
		callback(*it,m_config,name);
	}	
	Json::Reader reader_config;
		Json::Value value_config;
		 pFile = fopen("config_hongxin/config", "rb");
		 if (pFile == NULL) 
			{fputs("File error", stderr); exit(1);}    // ����ļ������˳�1 
		 fseek(pFile , 0 , SEEK_END); // ָ���Ƶ��ļ�ĩλ	
		lSize = ftell(pFile);  // ����ļ�����	 
		rewind(pFile);	// ����rewind()���ļ�ָ���Ƶ���stream(��)ָ���Ŀ�ʼ��, ͬʱ���������صĴ����EOF��� 
		buffer = (char*) malloc(sizeof(char) * lSize + sizeof(m_config)); // ���仺��������ǰ��� lSize   
		if (buffer == NULL) 
			{fputs("Memory error", stderr); exit(2);}  // �ڴ��������˳�2 
		result = fread(buffer, 1, lSize, pFile); // ����ֵ�Ƕ�ȡ����������	 
		
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
		 //free(buffer); //buffer ��С���ͱ仯��ֱ��free�ᵼ������
		 buffer = NULL;
		 return true;
	
}
