/*
 * Logger.cpp
 *
 *  Created on: Nov 9, 2023
 *      Author: prbas_000
 */

#include "Logger.h"
#include<iostream>
#include<cstring>

using namespace std;

Logger::Logger() {
    // TODO Auto-generated constructor stub
}

Logger::~Logger() {
    // TODO Auto-generated destructor stub
}

Status Logger::openErrorFile(const char* errorFile) {
    p_of.open(errorFile);
    if (!p_of.is_open()) {
        std::cout << "could not open error log " << endl;
        return FAILURE;
    }
    return SUCCESS;
}

Status Logger::closeErrorFile() {
    p_of.close();
    return SUCCESS;
}

void Logger::logMessage(ErrorCode errorCode, int num, ...) {
    va_list argsList;
    va_start(argsList, num);
    const size_t outputSize = 180;
    char output[outputSize] = {'\0'};

    strcat(output, va_arg(argsList, char*));
    strcat(output, " ");
    const char* message = errorMessage[errorCode];
    const char* strBegin=message;
    int len=0;

    while(*message != '\0') {
        if(*message == '%') {
            strncat(output, strBegin, len);
            ++message;
            char* arg = va_arg(argsList, char*);
            //if(arg == nullptr) return FAILURE;
            strcat_s(output, arg );
            message++;
            len=0;
            strBegin = message;
        }
        else {
            message++;
            len++;
        }
    }
    strncat(output, strBegin, len);

    va_end(argsList);

    ofstream& of = getInstance()->getOfstream();
    for(char c: output) {
        if (c== '\0') break;
        of << c;
    }
    of <<endl;
    char* errorMsg = new char[strlen(output) + 1];
    errorMsg[0] = '\0';
    strncat(errorMsg, output, strlen(output));
    errorMsg[strlen(output)] = '\0';
    getInstance()->addError(errorMsg);
    getDebugStreamInstance() << "error message " << errorMsg << endl;
}
