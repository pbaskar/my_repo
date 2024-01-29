#include "visitor.h"
#include "basicblock.h"
#include "positionblock.h"

static const int padding = 25;
static const int stmtHeight = 18;
static const int stmtWidth = 150;
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

PositionVisitor::PositionVisitor() : p_parentX(0), p_parentY(0) {
}

PositionVisitor::~PositionVisitor() {
}

const QList<PositionBlock> PositionVisitor::getPositionBlocks() {
    return p_positionBlocks;
}

void PositionVisitor::visitBasicBlock(const BasicBlock* basicBlock) {
    const QStringList& stmts = basicBlock->getStmts();
    int h = stmts.size() * stmtHeight;
    int w = stmtWidth;
    int x = p_parentX;
    int y = p_parentY;
    PositionBlock positionBlock(x, y, w, h, basicBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
    p_positionBlocks.append(positionBlock);
}

void PositionVisitor::visitIfElseBlock(const IfElseBlock* ifElseBlock) {
    const QList<BasicBlock*> ifBlocks = ifElseBlock->getIfBlocks();
    int parentX = p_parentX;
    qDebug() <<Q_FUNC_INFO << parentX;
    int parentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;
    int ifWidth = 0;
    int ifHeight = 0;
    for(const BasicBlock* basicBlock : ifBlocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + padding;
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = parentY + blockHeight;
    }
    ifWidth = blockWidth + padding;
    ifHeight = blockHeight;
    p_parentX = ifWidth;
    p_parentY = parentY;
    blockWidth = 0;
    blockHeight = 0;
    const QList<BasicBlock*> elseBlocks = ifElseBlock->getElseBlocks();
    for(const BasicBlock* basicBlock : elseBlocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + padding;
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = parentY + blockHeight;
    }
    blockWidth = ifWidth + blockWidth;
    blockHeight = qMax(ifHeight, blockHeight);
    PositionBlock positionBlock(parentX, parentY, blockWidth, blockHeight, ifElseBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
    p_parentX = parentX;
    p_parentY = parentY + blockHeight;
}

void PositionVisitor::visitWhileBlock(const WhileBlock* whileBlock) {
    const QList<BasicBlock*> blocks = whileBlock->getBlocks();
    int parentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + padding;
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = parentY + blockHeight;
    }
    PositionBlock positionBlock(p_parentX, parentY, blockWidth, blockHeight, whileBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
}

void PositionVisitor::visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock) {
    const QList<BasicBlock*> blocks = functionDeclBlock->getBlocks();
    int parentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + padding;
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = parentY + blockHeight;
    }
    PositionBlock positionBlock(p_parentX, parentY, blockWidth, blockHeight, functionDeclBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
}

void PositionVisitor::visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock) {
    int parentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;
    const BasicBlock* args = functionCallBlock->getArgs();
    args->acceptVisitor(*this);
    blockHeight += p_positionBlocks.constLast().getHeight() + padding;
    blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
    p_parentY = parentY + blockHeight;
    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    fnDecl->acceptVisitor(*this);
    blockHeight += p_positionBlocks.constLast().getHeight() + padding;
    blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
    PositionBlock positionBlock(p_parentX, parentY, blockWidth, blockHeight, functionCallBlock);
    p_parentY = parentY + blockHeight;
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
}
