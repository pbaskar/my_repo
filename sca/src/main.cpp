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
#include "ControlFlowGraph.h"
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

    char fileName[] = "C:\\workspace\\sca\\invalid_instructions.txt";
	InstrParser instrParser;
	Status s = instrParser.parseFile(fileName);

	if (s == FAILURE )
		cout <<"Instructions file parsing failed " <<endl;

	ControlFlowGraph cfg;
	s = cfg.buildCFG(instrParser.getStatementList());

	if (s == FAILURE )
		cout <<"Building Control flow graph failed " <<endl;

	cout <<"Building Control flow graph succeeded " <<endl;
	return 0;
}
