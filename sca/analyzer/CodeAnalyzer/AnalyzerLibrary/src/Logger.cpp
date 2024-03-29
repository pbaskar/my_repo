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

void Logger::logMessage(ErrorCode errorCode, int num, ...) {
    va_list argsList;
    va_start(argsList, num);

    char output[80] = {'\0'};
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
            strcat(output, arg );
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

    for(char c: output) {
        if (c== '\0') break;
        cout << c;
    }
    cout <<endl;
}
