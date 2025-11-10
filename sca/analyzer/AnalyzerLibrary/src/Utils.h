/*
 * Utils.h
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#ifndef UTILS_H_
#define UTILS_H_
#include "Results.h"
#include "Expr.h"

class Utils {
public:
    Utils();
    virtual ~Utils();
    static const char* makeWord(const char* word);
    static char* makeWordNConst(const char* word);
    static const char* makeWord(const char* word1, const char* word2);
    static bool isAlpha(char c);
    static Status populateMallocFnCall(const Variable* lhs, Expr* value);
    static Status populateDefinitions(const Variable* lhs, Expr* value);
};

#endif /* UTILS_H_ */
