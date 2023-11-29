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

int main(int argc, char* argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	if(argc==0) return 0;
	char* fileName = argv[1];
	InstrParser instrParser;
	Status s = instrParser.parseFile(fileName);

	if (s == FAILURE ) {
		cout <<"Instructions file parsing failed " <<endl;
		return 0;
	}
	cout << "********************************** Instructions file parsing done ****************************************" <<endl;
	ControlFlowGraph cfg;
	s = cfg.buildCFG(instrParser.getBlock());

	if (s == FAILURE )
		cout <<"Building Control flow graph failed " <<endl;

	cout <<"********************************** Build cfg done **********************************" <<endl;
	cout <<cfg <<endl;
	cout << "********************************** output cfg done ****************************************" <<endl;
	cfg.variableInitCheck();
	cout << "********************************** Variable Init Check done ****************************************" <<endl;
	cfg.clear();
	cout << "********************************** CFG Clear done ****************************************" <<endl;
	instrParser.clear();
	cout << "********************************** InstrParser Clear done ****************************************" <<endl;
	return 0;
}
