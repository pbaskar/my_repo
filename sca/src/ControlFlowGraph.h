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
using namespace std;

enum NodeType {
	ASSIGNMENT,
	CONDITION
};

class BasicBlock {
public:
	BasicBlock(): p_exit(0), p_back(0) {}
	~BasicBlock() {
		for(Node* n : nodeList) {
			delete n;
		}
	}
	vector<Node*> nodeList;
	Node* p_exit;
	Node* p_back;
};

class Node {
public:
	Node(): p_next(0) {}
	virtual ~Node(){
		//cout<<"Node destroyed " << *this <<endl;
	}
	virtual NodeType type()=0;
	virtual void print(ostream& os)=0;
    friend ostream& operator<<(ostream& os, Node& node) {
    	node.print(os);
    	return os;
    }
    Node* p_next;
};

class AssignmentNode : public Node {
public:
	AssignmentNode() : p_name(0), p_value(0) { }
	AssignmentNode(Stmt* stmt) {
		p_name = stmt->name;
		p_value = stmt->value;
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

	char* p_name;
	Expr* p_value;
};

class ConditionNode : public Node {
public:
	ConditionNode() : p_condition(0), p_exit(0) { }
	ConditionNode(Stmt* stmt) {
		p_condition = stmt->condition;
		p_exit = 0;
	}
	NodeType type() { return CONDITION; }
	virtual ~ConditionNode() {
		delete p_condition;
	}
	virtual void print(ostream& os) {
		//os << "condition " << *p_condition <<" ";
	}
	void setCondition(Expr* condition) { p_condition = condition; }

	Expr* p_condition;
	Node* p_exit;
};

class ControlFlowGraph {
public:
	ControlFlowGraph();
	virtual ~ControlFlowGraph();

	Status buildCFG(const vector<Stmt*>& stmtList);
	Status buildBlock(Node*& begin, const vector<Stmt*>& stmtList, Node*& end);
private:
	Node* head;
};

#endif /* SRC_CONTROLFLOWGRAPH_H_ */
