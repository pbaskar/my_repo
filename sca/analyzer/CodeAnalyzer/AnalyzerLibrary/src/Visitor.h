/*
 * Visitor.h
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#ifndef SRC_VISITOR_H_
#define SRC_VISITOR_H_
#include<vector>
#include<map>
#include "Results.h"
using namespace std;

class BasicBlock;
class IfElseBlock;
class WhileBlock;
class FunctionDeclBlock;
class FunctionCallBlock;
class AssignmentNode;
class Variable;
class Definition;
class Visitor {
public:
    Visitor();
    virtual ~Visitor();
    virtual void visitBasicBlock(BasicBlock* basicBlock)=0;
    virtual void visitIfElseBlock(IfElseBlock* ifElseBlock)=0;
    virtual void visitWhileBlock(WhileBlock* whileBlock)=0;
    virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock)=0;
    virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock)=0;
    virtual void visitCFG(BasicBlock* block)=0;
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
    virtual void visitCFG(BasicBlock* block){}
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
    virtual void visitCFG(BasicBlock* block){}
};

class VariableInitCheckVisitor : public Visitor {
public:
    VariableInitCheckVisitor(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& inVariableNodes,
                             map<BasicBlock*, map<const Variable*, vector<pair<const Definition*, bool>>>>& inDefinitions);
    virtual ~VariableInitCheckVisitor();

    virtual void visitBasicBlock(BasicBlock* basicBlock);
    virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(WhileBlock* whileBlock);
    virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void visitCFG(BasicBlock* block);

    void intersect(vector<AssignmentNode*>& dest, vector<AssignmentNode*>& source);
    const vector<Result>& getResults() { return p_results; }
private:
    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> p_inVariableNodes;
    map<BasicBlock*, map<const Variable*, vector<pair<const Definition*, bool>>>> p_inDefinitions;
    vector<Result> p_results;
};
#endif /* SRC_VISITOR_H_ */
