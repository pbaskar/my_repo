/*
 * Traverser.cpp
 *
 *  Created on: Oct 6, 2023
 *      Author: prbas_000
 */

#include "Traverser.h"
#include "BasicBlock.h"
#include "Visitor.h"

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
	BasicBlock* block = ifElseBlock->getIfFirst();
	BasicBlock* lastBlock = ifElseBlock->getIfLast();
	BasicBlock* next(0);
	while(block != lastBlock) {
		next = block->getNext();
		block->acceptTraverser(*this);
		block = next;
	}
	lastBlock->acceptTraverser(*this);
	block = ifElseBlock->getElseFirst();
	lastBlock = ifElseBlock->getElseLast();
	while(block != lastBlock) {
		next = block->getNext();
		block->acceptTraverser(*this);
		block = next;
	}
	if(block) {
		block->acceptTraverser(*this);
	}
	ifElseBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseWhileBlock(WhileBlock* whileBlock) {
	BasicBlock* block = whileBlock->getFirst();
	BasicBlock* lastBlock = whileBlock->getLast();
	BasicBlock* next(0);

	while(block != lastBlock) {
		next = block->getNext();
		block->acceptTraverser(*this);
		block = next;
	}
	block->acceptTraverser(*this);
	whileBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
	BasicBlock* block = functionDeclBlock->getFirst();
	BasicBlock* lastBlock = functionDeclBlock->getLast();
	BasicBlock* next(0);

	while(block != lastBlock) {
		next = block->getNext();
		block->acceptTraverser(*this);
		block = next;
	}
	block->acceptTraverser(*this);
	functionDeclBlock->acceptVisitor(*p_visitor);
}

void TraverserOne::traverseCFG(BasicBlock* block) {
	BasicBlock* next(0);
	while(block) {
		next = block->getNext();
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
	BasicBlock* block = basicBlock->getNext();
	if(block) block->acceptTraverser(*this);
}

void TraverserAllPath::traverseIfElseBlock(IfElseBlock* ifElseBlock) {
	ifElseBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = ifElseBlock->getIfFirst();
	block->acceptTraverser(*this);

	block = ifElseBlock->getElseFirst();
	if(block)
		block->acceptTraverser(*this);
	else {
		BasicBlock* next = ifElseBlock->getNext();
		if(next) next->acceptTraverser(*this);
	}
}

void TraverserAllPath::traverseWhileBlock(WhileBlock* whileBlock) {
	whileBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = whileBlock->getFirst();
	block->acceptTraverser(*this);

	BasicBlock* next = whileBlock->getNext();
	if(next) next->acceptTraverser(*this);
}

void TraverserAllPath::traverseFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
	/*whileBlock->acceptVisitor(*p_visitor);
	BasicBlock* block = whileBlock->getFirst();
	block->acceptTraverser(*this);

	BasicBlock* next = whileBlock->getNext();
	if(next) next->acceptTraverser(*this);*/
}

void TraverserAllPath::traverseCFG(BasicBlock* block) {
	if(block)
		block->acceptTraverser(*this);
}
