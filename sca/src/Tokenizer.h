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

	int readNextWordLen();
	void consumeWord();
	char* nextWord(bool peek=false);
	char nextChar(bool peek=false);
	Status nextLine();
	Status openFile(char* fileName);
	void closeFile();

private:
	char p_line[256];
    int p_pos;

    FileReader fr;
};

#endif /* TOKENIZER_H_ */
