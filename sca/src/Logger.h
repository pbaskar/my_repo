/*
 * Logger.h
 *
 *  Created on: Nov 9, 2023
 *      Author: prbas_000
 */

#ifndef LOGGER_H_
#define LOGGER_H_
#include<cstdarg>

enum ErrorCode {
	INVALID_CHARACTER,
	NOT_FOUND,
	NOT_PARSE
};

static const char errorMessage[][80] = {
		"Invalid character %1",
		"Character %1 not found",
		"Could not parse %1"
};

class Logger {
public:
	Logger();
	virtual ~Logger();

	static void logMessage(ErrorCode errorCode, int args, ...);
};

#endif /* LOGGER_H_ */
