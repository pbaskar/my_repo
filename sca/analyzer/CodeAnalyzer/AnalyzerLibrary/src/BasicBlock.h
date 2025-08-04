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
    virtual const Expr* getValue() const =0;
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
    AssignmentNode(const Variable* var, const Expr* value) : p_var(var), p_value(value) { }
    virtual NodeType type() { return ASSIGNMENT; }
    virtual ~AssignmentNode() {
        //delete p_var;
        //delete p_value;
    }
    virtual void print(ostream& os) const {
        if(p_var) os << "name " << *p_var;
        if(p_value)
            os <<" value " <<*p_value <<" ";
    }
    bool operator==(const AssignmentNode& other) {
        return p_var == other.p_var;
    }
    const Variable* getVariable() const { return p_var; }
    virtual const Expr* getValue() const { return p_value; }

private:
    const Variable* p_var;
    const Expr* p_value;
};

class ConditionNode : public Node {
public:
    ConditionNode() : p_condition(0) { }
    ConditionNode(const Expr* condition) : p_condition(condition) {
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
    virtual const Expr* getValue() const { return p_condition; };
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
    /*Variable* addSymbol(const char* name) {
        return p_symbolTable->addSymbol(name);
    }*/
    Variable* fetchVariable(const char* name) {
        return p_symbolTable->fetchVariable(name);
    }
    void addFnSymbol(const char* name) {
        return p_symbolTable->addFnSymbol(name);
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
        if(p_elseLast) {
            p_elseLast->setNext(p_last);
        }
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

class ForBlock : public BasicBlock {
public:
    ForBlock(BasicBlock* next, BasicBlock* first, BasicBlock* last):
        BasicBlock(next, 0), p_first(first), p_last(last) {}
    virtual ~ForBlock() {
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

class FunctionCallInstance {
public:
    FunctionCallInstance(const char* name, BasicBlock* first, FunctionDeclBlock* fnDecl, BasicBlock* last)
            : p_name(name), p_first(first), p_fnDecl(fnDecl), p_last(last) { }
    BasicBlock* getFnDecl() { return p_fnDecl; }
    BasicBlock* getFirst() { return p_first; }
    BasicBlock* getLast() { return p_last; }
    const char* getName() const { return p_name; }
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
    BasicBlock* p_last;
    vector<Expr*> p_actualArguments;
};

class FunctionCallBlock : public BasicBlock {
public:
    FunctionCallBlock(BasicBlock* next, const Expr* name): BasicBlock(next, 0), p_name(name) {
        p_last = new BasicBlock(nullptr);
    }
    virtual ~FunctionCallBlock(){
        for(int i=0; i<p_fnCallInstances.size(); i++)
            delete p_fnCallInstances[i];
    }
    virtual void acceptVisitor(Visitor& visitor);
    virtual void acceptTraverser(Traverser& traverser);
    /*bool match(const Expr* name) {
        return strcmp(p_name, name) ==0;
    }*/
    virtual void print() {
        cout << "Function Call " << p_name << " has Instances " <<endl;
        for(int i=0; i<p_fnCallInstances.size(); i++) {
            p_fnCallInstances[i]->print();
        }
        cout <<endl;
    }
    const Expr* getName() const { return p_name; }
    BasicBlock* getLast() { return p_last; }
    void addInstance(FunctionCallInstance* functionCallInstance) {
        functionCallInstance->getLast()->setNext(p_last);
        p_fnCallInstances.push_back(functionCallInstance);
    }
    vector<FunctionCallInstance*>& getFnCallInstances() { return p_fnCallInstances; }
private:
    const Expr* p_name;
    BasicBlock* p_last;
    vector<FunctionCallInstance*> p_fnCallInstances;
};

#endif /* BASICBLOCK_H_ */
