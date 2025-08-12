/*
 * FileReader.h
 *
 *  Created on: Sep 20, 2023
 *      Author: prbas_000
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_
#include<fstream>
#include "Utils.h"

using namespace std;

class FileReader {
public:
    FileReader();
    virtual ~FileReader();

    void setFile(const char* fileName);
    Status openFile();
    void closeFile();
    Status getLine(char* line);
private:
    ifstream p_is;
    char p_fileName[50];
};

#endif /* FILEREADER_H_ */
