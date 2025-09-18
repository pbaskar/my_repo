/*
 * Utils.cpp
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#include "Utils.h"
#include <cstring>

Utils::Utils() {
    // TODO Auto-generated constructor stub

}

Utils::~Utils() {
    // TODO Auto-generated destructor stub
}

const char* Utils::makeWord(const char* word) {
    int len = strlen(word);
    char* p = new char[len+1];
    p[len] = '\0';
    strncpy(p, word,len);
    return p;
}
