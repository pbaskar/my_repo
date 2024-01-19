#include "visitor.h"
#include "basicblock.h"

Visitor::Visitor() {
    // TODO Auto-generated constructor stub

}

Visitor::~Visitor() {
    // TODO Auto-generated destructor stub
}

void Visitor::visitCFG(const BasicBlock* block) {
    block->acceptVisitor(*this);
}

PrintVisitor::PrintVisitor() {

}

PrintVisitor::~PrintVisitor() {

}

void PrintVisitor::visitBasicBlock(const BasicBlock* basicBlock) {
    basicBlock->print();
}

void PrintVisitor::visitIfElseBlock(const IfElseBlock* ifElseBlock) {
    const QList<BasicBlock*> ifBlocks = ifElseBlock->getIfBlocks();
    for(const BasicBlock* basicBlock : ifBlocks) {
        basicBlock->acceptVisitor(*this);
    }
    const QList<BasicBlock*> elseBlocks = ifElseBlock->getElseBlocks();
    for(const BasicBlock* basicBlock : elseBlocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PrintVisitor::visitWhileBlock(const WhileBlock* whileBlock) {
    const QList<BasicBlock*> blocks = whileBlock->getBlocks();
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PrintVisitor::visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock) {
    const QList<BasicBlock*> blocks = functionDeclBlock->getBlocks();
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PrintVisitor::visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock) {
    const BasicBlock* args = functionCallBlock->getArgs();
    args->acceptVisitor(*this);
    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    fnDecl->acceptVisitor(*this);
}

void PositionVisitor::visitBasicBlock(const BasicBlock* basicBlock) {

}

void PositionVisitor::visitIfElseBlock(const IfElseBlock* ifElseBlock) {
    const QList<BasicBlock*> ifBlocks = ifElseBlock->getIfBlocks();
    for(const BasicBlock* basicBlock : ifBlocks) {
        basicBlock->acceptVisitor(*this);
    }
    const QList<BasicBlock*> elseBlocks = ifElseBlock->getElseBlocks();
    for(const BasicBlock* basicBlock : elseBlocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PositionVisitor::visitWhileBlock(const WhileBlock* whileBlock) {
    const QList<BasicBlock*> blocks = whileBlock->getBlocks();
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PositionVisitor::visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock) {
    const QList<BasicBlock*> blocks = functionDeclBlock->getBlocks();
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
    }
}

void PositionVisitor::visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock) {
    const BasicBlock* args = functionCallBlock->getArgs();
    args->acceptVisitor(*this);
    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    fnDecl->acceptVisitor(*this);
}
