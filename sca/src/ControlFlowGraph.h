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
	AssignmentNode(AssignStmt stmt) {
		p_name = stmt.p_name;
		p_value = stmt.p_value;
		//cout <<"assignment node const " <<*stmt.p_value <<endl;
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
	ConditionNode() : p_condition(0) { }
	ConditionNode(IfStmt stmt) : p_condition(stmt.p_condition) {
	}
	ConditionNode(WhileStmt stmt) : p_condition(stmt.p_condition) {
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

	Expr* p_condition;
};

class BasicBlock {
public:
	BasicBlock(): p_next(0), p_back(0) {}
	virtual ~BasicBlock() {
		for(Node* n : nodeList) {
			delete n;
		}
	}
	virtual void print(ostream& os) {
		for(Node* n: nodeList) {
			os <<*n <<" ";
		}
	}
    friend ostream& operator<<(ostream& os, BasicBlock& basicBlock) {
        basicBlock.print(os);
        return os;
    }
	virtual void setBack(BasicBlock* back) {
		p_back = back;
	}
	virtual void setNext(BasicBlock* next) {
		p_next = next;
	}
	virtual BasicBlock* getNext() {
		return p_next;
	}
	void addNode(Node* newNode) {
		nodeList.push_back(newNode);
	}
	vector<Node*> nodeList;
	BasicBlock* p_next;
	BasicBlock* p_back;
};

class IfElseBlock : public BasicBlock {
public:
	IfElseBlock(): p_ifLast(0), p_elseLast(0) {}
	~IfElseBlock() {
		//delete p_ifBlock;
		//delete p_elseBlock;
	}
	virtual void print(ostream& os) {
		BasicBlock* basicBlock = p_next;
		os <<endl <<"Begin IfElse	" <<endl;
		while(basicBlock != p_ifLast) {
			os <<"print if " << basicBlock <<" " <<*basicBlock <<" p_next " <<basicBlock->p_next <<endl;
			basicBlock = basicBlock->p_next;
		}
		os <<"last if " <<basicBlock <<" " <<*basicBlock <<" p_next " <<basicBlock->p_next <<endl;
		basicBlock = p_back;
		os<<endl <<"Else case " <<endl;
		while(basicBlock != p_elseLast) {
			os <<"print else " << basicBlock <<" " << *basicBlock <<" " <<basicBlock->p_next <<endl;
			basicBlock = basicBlock->p_next;
		}
		if(basicBlock)
			os <<"last else " <<basicBlock <<" " <<*basicBlock <<" p_next " <<basicBlock->p_next <<endl;
		os <<"End IfElse	" <<endl;
	}
	virtual void setBack(BasicBlock* back) {
		p_ifLast->setBack(back);
		if(p_back)
			p_elseLast->setBack(back);
	}
	virtual void setNext(BasicBlock* next) {
		p_ifLast->setNext(next);
		if(p_back)
			p_elseLast->setNext(next);
	}
	virtual BasicBlock* getNext() {
		return p_ifLast->getNext();;
	}
	BasicBlock* p_ifLast;
	BasicBlock* p_elseLast;
};

class WhileBlock : public BasicBlock {
public:
	WhileBlock(): p_last(0) {}
	~WhileBlock() {
	}
	virtual void print(ostream& os) {
		os << this <<" p_next "<< p_next <<" p_back " <<p_back <<" p_last " <<p_last;
		BasicBlock::print(os);
	}
	virtual void setBack(BasicBlock* back) {
		if(p_last != this)
			p_last->setBack(back);
		p_back = back;
	}
	virtual void setNext(BasicBlock* next) {
		if(p_last != this)
			p_last->setNext(next);
		p_next = next;
	}
	virtual BasicBlock* getNext() {
		if(p_last != this)
			return p_last->getNext();
		return p_next;
	}
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
	Status buildCFG(const vector<Stmt*>& stmtList);
	Status buildBlock(BasicBlock*& currBlock, const vector<Stmt*>& stmtList);
private:
	BasicBlock* head;
};

#endif /* SRC_CONTROLFLOWGRAPH_H_ */
