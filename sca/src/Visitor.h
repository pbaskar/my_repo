/*
 * Visitor.h
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#ifndef SRC_VISITOR_H_
#define SRC_VISITOR_H_
class ControlFlowGraph;
class BasicBlock;
class Visitor {
public:
	Visitor();
	virtual ~Visitor();
	virtual void visitBasicBlock(BasicBlock* basicBlock)=0;
	virtual void visitIfElseBlock(BasicBlock* ifElseBlock)=0;
	virtual void visitWhileBlock(BasicBlock* whileBlock)=0;
};

class PrintVisitor : public Visitor {
public:
	PrintVisitor();
	virtual ~PrintVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(BasicBlock* ifElseBlock);
	virtual void visitWhileBlock(BasicBlock* whileBlock);
};

#endif /* SRC_VISITOR_H_ */
