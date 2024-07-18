/*
 * ControlFlowGraph.h
 *
 *  Created on: Sep 26, 2023
 *      Author: prbas_000
 */

#ifndef SRC_CONTROLFLOWGRAPH_H_
#define SRC_CONTROLFLOWGRAPH_H_

#include<vector>
#include<iostream>
#include "Stmt.h"
#include "Utils.h"
#include "BasicBlock.h"
#include "Traverser.h"
#include "Visitor.h"

using namespace std;

class ControlFlowGraph {
public:
    ControlFlowGraph();
    virtual ~ControlFlowGraph();
    friend ostream& operator<<(ostream& os, ControlFlowGraph& cfg) {
        cfg.print(os);
        return os;
    }
    void print(ostream& os);
    void variableInitCheck(vector<Result>& results);
    void clear();
    BasicBlock* getHead() { return p_head; }
    Status buildCFG(const Block* block);
    Status buildBlock(BasicBlock*& currBlock, const Block* block);
private:
    BasicBlock* p_head;
};

#endif /* SRC_CONTROLFLOWGRAPH_H_ */
