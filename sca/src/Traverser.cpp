/*
 * Traverser.cpp
 *
 *  Created on: Oct 6, 2023
 *      Author: prbas_000
 */

#include "Traverser.h"
#include "Visitor.h"
#include "ControlFlowGraph.h"

Traverser::Traverser(Visitor* visitor) : p_visitor(visitor) {
	// TODO Auto-generated constructor stub
}

Traverser::~Traverser() {
	// TODO Auto-generated destructor stub
}

TraverserOne::TraverserOne(Visitor* visitor): Traverser(visitor) {
	// TODO Auto-generated constructor stub

}

TraverserOne::~TraverserOne() {
	// TODO Auto-generated destructor stub
}

void TraverserOne::traverseBasicBlock(BasicBlock* basicBlock) {
	basicBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseIfElseBlock(IfElseBlock* ifElseBlock) {
	BasicBlock* block = ifElseBlock->p_ifFirst;
	BasicBlock* lastBlock = ifElseBlock->p_ifLast;
	BasicBlock* next(0);
	while(block != lastBlock) {
		next = block->p_next;
		block->acceptTraverser(*this);
		block = next;
	}
	lastBlock->acceptTraverser(*this);
	block = ifElseBlock->p_elseFirst;
	lastBlock = ifElseBlock->p_elseLast;
	while(block != lastBlock) {
		next = block->p_next;
		block->acceptTraverser(*this);
		block = next;
	}
	if(block) {
		block->acceptTraverser(*this);
	}
	ifElseBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseWhileBlock(WhileBlock* whileBlock) {
	BasicBlock* block = whileBlock->p_first;
	BasicBlock* lastBlock = whileBlock->p_last;
	BasicBlock* next(0);

	while(block != lastBlock) {
		next = block->p_next;
		block->acceptTraverser(*this);
		block = next;
	}
	block->acceptTraverser(*this);
	whileBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseCFG(ControlFlowGraph* cfg) {
	BasicBlock* block = cfg->head;
	BasicBlock* next(0);
	while(block) {
		next = block->p_next;
		block->acceptTraverser(*this);
		block = next;
	}
}

TraverserAllPath::TraverserAllPath(Visitor* visitor): Traverser(visitor) {
	// TODO Auto-generated constructor stub

}

TraverserAllPath::~TraverserAllPath() {
	// TODO Auto-generated destructor stub
}

void TraverserAllPath::traverseBasicBlock(BasicBlock* basicBlock) {
	basicBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = basicBlock->p_next;
	if(block) block->acceptTraverser(*this);
}

void TraverserAllPath::traverseIfElseBlock(IfElseBlock* ifElseBlock) {
	ifElseBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = ifElseBlock->p_ifFirst;
	block->acceptTraverser(*this);

	block = ifElseBlock->p_elseFirst;
	if(block)
		block->acceptTraverser(*this);
	else {
		BasicBlock* next = ifElseBlock->p_next;
		if(next) next->acceptTraverser(*this);
	}
}

void TraverserAllPath::traverseWhileBlock(WhileBlock* whileBlock) {
	whileBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = whileBlock->p_first;
	block->acceptTraverser(*this);

	BasicBlock* next = whileBlock->p_next;
	if(next) next->acceptTraverser(*this);
}

void TraverserAllPath::traverseCFG(ControlFlowGraph* cfg) {
	BasicBlock* block = cfg->head;
	if(block)
		block->acceptTraverser(*this);
}
