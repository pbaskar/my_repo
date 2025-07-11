/*
 * BasicBlock.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */
#include "BasicBlock.h"
#include "Traverser.h" //Traverse FunctionDeclBlock

void BasicBlock::acceptTraverser(Traverser& traverser) {
    traverser.traverseBasicBlock(this);
}

void IfElseBlock::acceptTraverser(Traverser& traverser) {
    traverser.traverseIfElseBlock(this);
}

void WhileBlock::acceptTraverser(Traverser& traverser) {
    traverser.traverseWhileBlock(this);
}

void FunctionDeclBlock::acceptTraverser(Traverser& traverser) {
    traverser.traverseFunctionDeclBlock(this);
}

void FunctionCallBlock::acceptTraverser(Traverser& traverser) {
    traverser.traverseFunctionCallBlock(this);
}

void BasicBlock::acceptVisitor(Visitor& visitor) {
    visitor.visitBasicBlock(this);
}

void IfElseBlock::acceptVisitor(Visitor& visitor) {
    visitor.visitIfElseBlock(this);
}

void WhileBlock::acceptVisitor(Visitor& visitor) {
    visitor.visitWhileBlock(this);
}

void FunctionDeclBlock::acceptVisitor(Visitor& visitor) {
    visitor.visitFunctionDeclBlock(this);
}

void FunctionCallBlock::acceptVisitor(Visitor& visitor) {
    visitor.visitFunctionCallBlock(this);
}

FunctionDeclBlock::~FunctionDeclBlock() {
}
