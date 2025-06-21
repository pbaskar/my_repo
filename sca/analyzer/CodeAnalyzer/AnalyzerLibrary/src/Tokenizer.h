/*
 * Tokenizer.h
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_
#include "FileReader.h"

class Tokenizer {
public:
    Tokenizer();
    virtual ~Tokenizer();

    int readNextWordLen(int& begin);
    void consumeWord();
    char* nextWord(bool peek=false);
    char* nextWordUntil(char delim[], int numDelim);
    char lookAhead(int num);
    char nextChar(bool peek=false);
    Status nextLine();
    Status openFile(const char* fileName);
    void closeFile();
    void setPos(int pos);
    int getPos() const;

private:
    char p_line[256];
    int p_pos;

    FileReader fr;
};

#endif /* TOKENIZER_H_ */
