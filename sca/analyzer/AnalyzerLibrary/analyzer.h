/*
 * Analyzer.h
 *
 *  Created on: Nov 29, 2023
 *      Author: prbas_000
 */

#ifndef SRC_ANALYZER_H_
#define SRC_ANALYZER_H_

#include<vector>
#include "src/Results.h"
#include "src/BasicBlock.h"
#include "src/Visitor.h"
#include "src/InstrParser.h"
#include "src/ControlFlowGraph.h"

class Analyzer {
public:
    Analyzer();
	virtual ~Analyzer();

    Status openErrorFile(const char* errorFilePath);
    Status closeErrorFile();
    Status execute(const char* inputFile, const char* outputFilePath, std::vector<Result>& results);
    Status getCFG(const char* fileName, BasicBlock*& cfg, const char* outputFilePath, vector<Result>& results);
    Status fetchErrors(std::vector<Result>& results);
    Status clear();
private:
    InstrParser p_instrParser;
    ControlFlowGraph p_cfg;
};

#endif /* SRC_ANALYZER_H_ */
