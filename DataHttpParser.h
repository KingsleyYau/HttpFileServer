/*
 * DataHttpParser.h
 *
 *  Created on: 2015-9-28
 *      Author: Max
 */

#ifndef DATAHTTPPARSER_H_
#define DATAHTTPPARSER_H_

#include "DataParser.h"
#include "MessageList.h"

#include <common/Arithmetic.hpp>

#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <algorithm>

#include <string>
#include <map>
using namespace std;

typedef map<string, string> Parameters;

typedef enum HttpType {
	GET,
	POST,
	UNKNOW,
};

class DataHttpParser : public DataParser {
public:
	DataHttpParser();
	virtual ~DataHttpParser();

	int ParseData(char* buffer, int len);
	void Reset();

	HttpType GetType();
	string GetMethod();
	string GetPath();
	string GetQueryString();

private:
	HttpType mHttpType;
	string mMethod;
	string mPath;
	string mQueryString;

	bool ParseFirstLine(char* buffer);
};

#endif /* DATAHTTPPARSER_H_ */
