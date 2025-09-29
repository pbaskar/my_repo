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
#include <ctime>
using namespace std;
#include "analyzer.h"
#include "Logger.h"

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
        Logger::getDebugStreamInstance() <<"Instructions file parsing failed " <<endl;
        return s;
    }
    Logger::getDebugStreamInstance() << "********************************** Instructions file parsing done ****************************************" <<endl;
    ControlFlowGraph cfg;
    s = cfg.buildCFG(instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Building Control flow graph failed " <<endl;
        return s;
    }
    Logger::getDebugStreamInstance() <<"********************************** Build cfg done **********************************" <<endl;
    Logger::getDebugStreamInstance() <<cfg <<endl;
    Logger::getDebugStreamInstance() << "********************************** output cfg done ****************************************" <<endl;
    cfgHead = cfg.getHead();
    return s;
}

Status Analyzer::execute(const char* inputFile, const char* outputFile, std::vector<Result>& results) {
    Logger::getDebugStreamInstance().open("debug.log");
    Logger::getDebugStreamInstance() << endl << "Logging... " <<inputFile <<endl;
    InstrParser instrParser;
    Status s = instrParser.parseFile(inputFile);

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Instructions file parsing failed " <<endl;
        return s;
    }
    Logger::getDebugStreamInstance() << "********************************** Instructions file parsing done ****************************************" <<endl;

    ExprSimplifier exprSimplifier;
    s = exprSimplifier.simplify(instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Simplify expressions failed " <<endl;
        return s;
    }

    Logger::getDebugStreamInstance() << "********************************** Expression Simplification done ****************************************" <<endl;

    ControlFlowGraph cfg;
    s = cfg.buildCFG(instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Building Control flow graph failed " <<endl;
        return s;
    }

    Logger::getDebugStreamInstance() <<"********************************** Build cfg done **********************************" <<endl;
    Logger::getDebugStreamInstance() <<cfg <<endl;
    Logger::getDebugStreamInstance() << "********************************** output cfg done ****************************************" <<endl;
    cfg.variableInitCheck(results);
    Logger::getDebugStreamInstance() << "********************************** Variable Init Check done ****************************************" <<endl;
    ofstream of(outputFile, ios::app);
    if(of.is_open()) {
        std::time_t currTime = std::time(nullptr);
        of<< endl << "Running... " << inputFile << ", timestamp: " << std::asctime(std::localtime(&currTime)) << endl;
        for(Result r : results) {
            of <<r.errorMessage <<endl;
            delete(r.errorMessage);
        }
        of << "Finished: status = " << s << std::endl;
        of.close();
    }
    Logger::getDebugStreamInstance() << "********************************** Output result done ****************************************" <<endl;
    cfg.clear();
    Logger::getDebugStreamInstance() << "********************************** CFG Clear done ****************************************" <<endl;
    instrParser.clear();
    Logger::getDebugStreamInstance() << "********************************** InstrParser Clear done ****************************************" <<endl;
    Logger::getDebugStreamInstance().flush();
    Logger::getDebugStreamInstance().close();
#ifdef NDEBUG
    std::remove("debug.log");
#endif
    return s;
}
