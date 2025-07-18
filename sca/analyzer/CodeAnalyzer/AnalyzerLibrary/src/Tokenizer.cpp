/*
 * Tokenizer.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<cstring>
#include<memory>
#include<iostream>
#include "Tokenizer.h"
using namespace std;

static const char delimiters[] = { '=', ';', '(', ')', '{', '}', ',', '[', ']'};
Tokenizer::Tokenizer() : p_pos(0) {
    // TODO Auto-generated constructor stub

}

Tokenizer::~Tokenizer() {
    // TODO Auto-generated destructor stub
}

int Tokenizer::getPos() const {
    return p_pos;
}

void Tokenizer::setPos(int pos) {
    p_pos = pos;
}

Status Tokenizer::openFile(const char* fileName) {
    fr.setFile(fileName);
    return fr.openFile();
}

void Tokenizer::closeFile() {
    fr.closeFile();
}

Status Tokenizer::nextLine() {
    Status status = SUCCESS;
    memset(p_line, 0, 256);
    p_pos = 0;
    fr.getLine(p_line);
    if(p_line[0] == '\0') return FAILURE;
    if(p_line[0] == '/' && p_line[1] == '/') {
        memset(p_line, 0, 256);
        fr.getLine(p_line);
    }
    return status;
}

static bool isDelimiter(char c, const char delim[], int numDelim) {
    for(size_t i=0; i<numDelim; i++) {
        if(c == delim[i])
            return true;
    }
    return false;
}

char Tokenizer::nextChar(bool peek) {
    int p = p_pos;
    while(p_line[p] == ' ') {
        p++;
    }
    char next = p_line[p];
    if(!peek && next != '\0')
        p_pos = p+1;
    return next;
}

int Tokenizer::readNextWordLen(int& begin) {
    int p = begin > p_pos ? begin : p_pos;
    int wordLen =0;
    if(p_line[p] == '\0') {
        return 0;
    }
    while(p_line[p] == ' ') {
        begin++;
        p++;
    }
    if(isDelimiter(p_line[p], delimiters, sizeof(delimiters))) {
        wordLen++;
    }
    else {
        while( p_line[p] != ' ' && p_line[p] != '\0' && !isDelimiter(p_line[p], delimiters, sizeof(delimiters))) {
            wordLen++;
            p++;
        }
    }
    return wordLen;
}

void Tokenizer::consumeWord() {
    int begin = p_pos;
    int wordLen = readNextWordLen(begin);
    p_pos = begin + wordLen;
    //cout<<"consumed word " <<wordLen <<" " <<p_pos <<endl;
}

char* Tokenizer::nextWord(bool peek) {
    int begin = p_pos;
    int wordLen = readNextWordLen(begin);
    if(wordLen == 0) return nullptr;
    char* token = new char[wordLen+1];
    strncpy(token, p_line + begin, wordLen);
    token[wordLen] = '\0';

    if(!peek) {
        p_pos = begin + wordLen;
        cout<<"token " <<token <<" p_pos " <<p_pos<<" wordLen " <<wordLen <<endl;
    }
    return token;
}

char* Tokenizer::nextWordUntil(char delim[], int numDelim) {
    int p = p_pos;
    int wordLen = 0;
    while(p_line[p] == ' ') {
        p++;
        p_pos++;
    }
    while( p_line[p] != '\0' && !isDelimiter(p_line[p], delim, numDelim)) {
        wordLen++;
        p++;
    }
    if(wordLen == 0) return nullptr;
    char* token = new char[wordLen+1];
    strncpy(token, p_line + p_pos, wordLen);
    token[wordLen] = '\0';
    p_pos += wordLen;
    return token;
}

char Tokenizer::lookAhead(int num) {
    int begin=p_pos;
    int wordLen = 0;
    while(num) {
        begin += wordLen;
        wordLen = readNextWordLen(begin);
        if(wordLen==0) return '\0';
        num--;
    }
    return p_line[begin];
}
