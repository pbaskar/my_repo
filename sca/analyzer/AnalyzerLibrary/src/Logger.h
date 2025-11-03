/*
 * Logger.h
 *
 *  Created on: Nov 9, 2023
 *      Author: prbas_000
 */

#ifndef LOGGER_H_
#define LOGGER_H_
#include<cstdarg>
#include<fstream>
#include<sstream>
#include<vector>
#include "Results.h"
using namespace std;

enum ErrorCode {
    CHAR_INVALID,
    STMT_INVALID,
    NOT_FOUND,
    NOT_PARSE,
    FILE_NOT_OPEN,
    FILE_EMPTY
};

static const char errorMessage[][80] = {
        "Invalid character %1",
        "Invalid statement",
        "Character %1 not found",
        "Could not parse %1",
        "Could not open file %1",
        "File %1 is empty"
};

class Logger {
public:
    Logger();
    virtual ~Logger();

    ofstream& getOfstream() { return p_of; }
    static Logger* getInstance() {
        static Logger logger;
        return &logger;
    }

    static ostream& getDebugStreamInstance() {
        static stringstream p_debug;
        return p_debug;
    }
    Status setErrorFile(const char* errorFile);
    void addError(char* r) { p_errors.push_back(r); }
    void clearErrors() {
        for (int i = 0; i < p_errors.size(); i++) {
            delete p_errors[i];
        }
        p_errors.clear();
    }
    const std::vector<char*>& getErrors() const {
        return p_errors;
    }
    static void logMessage(ErrorCode errorCode, int args, ...);
private:
    ofstream p_of;
    std::vector<char*> p_errors;
};

#endif /* LOGGER_H_ */