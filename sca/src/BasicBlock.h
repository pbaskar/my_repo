/*
 * BasicBlock.h
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */

#ifndef BASICBLOCK_H_
#define BASICBLOCK_H_
#include "Stmt.h"

using namespace std;

class Visitor;
class Traverser;

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
	const Node* getNext() { return p_next; }
private:
	Node* p_next;
};

class AssignmentNode : public Node {
public:
	AssignmentNode() : p_var(0), p_value(0) { }
	AssignmentNode(Variable* var) : p_var(var), p_value(0) { }
	AssignmentNode(AssignStmt& stmt) {
		p_var = stmt.getVar();
		p_value = stmt.getValue();
	}
	virtual NodeType type() { return ASSIGNMENT; }
	virtual ~AssignmentNode() {
		//delete p_var;
		//delete p_value;
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
	const Variable* p_var;
	const Expr* p_value;
};

class ConditionNode : public Node {
public:
	ConditionNode() : p_condition(0) { }
	ConditionNode(const IfStmt& stmt) : p_condition(stmt.getCondition()) {
	}
	ConditionNode(const WhileStmt& stmt) : p_condition(stmt.getCondition()) {
	}
	NodeType type() { return CONDITION; }
	virtual ~ConditionNode() {
		//delete p_condition;
	}
	virtual void print(ostream& os) const {
		os <<"condition "<<" ";
		if(p_condition)
			os <<*p_condition <<" ";
	}
	void setCondition(Expr* condition) { p_condition = condition; }
private:
	const Expr* p_condition;
};

class BasicBlock {
public:
	BasicBlock(SymbolTable* symbolTable) : p_next(0), p_symbolTable(symbolTable) {}
	BasicBlock(BasicBlock* next, SymbolTable* symbolTable): p_next(next), p_symbolTable(symbolTable){}
	virtual ~BasicBlock() {
		for(Node* n : nodeList) {
			delete n;
		}
		//delete p_symbolTable;
	}
	void print() {
		for(Node* n: nodeList) {
			cout <<*n <<" ";
		}
		cout<<endl;
	}
	Variable* addSymbol(const char* name) {
		return p_symbolTable->addSymbol(name);
	}
	Variable* fetchVariable(const char* name) {
		return p_symbolTable->fetchVariable(name);
	}
	FunctionDeclBlock* addFnSymbol(FunctionDeclBlock* functionDeclBlock) {
		return p_symbolTable->addFnSymbol(functionDeclBlock);
	}
	FunctionDeclBlock* fetchFunctionDeclBlock(const char* name) {
		return p_symbolTable->fetchFunctionDeclBlock(name);
	}
	virtual void setNext(BasicBlock* next) {
		p_next = next;
	}
	BasicBlock* getNext() { return p_next; }
	void addNode(Node* newNode) {
		nodeList.push_back(newNode);
	}
	const vector<Node*>& getNodeList() { return nodeList; }
	virtual void acceptVisitor(Visitor& visitor);
	virtual void acceptTraverser(Traverser& traverser);
private:
	vector<Node*> nodeList;
	BasicBlock* p_next;
	SymbolTable* p_symbolTable;
};

class IfElseBlock : public BasicBlock {
public:
	IfElseBlock(BasicBlock* next, BasicBlock* ifFirst, BasicBlock* ifLast, BasicBlock* elseFirst, BasicBlock* elseLast)
: BasicBlock(next,0), p_ifFirst(ifFirst), p_ifLast(ifLast), p_elseFirst(elseFirst), p_elseLast(elseLast) {}
	virtual ~IfElseBlock() {
	}
	virtual void setNext(BasicBlock* next) {
		p_ifLast->setNext(next);
		if(p_elseLast)
			p_elseLast->setNext(next);
		BasicBlock::setNext(next);
	}
	virtual void acceptVisitor(Visitor& visitor);
	virtual void acceptTraverser(Traverser& traverser);
	BasicBlock* getIfFirst() { return p_ifFirst; }
	BasicBlock* getIfLast() { return p_ifLast; }
	BasicBlock* getElseFirst() { return p_elseFirst; }
	BasicBlock* getElseLast() { return p_elseLast; }
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
	virtual ~WhileBlock() {
	}
	virtual void setNext(BasicBlock* next) {
		BasicBlock::setNext(next);
	}
	void setSelf() {
		p_last->setNext(this);
	}
	virtual void acceptVisitor(Visitor& visitor);
	virtual void acceptTraverser(Traverser& traverser);
	BasicBlock* getFirst() { return p_first; }
	BasicBlock* getLast() { return p_last; }
private:
	BasicBlock* p_first;
	BasicBlock* p_last;
};

class FunctionDeclBlock : public BasicBlock {
public:
	FunctionDeclBlock(BasicBlock* next, const char* name, BasicBlock* first, BasicBlock* last):
		BasicBlock(next, 0), p_name(name), p_first(first), p_last(last) {}
	virtual ~FunctionDeclBlock();
	virtual void setNext(BasicBlock* next) {
		p_last->setNext(next);
	}
	virtual void acceptVisitor(Visitor& visitor);
	virtual void acceptTraverser(Traverser& traverser);
	BasicBlock* getFirst() { return p_first; }
	BasicBlock* getLast() { return p_last; }
	const char* getName() { return p_name; }
	bool match(const char* name) {
		return strcmp(p_name, name) ==0;
	}
private:
	const char* p_name;
	BasicBlock* p_first;
	BasicBlock* p_last;
};

class FunctionCallBlock : public BasicBlock {
public:
	FunctionCallBlock(BasicBlock* next, const char* name, BasicBlock* first, FunctionDeclBlock* fnDecl):
		BasicBlock(next, 0), p_name(name), p_first(first), p_fnDecl(fnDecl){}
	virtual ~FunctionCallBlock(){}
	virtual void setNext(BasicBlock* next) {
		p_fnDecl->setNext(next);
	}
	virtual void acceptVisitor(Visitor& visitor);
	virtual void acceptTraverser(Traverser& traverser);
	BasicBlock* getFnDecl() { return p_fnDecl; }
	BasicBlock* getFirst() { return p_first; }
	bool match(const char* name) {
		return strcmp(p_name, name) ==0;
	}
private:
	const char* p_name;
	BasicBlock* p_first;
	FunctionDeclBlock* p_fnDecl;
};

#endif /* BASICBLOCK_H_ */
