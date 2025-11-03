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
#include <cstdio>
using namespace std;
#include "analyzer.h"
#include "Logger.h"

static const char* usageMsg = "Usage: DefChecker.exe -i <inputfile> -o <outputpath> \nMore info is available at https://github.com/pbaskar/my_repo/tree/defchecker_release_version_1_0_0/sca/doc";

Analyzer::Analyzer() {
    // TODO Auto-generated constructor stub

}

Analyzer::~Analyzer() {
    // TODO Auto-generated destructor stub
}

Status Analyzer::fetchErrors(std::vector<Result>& results) {
    const vector<char*>& errors = Logger::getInstance()->getErrors();
    for (int i = 0; i < errors.size(); i++) {
        Result r;
        r.errorMessage = errors[i];
        results.push_back(r);
    }
    Logger::getInstance()->clearErrors();
    Logger::getDebugStreamInstance() << results.size() << endl;
    return SUCCESS;
}

Status Analyzer::getCFG(const char* fileName, BasicBlock*& cfgHead, vector<Result>& results) {
    Status s = p_instrParser.parseFile(fileName);

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Instructions file parsing failed " <<endl;
        fetchErrors(results);
        return s;
    }
    Logger::getDebugStreamInstance() << "********************************** Instructions file parsing done ****************************************" <<endl;

    ExprSimplifier exprSimplifier;
    s = exprSimplifier.simplify(p_instrParser.getBlock());
    if (s == FAILURE) {
        Logger::getDebugStreamInstance() << "Simplify expressions failed " << endl;
        fetchErrors(results);
        return s;
    }
    Logger::getDebugStreamInstance() << "********************************** Expression Simplification done ****************************************" << endl;

    s = p_cfg.buildCFG(p_instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Building Control flow graph failed " <<endl;
        fetchErrors(results);
        return s;
    }
    Logger::getDebugStreamInstance() <<"********************************** Build cfg done **********************************" <<endl;
    Logger::getDebugStreamInstance() << p_cfg <<endl;
    Logger::getDebugStreamInstance() << "********************************** output cfg done ****************************************" <<endl;
    cfgHead = p_cfg.getHead();
    Logger::getDebugStreamInstance().flush();
    Logger::getDebugStreamInstance().clear();
    //Logger::getDebugStreamInstance().close();
    return s;
}

Status Analyzer::setOutputPath(const char* outputFilePath) {
    /*const char* errorFile = Utils::makeWord(outputFilePath, "\\error.log");
    Status s = Logger::getInstance()->setErrorFile(errorFile);
    if (s == FAILURE) {
        cout << usageMsg << endl;
    }
    delete errorFile;*/
    return SUCCESS;
}

Status Analyzer::execute(const char* inputFile, const char* outputFilePath, std::vector<Result>& results) {
    /*const char* debugFile = Utils::makeWord(outputFilePath, "\\debug.log");
    Logger::getDebugStreamInstance().open(debugFile);
    if (!Logger::getDebugStreamInstance().is_open()) {
        cout << "Could not open debug log " << debugFile << endl;
        cout << usageMsg << endl;
        return FAILURE;
    }
    Logger::getDebugStreamInstance() << endl << "Logging... " <<inputFile <<endl;*/
    Status s = p_instrParser.parseFile(inputFile);

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Instructions file parsing failed " <<endl;
        fetchErrors(results);
        return s;
    }
    Logger::getDebugStreamInstance() << "********************************** Instructions file parsing done ****************************************" <<endl;

    ExprSimplifier exprSimplifier;
    s = exprSimplifier.simplify(p_instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Simplify expressions failed " <<endl;
        fetchErrors(results);
        return s;
    }

    Logger::getDebugStreamInstance() << "********************************** Expression Simplification done ****************************************" <<endl;

    s = p_cfg.buildCFG(p_instrParser.getBlock());

    if (s == FAILURE ) {
        Logger::getDebugStreamInstance() <<"Building Control flow graph failed " <<endl;
        fetchErrors(results);
        return s;
    }

    Logger::getDebugStreamInstance() <<"********************************** Build cfg done **********************************" <<endl;
    Logger::getDebugStreamInstance() << p_cfg <<endl;
    Logger::getDebugStreamInstance() << "********************************** output cfg done ****************************************" <<endl;
    p_cfg.variableInitCheck(results);
    Logger::getDebugStreamInstance() << "********************************** Variable Init Check done ****************************************" <<endl;
    /*const char* outputFile = Utils::makeWord(outputFilePath, "\\output.log");
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
    else {
        cout << usageMsg << endl;
        cout << "could not open output log " << outputFile << endl;
    }
    delete outputFile;*/
    Logger::getDebugStreamInstance() << "********************************** Output result done ****************************************" <<endl;
    p_cfg.clear();
    Logger::getDebugStreamInstance() << "********************************** CFG Clear done ****************************************" <<endl;
    p_instrParser.clear();
    Logger::getDebugStreamInstance() << "********************************** InstrParser Clear done ****************************************" <<endl;
    Logger::getDebugStreamInstance().flush();
    Logger::getDebugStreamInstance().clear();
    //Logger::getDebugStreamInstance().close();
//#ifdef NDEBUG
    //std::remove(debugFile);
    //delete debugFile;
//#endif
    return s;
}

Status Analyzer::clear() {
    p_cfg.clear();
    p_instrParser.clear();
    return SUCCESS;
}
