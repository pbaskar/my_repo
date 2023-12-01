/*
 * Analyzer.cpp
 *
 *  Created on: Nov 29, 2023
 *      Author: prbas_000
 */

#include <iostream>
#include <fstream>
#include "InstrParser.h"
#include "ControlFlowGraph.h"
using namespace std;
#include "analyzer.h"

Analyzer::Analyzer() {
	// TODO Auto-generated constructor stub

}

Analyzer::~Analyzer() {
	// TODO Auto-generated destructor stub
}

int Analyzer::execute(const char* fileName, std::vector<Result>& results) {
    //char* fileName = "C:\\workspace\\my_repo\\sca\\test\\instructions.c";
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
    cfg.variableInitCheck(results);
	cout << "********************************** Variable Init Check done ****************************************" <<endl;
    cfg.clear();
	cout << "********************************** CFG Clear done ****************************************" <<endl;
    instrParser.clear();
    cout << "********************************** InstrParser Clear done ****************************************" <<endl;
	return 0;
}
