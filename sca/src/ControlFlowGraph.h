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
	virtual void print(ostream& os)=0;
	friend ostream& operator<<(ostream& os, Node& node) {
		node.print(os);
		return os;
	}
private:
	Node* p_next;
};

class AssignmentNode : public Node {
public:
	AssignmentNode() : p_name(0), p_value(0) { }
	AssignmentNode(AssignStmt& stmt) {
		p_name = stmt.p_name;
		p_value = stmt.p_value;
	}
	virtual NodeType type() { return ASSIGNMENT; }
	virtual ~AssignmentNode() {
		delete p_name;
		delete p_value;
	}
	virtual void print(ostream& os) {
		os << "name " << p_name <<" value " <<*p_value <<" ";
	}
	void setName(char* name) { p_name = name; }
	void setValue(Expr* value) { p_value = value; }
private:
	char* p_name;
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
	virtual void print(ostream& os) {
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
	BasicBlock(): p_next(0) {}
	BasicBlock(BasicBlock* next): p_next(next){}
	virtual ~BasicBlock() {
		for(Node* n : nodeList) {
			delete n;
		}
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
private:
	vector<Node*> nodeList;
	BasicBlock* p_next;
};

class IfElseBlock : public BasicBlock {
public:
	IfElseBlock(): p_ifFirst(0), p_ifLast(0), p_elseFirst(0), p_elseLast(0) {}
	IfElseBlock(BasicBlock* next, BasicBlock* ifFirst, BasicBlock* ifLast, BasicBlock* elseFirst, BasicBlock* elseLast)
	: BasicBlock(next), p_ifFirst(ifFirst), p_ifLast(ifLast), p_elseFirst(elseFirst), p_elseLast(elseLast) {}
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
private:
	BasicBlock* p_ifFirst;
	BasicBlock* p_ifLast;
	BasicBlock* p_elseFirst;
	BasicBlock* p_elseLast;
};

class WhileBlock : public BasicBlock {
public:
	WhileBlock(): p_first(0), p_last(0) {}
	WhileBlock(BasicBlock* next, BasicBlock* first, BasicBlock* last): BasicBlock(next), p_first(first), p_last(last) {}
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
	void clear();
	Status buildCFG(const vector<Stmt*>& stmtList);
	Status buildBlock(BasicBlock*& currBlock, const vector<Stmt*>& stmtList);
	friend TraverserOne;
	friend TraverserAllPath;
private:
	BasicBlock* head;
};

#endif /* SRC_CONTROLFLOWGRAPH_H_ */
