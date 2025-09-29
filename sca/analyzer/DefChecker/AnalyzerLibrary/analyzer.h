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

class Analyzer {
public:
    Analyzer();
	virtual ~Analyzer();

    Status execute(const char* inputFile, const char* outputFile, std::vector<Result>& results);
    Status getCFG(const char* fileName, BasicBlock*& cfg);
};

#endif /* SRC_ANALYZER_H_ */
