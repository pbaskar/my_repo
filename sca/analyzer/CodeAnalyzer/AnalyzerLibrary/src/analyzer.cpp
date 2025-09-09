/*
 * Analyzer.cpp
 *
 *  Created on: Nov 29, 2023
 *      Author: prbas_000
 */

#include <iostream>
#include <fstream>
#include "InstrParser.h"
#include "ExprSimplifier.h"
#include "ControlFlowGraph.h"
using namespace std;
#include "analyzer.h"

Analyzer::Analyzer() {
    // TODO Auto-generated constructor stub

}

Analyzer::~Analyzer() {
    // TODO Auto-generated destructor stub
}

Status Analyzer::getCFG(const char* fileName, BasicBlock*& cfgHead) {
    InstrParser instrParser;
    Status s = instrParser.parseFile(fileName);

    if (s == FAILURE ) {
        cout <<"Instructions file parsing failed " <<endl;
        return s;
    }
    cout << "********************************** Instructions file parsing done ****************************************" <<endl;
    ControlFlowGraph cfg;
    s = cfg.buildCFG(instrParser.getBlock());

    if (s == FAILURE ) {
        cout <<"Building Control flow graph failed " <<endl;
        return s;
    }
    cout <<"********************************** Build cfg done **********************************" <<endl;
    cout <<cfg <<endl;
    cout << "********************************** output cfg done ****************************************" <<endl;
    cfgHead = cfg.getHead();
    return s;
}

Status Analyzer::execute(const char* fileName, std::vector<Result>& results) {
    //char* fileName = "C:\\workspace\\my_repo\\sca\\test\\instructions.c";
    InstrParser instrParser;
    Status s = instrParser.parseFile(fileName);

    if (s == FAILURE ) {
        cout <<"Instructions file parsing failed " <<endl;
        return s;
    }
    cout << "********************************** Instructions file parsing done ****************************************" <<endl;

    ExprSimplifier exprSimplifier;
    s = exprSimplifier.simplify(instrParser.getBlock());

    if (s == FAILURE ) {
        cout <<"Simplify expressions failed " <<endl;
        return s;
    }

    cout << "********************************** Expression Simplification done ****************************************" <<endl;

    ControlFlowGraph cfg;
    s = cfg.buildCFG(instrParser.getBlock());

    if (s == FAILURE ) {
        cout <<"Building Control flow graph failed " <<endl;
        return s;
    }

    cout <<"********************************** Build cfg done **********************************" <<endl;
    cout <<cfg <<endl;
    cout << "********************************** output cfg done ****************************************" <<endl;
    cfg.variableInitCheck(results);
    cout << "********************************** Variable Init Check done ****************************************" <<endl;
    ofstream of("output.log");
    if(of.is_open()) {
        for(Result r : results) {
            of <<r.errorMessage <<endl;
            delete(r.errorMessage);
        }
        of.close();
    }
    cout << "********************************** Output result done ****************************************" <<endl;
    cfg.clear();
    cout << "********************************** CFG Clear done ****************************************" <<endl;
    instrParser.clear();
    cout << "********************************** InstrParser Clear done ****************************************" <<endl;
    cout.flush();
    return s;
}
