/*
 * Visitor.h
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#ifndef SRC_VISITOR_H_
#define SRC_VISITOR_H_
#include<vector>
#include "Results.h"
using namespace std;

class BasicBlock;
class IfElseBlock;
class WhileBlock;
class FunctionDeclBlock;
class FunctionCallBlock;
class Node;
class Visitor {
public:
	Visitor();
	virtual ~Visitor();
	virtual void visitBasicBlock(BasicBlock* basicBlock)=0;
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock)=0;
	virtual void visitWhileBlock(WhileBlock* whileBlock)=0;
	virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock)=0;
	virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock)=0;
};

class PrintVisitor : public Visitor {
public:
	PrintVisitor();
	virtual ~PrintVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
	virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
	virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
};

class DeleteVisitor : public Visitor {
public:
	DeleteVisitor();
	virtual ~DeleteVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
	virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
	virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
};

class VariableInitCheckVisitor : public Visitor {
public:
	VariableInitCheckVisitor();
	virtual ~VariableInitCheckVisitor();

	virtual void visitBasicBlock(BasicBlock* basicBlock);
	virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
	virtual void visitWhileBlock(WhileBlock* whileBlock);
	virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
	virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);

    const vector<Result>& getResults() { return p_results; }
private:
	vector<Node*> p_variableNodes;
    vector<Result> p_results;
};
#endif /* SRC_VISITOR_H_ */
