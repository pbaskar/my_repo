/*
 * Visitor.cpp
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#include "Visitor.h"
#include "ControlFlowGraph.h"

Visitor::Visitor() {
	// TODO Auto-generated constructor stub

}

Visitor::~Visitor() {
	// TODO Auto-generated destructor stub
}

PrintVisitor::PrintVisitor() {

}

PrintVisitor::~PrintVisitor() {

}

void PrintVisitor::visitBasicBlock(BasicBlock* basicBlock) {
	basicBlock->print();
}

void PrintVisitor::visitIfElseBlock(BasicBlock* ifElseBlock) {
	ifElseBlock->print();
}

void PrintVisitor::visitWhileBlock(BasicBlock* whileBlock) {
	whileBlock->print();
}
