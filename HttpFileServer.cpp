/*
 * HttpFileServer.cpp
 *
 *  Created on: 2015-11-30
 *      Author: Max
 */

#include "HttpFileServer.h"

#define ISspace(x) isspace((int)(x))

HttpFileServer::HttpFileServer() {
	// TODO Auto-generated constructor stub

}

HttpFileServer::~HttpFileServer() {
	// TODO Auto-generated destructor stub
}

void HttpFileServer::Run(const string& config) {
	if( config.length() > 0 ) {
		mConfigFile = config;

		// Reload config
		if( Reload() ) {
			if( miDebugMode == 1 ) {
				LogManager::LogSetFlushBuffer(0);
			} else {
				LogManager::LogSetFlushBuffer(5 * BUFFER_SIZE_1K * BUFFER_SIZE_1K);
			}

			Run();
		} else {
			printf("# Snifer Server can not load config file exit. \n");
		}

	} else {
		printf("# No config file can be use exit. \n");
	}
}

void HttpFileServer::Run() {
	/* log system */
	LogManager::GetLogManager()->Start(1000, miLogLevel, mLogDir);
	LogManager::GetLogManager()->Log(
			LOG_MSG,
			"HttpFileServer::Run( "
			"miPort : %d, "
			"miLogLevel : %d, "
			"mlogDir : %s "
			")",
			miPort,
			miLogLevel,
			mLogDir.c_str()
			);


	mIsRunning = true;

	printf("# HttpFileServer start OK. \n");
	LogManager::GetLogManager()->Log(LOG_WARNING, "HttpFileServer::Run( Init OK )");

	/* bind server socket */
	struct sockaddr_in ac_addr;
	if ((mServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		LogManager::GetLogManager()->Log(LOG_ERR_SYS, "HttpFileServer::Run( Create socket error )");
		printf("# Create socket error \n");
		return;
	}

	int so_reuseaddr = 1;
	setsockopt(mServer, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

	bzero(&ac_addr, sizeof(ac_addr));
	ac_addr.sin_family = PF_INET;
	ac_addr.sin_port = htons(miPort);
	ac_addr.sin_addr.s_addr = INADDR_ANY;

	if ( bind(mServer, (struct sockaddr *) &ac_addr, sizeof(struct sockaddr))== -1 ) {
		LogManager::GetLogManager()->Log(LOG_ERR_SYS, "HttpFileServer::Run( Bind socket error )");
		printf("# Bind socket error \n");
		return;
	}

	if ( listen(mServer, 1024) == -1 ) {
		LogManager::GetLogManager()->Log(LOG_ERR_SYS, "HttpFileServer::Run( Listen socket error )");
		printf("# Listen socket error \n");
		return;
	}

	/* call server */
	while( true ) {
		pid_t pid;
		int client;

		struct sockaddr_in addr;
		socklen_t iAddrLen = sizeof(struct sockaddr);
		client = accept(mServer, (struct sockaddr *)&addr, &iAddrLen);

		pid = fork();
		if(pid == 0) {
			// child
			HandleChildRequest(client);
	 	} else if( pid > 0 ) {
	 		// parent
			close(client);
		}

		/* do nothing here */
		sleep(5);
	}
}

bool HttpFileServer::Reload() {
	bool bFlag = false;
	mConfigMutex.lock();
	if( mConfigFile.length() > 0 ) {
		ConfFile conf;
		conf.InitConfFile(mConfigFile.c_str(), "");
		if ( conf.LoadConfFile() ) {
			// BASE
			miPort = atoi(conf.GetPrivate("BASE", "PORT", "9876").c_str());

			// LOG
			miLogLevel = atoi(conf.GetPrivate("LOG", "LOGLEVEL", "5").c_str());
			mLogDir = conf.GetPrivate("LOG", "LOGDIR", "log");
			miDebugMode = atoi(conf.GetPrivate("LOG", "DEBUGMODE", "0").c_str());

			LogManager::GetLogManager()->Log(
					LOG_WARNING,
					"HttpFileServer::Reload( "
					"miPort : %d, "
					"miLogLevel : %d, "
					"mlogDir : %s "
					")",
					miPort,
					miLogLevel,
					mLogDir.c_str()
					);

			bFlag = true;
		}
	}
	mConfigMutex.unlock();
	return bFlag;
}

bool HttpFileServer::IsRunning() {
	return mIsRunning;
}

void HttpFileServer::HandleChildRequest(int client) {
	char buf[1024];
	int numchars;
	char method[255];
	char url[255];
	char path[512];
	size_t i, j;
	struct stat st;
	int cgi = 0;      /* becomes true if server decides this is a CGI
	                    * program */
	char *query_string = NULL;

	int ret = GetLine(client, buf, sizeof(buf));

	i = 0; j = 0;
	while (!ISspace(buf[j]) && (i < sizeof(method) - 1)) {
		method[i] = buf[j];
		i++; j++;
	}
	method[i] = '\0';

	if (strcasecmp(method, "POST") != 0) {
		return;
	}

	i = 0;

	while (ISspace(buf[j]) && (j < sizeof(buf))) {
		j++;
	}

	while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf))) {
		url[i] = buf[j];
		i++; j++;
	}
	url[i] = '\0';

	sprintf(path, ".%s", url);
	ExecuteCGI(client, path, method);
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int HttpFileServer::GetLine(int sock, char *buf, int size) {
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')) {
		n = recv(sock, &c, 1, 0);
		/* DEBUG printf("%02X\n", c); */
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
				/* DEBUG printf("%02X\n", c); */
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';

	return(i);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/


void HttpFileServer::ExecuteCGI(
		int client,
		const char *path,
		const char *method
		) {
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int i;
	//char c;
	int numchars = 1;
	int content_length = -1;
	char buffer[1024];
	int ret;
	char content_type[255];
}
