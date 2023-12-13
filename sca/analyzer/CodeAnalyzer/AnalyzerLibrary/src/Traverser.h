/*
 * Traverser.h
 *
 *  Created on: Oct 6, 2023
 *      Author: prbas_000
 */

#ifndef SRC_TRAVERSER_H_
#define SRC_TRAVERSER_H_

#include "Visitor.h"

class BasicBlock;
class IfElseBlock;
class WhileBlock;

class Traverser {
public:
    Traverser(Visitor* visitor, bool skipFnDeclBlock);
    virtual ~Traverser();

    virtual void traverseBasicBlock(BasicBlock* basicBlock)=0;
    virtual void traverseIfElseBlock(IfElseBlock* ifElseBlock)=0;
    virtual void traverseWhileBlock(WhileBlock* whileBlock)=0;
    virtual void traverseFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock)=0;
    virtual void traverseFunctionCallBlock(FunctionCallBlock* functionCallBlock)=0;
    virtual void traverseCFG(BasicBlock* block)=0;

    Visitor* p_visitor;
    bool p_skipFnDeclBlock;
};

class TraverserOne : public Traverser {
public:
    TraverserOne(Visitor* visitor, bool skipFnDeclBlock=false);
    virtual ~TraverserOne();

    virtual void traverseBasicBlock(BasicBlock* basicBlock);
    virtual void traverseIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void traverseWhileBlock(WhileBlock* whileBlock);
    virtual void traverseFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void traverseFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void traverseCFG(BasicBlock* block);
};

class TraverserAllPath : public Traverser {
public:
    TraverserAllPath(Visitor* visitor, bool skipFnDeclBlock=false);
    virtual ~TraverserAllPath();

    virtual void traverseBasicBlock(BasicBlock* basicBlock);
    virtual void traverseIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void traverseWhileBlock(WhileBlock* whileBlock);
    virtual void traverseFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void traverseFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void traverseCFG(BasicBlock* block);
};

#endif /* SRC_TRAVERSER_H_ */
