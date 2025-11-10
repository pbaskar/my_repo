/*
 * Utils.h
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#ifndef UTILS_H_
#define UTILS_H_
#include "Results.h"

class Utils {
public:
    Utils();
    virtual ~Utils();
    static const char* makeWord(const char* word);
    static char* makeWordNConst(const char* word);
    static const char* makeWord(const char* word1, const char* word2);
};

#endif /* UTILS_H_ */
