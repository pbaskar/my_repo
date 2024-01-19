#include "basicblock.h"
#include "visitor.h"

BasicBlock::BasicBlock() {
}

BasicBlock::BasicBlock(QStringList stmts) : p_stmts(stmts) {
}

void BasicBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitBasicBlock(this);
}

IfElseBlock::IfElseBlock(QList<BasicBlock*> ifBlocks, QList<BasicBlock*> elseBlocks)
    : p_ifBlocks(ifBlocks), p_elseBlocks(elseBlocks) {
}

void IfElseBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitIfElseBlock(this);
}

WhileBlock::WhileBlock(QList<BasicBlock*> blocks)
    : p_blocks(blocks) {
}

void WhileBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitWhileBlock(this);
}

FunctionDeclBlock::FunctionDeclBlock(QList<BasicBlock*> blocks)
    : p_blocks(blocks) {
}

void FunctionDeclBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitFunctionDeclBlock(this);
}

FunctionCallBlock::FunctionCallBlock(BasicBlock* args, FunctionDeclBlock* fnDecl)
    : p_args(args), p_fnDecl(fnDecl) {
}

void FunctionCallBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitFunctionCallBlock(this);
}
