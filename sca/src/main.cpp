//============================================================================
// Name        : helloworld.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include "InstrParser.h"
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

    char fileName[] = "C:\\workspace\\parser\\invalid_instructions.txt";
	InstrParser instrParser;
	Status s = instrParser.parseFile(fileName);
	if (s == FAILURE )
		cout <<"Instructions file parsing failed " <<endl;
	else
		cout <<"Instructions file parsing succeeded " <<endl;
	return 0;
}
