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

ForBlock::ForBlock(QList<BasicBlock*> blocks)
    : p_blocks(blocks) {
}

void ForBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitForBlock(this);
}

FunctionDeclBlock::FunctionDeclBlock(QString name, QList<BasicBlock*> blocks)
    : p_name(name), p_blocks(blocks) {
}

void FunctionDeclBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitFunctionDeclBlock(this);
}

FunctionCallBlock::FunctionCallBlock(QString name, BasicBlock* args, FunctionDeclBlock* fnDecl)
    : p_name(name), p_args(args), p_fnDecl(fnDecl) {
}

void FunctionCallBlock::acceptVisitor(Visitor& visitor) const {
    visitor.visitFunctionCallBlock(this);
}
