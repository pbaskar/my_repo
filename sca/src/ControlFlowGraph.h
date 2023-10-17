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
#include "InstrParser.h"
#include "Traverser.h"
using namespace std;

enum NodeType {
	ASSIGNMENT,
	CONDITION
};

class Node {
public:
	Node(): p_next(0) {}
	virtual ~Node(){
	}
	virtual NodeType type()=0;
	virtual void print(ostream& os) const=0;
	friend ostream& operator<<(ostream& os, const Node& node) {
		node.print(os);
		return os;
	}
private:
	Node* p_next;
};

class AssignmentNode : public Node {
public:
	AssignmentNode() : p_var(0), p_value(0) { }
	AssignmentNode(AssignStmt& stmt) {
		p_var = stmt.p_var;
		p_value = stmt.p_value;
	}
	virtual NodeType type() { return ASSIGNMENT; }
	virtual ~AssignmentNode() {
		//delete p_var;
		delete p_value;
	}
	virtual void print(ostream& os) const {
		os << "name " << *p_var;
		if(p_value)
			os <<" value " <<*p_value <<" ";
	}
	bool operator==(const AssignmentNode& other) {
		return p_var == other.p_var;
	}
	const Variable* getVariable() { return p_var; }
	const Expr* getValue() { return p_value; }

private:
	Variable* p_var;
	Expr* p_value;
};

class ConditionNode : public Node {
public:
	ConditionNode() : p_condition(0) { }
	ConditionNode(IfStmt& stmt) : p_condition(stmt.p_condition) {
	}
	ConditionNode(WhileStmt& stmt) : p_condition(stmt.p_condition) {
	}
	NodeType type() { return CONDITION; }
	virtual ~ConditionNode() {
		delete p_condition;
	}
	virtual void print(ostream& os) const {
		os <<"condition "<<" ";
		if(p_condition)
			os <<*p_condition <<" ";
	}
	void setCondition(Expr* condition) { p_condition = condition; }
private:
	Expr* p_condition;
};

class BasicBlock {
public:
	BasicBlock(const SymbolTable* symbolTable) : p_next(0), p_symbolTable(symbolTable) {}
	BasicBlock(BasicBlock* next, SymbolTable* symbolTable): p_next(next), p_symbolTable(symbolTable){}
	virtual ~BasicBlock() {
		for(Node* n : nodeList) {
			delete n;
		}
		delete p_symbolTable;
	}
	void print() {
		for(Node* n: nodeList) {
			cout <<*n <<" ";
		}
		cout<<endl;
	}
	virtual void setNext(BasicBlock* next) {
		p_next = next;
	}
	void addNode(Node* newNode) {
		nodeList.push_back(newNode);
	}
	virtual void acceptVisitor(Visitor& visitor) {
		visitor.visitBasicBlock(this);
	}
	virtual void acceptTraverser(Traverser& traverser) {
		traverser.traverseBasicBlock(this);
	}
	friend TraverserOne;
	friend TraverserAllPath;
	friend VariableInitCheckVisitor;
private:
	vector<Node*> nodeList;
	BasicBlock* p_next;
	const SymbolTable* p_symbolTable;
};

class IfElseBlock : public BasicBlock {
public:
	IfElseBlock(BasicBlock* next, BasicBlock* ifFirst, BasicBlock* ifLast, BasicBlock* elseFirst, BasicBlock* elseLast)
: BasicBlock(next,0), p_ifFirst(ifFirst), p_ifLast(ifLast), p_elseFirst(elseFirst), p_elseLast(elseLast) {}
	~IfElseBlock() {
	}
	virtual void setNext(BasicBlock* next) {
		p_ifLast->setNext(next);
		if(p_elseLast)
			p_elseLast->setNext(next);
		BasicBlock::setNext(next);
	}
	virtual void acceptVisitor(Visitor& visitor) {
		visitor.visitIfElseBlock(this);
	}
	virtual void acceptTraverser(Traverser& traverser) {
		traverser.traverseIfElseBlock(this);
	}
	friend TraverserOne;
	friend TraverserAllPath;
	friend VariableInitCheckVisitor;
private:
	BasicBlock* p_ifFirst;
	BasicBlock* p_ifLast;
	BasicBlock* p_elseFirst;
	BasicBlock* p_elseLast;
};

class WhileBlock : public BasicBlock {
public:
	WhileBlock(BasicBlock* next, BasicBlock* first, BasicBlock* last):
		BasicBlock(next, 0), p_first(first), p_last(last) {}
	~WhileBlock() {
	}
	virtual void setNext(BasicBlock* next) {
		BasicBlock::setNext(next);
	}
	void setSelf() {
		p_last->setNext(this);
	}
	virtual void acceptVisitor(Visitor& visitor) {
		visitor.visitWhileBlock(this);
	}
	virtual void acceptTraverser(Traverser& traverser) {
		traverser.traverseWhileBlock(this);
	}
	friend TraverserOne;
	friend TraverserAllPath;
	friend VariableInitCheckVisitor;
private:
	BasicBlock* p_first;
	BasicBlock* p_last;
};

class ControlFlowGraph {
public:
	ControlFlowGraph();
	virtual ~ControlFlowGraph();
	friend ostream& operator<<(ostream& os, ControlFlowGraph& cfg) {
		cfg.print(os);
		return os;
	}
	void print(ostream& os);
	void variableInitCheck();
	void clear();
	Status buildCFG(const Block* block);
	Status buildBlock(BasicBlock*& currBlock, const Block* block);
	friend TraverserOne;
	friend TraverserAllPath;
private:
	BasicBlock* head;
};

#endif /* SRC_CONTROLFLOWGRAPH_H_ */
