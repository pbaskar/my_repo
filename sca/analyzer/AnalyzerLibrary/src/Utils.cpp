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

char* Utils::makeWordNConst(const char* word) {
    int len = strlen(word);
    char* p = new char[len + 1];
    p[len] = '\0';
    strncpy(p, word, len);
    return p;
}

const char* Utils::makeWord(const char* word1, const char* word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    char* newName = new char[len1+len2+1];
    newName[len1] = '\0';
    strncpy(newName, word1, len1);
    newName[len1 + len2] = '\0';
    strcat(newName, word2);
    return newName;
}
