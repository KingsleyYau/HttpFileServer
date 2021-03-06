/*
 * server.cpp
 *
 *  Created on: 2015-9-28
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <string>
#include <map>
using namespace std;

#include "HttpFileServer.h"
#define VERSION_STRING "Version : 1.0.0"

string sConf = "";  // 配置文件

bool Parse(int argc, char *argv[]);

void sig_chld(int signo) {
	pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
    }

    return;
}

int main(int argc, char *argv[]) {
	printf("############## Http File Server ############## \n");
	printf("# %s \n", VERSION_STRING);
	printf("# Build date : %s %s \n", __DATE__, __TIME__ );

	srand(time(0));

	/* Ignore SIGPIPE */
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, 0);
	sigaction(SIGCHLD, &sa, 0);

	Parse(argc, argv);

	HttpFileServer server;
	if( sConf.length() > 0 ) {
		server.Run(sConf);
	} else {
		printf("# Usage : ./httpfileserver [ -f <config file> ] \n");
		server.Run("/etc/httpfileserver.config");
	}

	return EXIT_SUCCESS;
}

bool Parse(int argc, char *argv[]) {
	string key, value;
	for( int i = 1; (i + 1) < argc; i+=2 ) {
		key = argv[i];
		value = argv[i+1];

		if( key.compare("-f") == 0 ) {
			sConf = value;
		}
	}

	return true;
}
