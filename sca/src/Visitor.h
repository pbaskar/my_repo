/*
 * Visitor.h
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#ifndef SRC_VISITOR_H_
#define SRC_VISITOR_H_
#include<vector>
using namespace std;
class ControlFlowGraph;
class BasicBlock;
class IfElseBlock;
class WhileBlock;
class Node;
class Visitor {
public:
	Visitor();
	virtual ~Visitor();
	virtual void visitBasicBlock(BasicBlock* basicBlock)=0;
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock)=0;
	virtual void visitWhileBlock(WhileBlock* whileBlock)=0;
};

class PrintVisitor : public Visitor {
public:
	PrintVisitor();
	virtual ~PrintVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
};

class DeleteVisitor : public Visitor {
public:
	DeleteVisitor();
	virtual ~DeleteVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
};

class VariableInitCheckVisitor : public Visitor {
public:
	VariableInitCheckVisitor();
	virtual ~VariableInitCheckVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
private:
	vector<Node*> p_variableNodes;
};
#endif /* SRC_VISITOR_H_ */
