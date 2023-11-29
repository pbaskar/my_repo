/*
 * FileReader.cpp
 *
 *  Created on: Sep 20, 2023
 *      Author: prbas_000
 */
#include<cstring>
#include<iostream>
#include "FileReader.h"

FileReader::FileReader() {
	// TODO Auto-generated constructor stub

}

FileReader::~FileReader() {
	// TODO Auto-generated destructor stub
}

void FileReader::setFile(char* fileName) {
	strcpy(p_fileName, fileName);
}

Status FileReader::openFile() {
	Status status = SUCCESS;
	p_is.open(p_fileName);
	if(!p_is.is_open()) {
		status = FAILURE;
		cout <<"File not opened " << p_fileName <<endl;
	}
	else
		cout <<"File opened " << p_fileName <<endl;
	return status;
}

void FileReader::closeFile() {
	p_is.close();
}

void FileReader::getLine(char* line) {
	p_is.getline(line, 256);
}

