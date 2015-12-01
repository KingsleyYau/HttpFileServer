/*
 * HttpFileServer.cpp
 *
 *  Created on: 2015-11-30
 *      Author: Max
 */

#include "HttpFileServer.h"

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
		if( pid == 0 ) {
			// child
			HandleChildRequest(client);
	 	} else if( pid > 0 ) {
	 		// parent
		}

		close(client);

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
	LogManager::GetLogManager()->Log(
			LOG_MSG,
			"HttpFileServer::HandleChildRequest( "
			"client : %d, "
			"start "
			")",
			client
			);

	char buffer[1025] = {'\0'};
	int len = 0;
	int ret = -1;

	len = GetLine(client, buffer, sizeof(buffer) - 1);
	buffer[len] = '\0';

	DataHttpParser dataHttpParser;
	ret = dataHttpParser.ParseData(buffer, len);

	LogManager::GetLogManager()->Log(
			LOG_MSG,
			"HttpFileServer::HandleChildRequest( "
			"client : %d, "
			"ret : %d, "
			"buffer : %s "
			")",
			client,
			ret,
			buffer
			);

	if( ret == 1 ) {
	    char curPath[PATH_MAX + 1];
	    if( NULL != realpath("./", curPath) ) {
	    	string realPath = curPath;
	    	realPath += dataHttpParser.GetPath();

	    	LogManager::GetLogManager()->Log(
	    			LOG_MSG,
	    			"HttpFileServer::HandleChildRequest( "
	    			"client : %d, "
	    			"realPath : %s "
	    			")",
	    			client,
	    			realPath.c_str()
	    			);

			ExecuteCGI(
					client,
					realPath.c_str(),
					dataHttpParser.GetMethod().c_str(),
					dataHttpParser.GetQueryString().c_str()
					);
	    }
	}
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
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
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
		const char *method,
		const char *query_string
		) {
	LogManager::GetLogManager()->Log(
			LOG_MSG,
			"HttpFileServer::ExecuteCGI( "
			"client : %d, "
			"path : %s, "
			"method : %s, "
			"query_string : %s "
			")",
			client,
			path,
			method,
			query_string
			);

	char buf[1024];
	int cgi_pipe[2];
	pid_t pid;
	int status;
	int i;
	//char c;
	int numchars = 1;
	int content_length = -1;
	char buffer[1024];
	int ret;
	char content_type[255];

	buf[0] = 'A'; buf[1] = '\0';
	if (strcasecmp(method, "GET") == 0) {
		while ((numchars > 0) && strcmp("\n", buf)) {
			numchars = GetLine(client, buf, sizeof(buf));
		}
	} else if (strcasecmp(method, "POST") == 0) {
		numchars = GetLine(client, buf, sizeof(buf));
		while ((numchars > 0) && strcmp("\n", buf)) {
			if (strncasecmp(buf, "Content-Length:", 15) == 0) {
				content_length = atoi(&(buf[16]));
			}
			if (strncasecmp(buf, "Content-Type:", 13) == 0) {
				char *p = &buf[14];
				for(i=0; i<numchars && p[i] != ';'; i++) {
					content_type[i] = p[i];
				}
				content_type[i] = 0;
			}
			numchars = GetLine(client, buf, sizeof(buf));
		}

		if (content_length == -1) {
			close(client);
			return;
		}
	}

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);

	if( socketpair(AF_UNIX, SOCK_STREAM, 0, cgi_pipe) < 0 ) {
		close(client);
		return;
	}

	if ( (pid = fork()) < 0 ) {
		close(client);
		return;
	}

	if (pid == 0) {
		/**
		 * child process
		 */

		char meth_env[255];
		sprintf(meth_env, "REQUEST_METHOD=%s", method);
		putenv(meth_env);

		if (strcasecmp(method, "GET") == 0) {
			char query_env[255];
			sprintf(query_env, "QUERY_STRING=%s", query_string);
			putenv(query_env);
		} else if (strcasecmp(method, "POST") == 0) {
			char length_env[255];
			sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(length_env);

			char content_env[255];
			sprintf(content_env, "CONTENT_TYPE=%s", content_type);
			putenv(content_env);
		}

		/**
		 * close parent pipe
		 */
		close(cgi_pipe[0]);

		/**
		 * dup2 child write pipe
		 */
		dup2(cgi_pipe[1], STDOUT_FILENO);

		/**
		 * dup2 child read pipe
		 */
		dup2(cgi_pipe[1], STDIN_FILENO);

//		/**
//		 * close child read pipe
//		 */
//		close(cgi_pipe[1]);

		execl(path, path, NULL);

		exit(0);

	} else {
		/**
		 * parent process
		 */
		/**
		 * close child pipe
		 */
		close(cgi_pipe[1]);

		if (strcasecmp(method, "POST") == 0) {
			for (i = 0; i < content_length;) {
				/**
				 * recv from client
				 */
				if ((ret = recv(client, buffer, sizeof(buffer) - 1, 0)) < 0) {
					perror("recv");
					break;
				}

				buffer[ret] = '\0';
				LogManager::GetLogManager()->Log(
						LOG_STAT,
						"HttpFileServer::ExecuteCGI( "
						"client : %d, "
						"request(%d) : \n%s"
						")",
						client,
						ret,
						buffer
						);

				/**
				 * write to parent pipe
				 */
				if (write(cgi_pipe[0], buffer, ret) < 0) {
					perror("write");
					break;
				}
				i += ret;

	//			 /**
	//			  * close child pipe input
	//			  */
	//			 shutdown(cgi_pipe[0], SHUT_WR);
			}
		}

		/**
		 * read from parent pipe
		 */
		while ((ret = read(cgi_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[ret] = '\0';

			LogManager::GetLogManager()->Log(
					LOG_STAT,
					"HttpFileServer::ExecuteCGI( "
					"client : %d, "
					"respond(%d) : \n%s"
					")",
					client,
					ret,
					buffer
					);

			/**
			 * send to client
			 */
			send(client, buffer, ret, 0);
		}

//		 /**
//		  * close parent pipe
//		  */
//		 close(cgi_pipe[0]);

		waitpid(pid, &status, 0);
	}

	LogManager::GetLogManager()->Log(
			LOG_MSG,
			"HttpFileServer::ExecuteCGI( "
			"client : %d, "
			"finish "
			")",
			client
			);
}
