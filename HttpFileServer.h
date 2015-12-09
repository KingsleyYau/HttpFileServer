/*
 * HttpFileServer.h
 *
 *  Created on: 2015-11-30
 *      Author: Max
 */

#ifndef HTTPFILESERVER_H_
#define HTTPFILESERVER_H_

#include "LogManager.h"
#include "DataHttpParser.h"

#include <common/ConfFile.hpp>
#include <common/KSafeMap.h>
#include <common/TimeProc.hpp>
#include <common/StringHandle.h>

#include <map>
#include <list>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <sys/syscall.h>

class HttpFileServer {
public:
	HttpFileServer();
	virtual ~HttpFileServer();

	void Run(const string& config);
	void Run();
	bool Reload();
	bool IsRunning();

private:

	void HandleChildRequest(int client);
	int GetLine(int sock, char *buf, int size);
	void ExecuteCGI(int client,	const char *path, const char *method, const char *query_string);
	void GetFile(int client, const char *path);
	void Headers(int client, const char *path);
	void Cat(int client, FILE *resource);

	int mServer;

	// BASE
	short miPort;

	// LOG
	int miLogLevel;
	string mLogDir;
	int miDebugMode;

	/**
	 * 是否运行
	 */
	bool mIsRunning;

	/**
	 * 配置文件锁
	 */
	KMutex mConfigMutex;

	/**
	 * 配置文件
	 */
	string mConfigFile;
};

#endif /* HTTPFILESERVER_H_ */
