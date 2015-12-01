# Copyright (C) 2015 The HttpFileServer
# HttpFileServer Makefile
#
# Created on: 2015/10/10
# Author: Max.Chiu
# Email: Kingsleyyau@gmail.com
#
export MAKE	:=	make

release=0
ifeq ($(release), 1)
CXXFLAGS = -O3 
else
CXXFLAGS = -O2 -g
endif

CXXFLAGS +=	-Wall -fmessage-length=0 -Wunused-variable
CXXFLAGS +=	-I. -Ijson -Iinclude

COMMONOBJ =	common/LogFile.o common/md5.o common/KThread.o \
			common/ConfFile.o common/Arithmetic.o
OBJS =		server.o HttpFileServer.o MessageList.o LogManager.o DataHttpParser.o DataParser.o
OBJS +=		$(COMMONOBJ)

LIBS =	-Wl,-Bdynamic -ldl -lz -lpthread 

TARGET =	httpfileserver

.PHONY: all clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	@echo '################################################################'
	@echo ''
	@echo '# Bulid HttpFileServer completed!'
	@echo ''
	@echo '################################################################'


all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
