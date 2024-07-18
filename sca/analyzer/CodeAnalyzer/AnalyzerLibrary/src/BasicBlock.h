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
    AssignmentNode(Variable* var, Expr* value) : p_var(var), p_value(value) { }
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
    virtual void print() {
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
    void clearFnSymbolEntries() {
        p_symbolTable->clearFnSymbolEntries();
    }
    virtual void setNext(BasicBlock* next) {
        p_next = next;
        next->addPredecessor(this);
    }
    BasicBlock* getNext() { return p_next; }
    void addNode(Node* newNode) {
        nodeList.push_back(newNode);
    }
    const vector<Node*>& getNodeList() { return nodeList; }
    const vector<BasicBlock*>& getPredecessors() { return p_predecessors; }
    virtual void acceptVisitor(Visitor& visitor);
    virtual void acceptTraverser(Traverser& traverser);
    virtual void addPredecessor(BasicBlock* predecessor) {
        p_predecessors.push_back(predecessor);
    }
private:
    vector<Node*> nodeList;
    BasicBlock* p_next;
    vector<BasicBlock*> p_predecessors;
    SymbolTable* p_symbolTable;
};

class IfElseBlock : public BasicBlock {
public:
    IfElseBlock(BasicBlock* next, BasicBlock* ifFirst, BasicBlock* ifLast, BasicBlock* elseFirst, BasicBlock* elseLast)
: BasicBlock(next,0), p_ifFirst(ifFirst), p_ifLast(ifLast), p_elseFirst(elseFirst), p_elseLast(elseLast) {
        p_last = new BasicBlock(nullptr);
        p_ifLast->setNext(p_last);
        p_elseLast->setNext(p_last);
    }
    virtual ~IfElseBlock() {
    }
    virtual void acceptVisitor(Visitor& visitor);
    virtual void acceptTraverser(Traverser& traverser);
    BasicBlock* getIfFirst() { return p_ifFirst; }
    BasicBlock* getIfLast() { return p_ifLast; }
    BasicBlock* getElseFirst() { return p_elseFirst; }
    BasicBlock* getElseLast() { return p_elseLast; }
    BasicBlock* getLast() { return p_last; }
private:
    BasicBlock* p_ifFirst;
    BasicBlock* p_ifLast;
    BasicBlock* p_elseFirst;
    BasicBlock* p_elseLast;
    BasicBlock* p_last;
};

class WhileBlock : public BasicBlock {
public:
    WhileBlock(BasicBlock* next, BasicBlock* first, BasicBlock* last):
        BasicBlock(next, 0), p_first(first), p_last(last) {}
    virtual ~WhileBlock() {
    }
    void setSelf() {
        p_last->setNext(p_first);
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
    virtual void acceptVisitor(Visitor& visitor);
    virtual void acceptTraverser(Traverser& traverser);
    BasicBlock* getFirst() { return p_first; }
    BasicBlock* getLast() { return p_last; }
    const char* getName() { return p_name; }
    const vector<Variable*>& getFormalArguments() const { return p_formalArguments; }
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    void addFormalArgument(Variable* var) { p_formalArguments.push_back(var); }
    virtual void print() {
        cout << "Function name " << p_name << " formal arguments ";
        for(Variable* v : p_formalArguments) {
            cout << *v << " ";
        }
        cout<<endl;
    }
private:
    const char* p_name;
    BasicBlock* p_first;
    BasicBlock* p_last;
    vector<Variable*> p_formalArguments;
};

class FunctionCallBlock : public BasicBlock {
public:
    FunctionCallBlock(BasicBlock* next, const char* name, BasicBlock* first, FunctionDeclBlock* fnDecl):
        BasicBlock(next, 0), p_name(name), p_first(first), p_fnDecl(fnDecl){}
    virtual ~FunctionCallBlock(){}
    virtual void acceptVisitor(Visitor& visitor);
    virtual void acceptTraverser(Traverser& traverser);
    BasicBlock* getFnDecl() { return p_fnDecl; }
    BasicBlock* getFirst() { return p_first; }
    const char* getName() { return p_name; }
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    void addActualArgument(Expr* expr) { p_actualArguments.push_back(expr); }
    virtual void print() {
        cout << "Function name " << p_name << " actual arguments ";
        for(Expr* e : p_actualArguments) {
            cout << *e << " ";
        }
        cout <<endl;
    }
private:
    const char* p_name;
    BasicBlock* p_first;
    FunctionDeclBlock* p_fnDecl;
    vector<Expr*> p_actualArguments;
};

#endif /* BASICBLOCK_H_ */
