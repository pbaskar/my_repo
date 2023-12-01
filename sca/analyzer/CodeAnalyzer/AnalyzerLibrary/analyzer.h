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

class Analyzer {
public:
    Analyzer();
	virtual ~Analyzer();

    int execute(const char* fileName, std::vector<Result>& results);
};

#endif /* SRC_ANALYZER_H_ */
