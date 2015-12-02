/*
 * DataHttpParser.cpp
 *
 *  Created on: 2015-9-28
 *      Author: Max
 */

#include "DataHttpParser.h"

DataHttpParser::DataHttpParser() {
	// TODO Auto-generated constructor stub
	Reset();
}

DataHttpParser::~DataHttpParser() {
	// TODO Auto-generated destructor stub
}

void DataHttpParser::Reset() {
	mHttpType = UNKNOW;
	mMethod = "";
	mQueryString = "";
}

int DataHttpParser::ParseData(char* buffer, int len) {
	int result = 0;

	if( ParseFirstLine(buffer) ) {
		result = 1;
	}

	return result;
}

HttpType DataHttpParser::GetType() {
	return mHttpType;
}

string DataHttpParser::GetMethod() {
	return mMethod;
}

string DataHttpParser::GetPath() {
	return mPath;
}

string DataHttpParser::GetQueryString() {
	return mQueryString;
}

bool DataHttpParser::ParseFirstLine(char* buffer) {
	bool bFlag = false;
	char temp[1024];
	char* p = NULL;
	int j = 0;

	char *pFirst = NULL;

	p = strtok_r(buffer, " ", &pFirst);
	while( p != NULL ) {
		switch(j) {
		case 0:{
			// type
			if( strcmp("GET", p) == 0 ) {
				mHttpType = GET;
				mMethod = "GET";
				bFlag = true;
			} else if( strcmp("POST", p) == 0 ) {
				mHttpType = POST;
				mMethod = "POST";
				bFlag = true;
			} else {
				bFlag = false;
				break;
			}
		}break;
		case 1:{
			// path and parameters
			char path[1025] = {'\0'};
			char query[1025] = {'\0'};
			Arithmetic ari;
			int len = strlen(p);
			len = ari.decode_url(p, len, temp);
			char* pPath = strstr(temp, "?");
			if( pPath != NULL && ((pPath + 1) != NULL) ) {
				len = pPath - temp;
				memcpy(path, temp, len);
				memcpy(query, pPath + 1, strlen(p) - len - 1);
			} else {
				len = strlen(temp);
				memcpy(path, temp, len);
			}
			path[len] = '\0';
			mPath = path;
			mQueryString = query;
		}break;
		default:break;
		};

		j++;
		p = strtok_r(NULL, " ", &pFirst);
	}

	return bFlag;
}
