######################################
#
#Makefile文件
#
#编译参数
CXX   	=	g++ -c -g -Wall -Iinclude -o
CC	  	=	gcc
LIBS 	=	-Wl,-rpath=/linux64/lib/ -Llinux64/lib/ -lHCCore -lhpr -lhcnetsdk -lHCPreview -lHCAlarm -lHCCoreDevCfg \
	-lStreamTransClient -lSystemTransform -lHCVoiceTalk -lHCIndustry -lHCPlayBack -lEasyPusher -leasyrtspclient -lpthread -lsadp -lssl 

CFLAGS += -std=c99 -Iinclude -Wall -ggdb -O2
EXECUTABLE = sonia1

obj =	src/Alarm.o \
		src/CapPicture.o \
		src/ConfigParams.o \
		src/pushstream.o \
		src/thread.o \
		src/config.o \
		src/GetStream.o \
		src/playback.o \
		src/tool.o \
		src/Voice.o \
		src/json/json_reader.o \
		src/json/json_value.o \
		src/json/json_writer.o \
		src/threadpool.o \
		src/talkingthread.o \
		src/CTimer.o \
		src/cameralist.o \
		src/camera_config.o \
		src/file.o \
		src/streamclient.o \
		src/Main.o 
#http解析库
httpobj = src/http/libh3.a
#xml解析库
xmlobj = src/tinyxml/tinyxml.a

all:$(httpobj) $(xmlobj) $(EXECUTABLE)
	@echo export LD_LIBRARY_PATH=/home/li_weizhao/svn/c++/linux64/lib
	@echo ++++++++++++++++++++++++++++++++++编译完成+++++++++++++++++++++++++++
src/%.o: src/%.cpp
	$(CXX) $@ $<
src/json/%.o: src/json/%.cpp
	$(CXX) $@ $<

$(EXECUTABLE):$(obj)
	g++ -o $(EXECUTABLE)  $(obj) $(httpobj) $(xmlobj) -Iinclude -Wall -O2 $(LIBS)
$(httpobj):
	cd src/http;make all
$(xmlobj):
	cd src/tinyxml;make
.PHONY: clean
clean:
	rm -rf $(obj) $(EXECUTABLE)

