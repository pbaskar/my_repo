#include "visitor.h"
#include "basicblock.h"
#include "positionblock.h"
#include <QFile>

static const int padding = 15;
static const int topPadding = 50;
static const int stmtHeight = 17;
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

void PrintVisitor::visitForBlock(const ForBlock* forBlock) {
    const QList<BasicBlock*> blocks = forBlock->getBlocks();
    for (const BasicBlock* basicBlock : blocks) {
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
    if(args != nullptr)
        args->acceptVisitor(*this);
    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    fnDecl->acceptVisitor(*this);
}

PositionVisitor::PositionVisitor() : p_parentX(padding), p_parentY(padding) {
}

PositionVisitor::~PositionVisitor() {
}

const QList<PositionBlock> PositionVisitor::getPositionBlocks() {
    return p_positionBlocks;
}

void PositionVisitor::visitBasicBlock(const BasicBlock* basicBlock) {
    const QStringList& stmts = basicBlock->getStmts();
    int h = stmts.empty() ? stmtHeight : (stmts.size() * stmtHeight);
    int w = stmtWidth;
    int x = p_parentX;
    int y = p_parentY;

    h += 2*padding; //top bottom padding
    w += 2*padding; //left right padding
    PositionBlock positionBlock(x, y, w, h, basicBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
    p_positionBlocks.append(positionBlock);
}

void PositionVisitor::visitIfElseBlock(const IfElseBlock* ifElseBlock) {
    const QList<BasicBlock*> ifBlocks = ifElseBlock->getIfBlocks();
    int parentX = p_parentX;
    qDebug() <<Q_FUNC_INFO << parentX;
    int parentY = p_parentY;
    int lastParentX = p_parentX;
    int lastParentY = p_parentY;
    int ifElseWidth = 0;
    int ifElseHeight = 0;
    int ifWidth = 0;
    int ifHeight = 0;
    int elseWidth = 0;
    int elseHeight = 0;

    p_parentX += padding; //left padding
    p_parentY += topPadding; //top padding
    lastParentY = p_parentY;
    Q_ASSERT(!p_positionBlocks.empty());
    for(const BasicBlock* basicBlock : ifBlocks) {
        lastParentX = p_parentX;
        basicBlock->acceptVisitor(*this);
        ifHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
        ifWidth = qMax(ifWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + ifHeight;
    }
    p_parentX = lastParentX + ifWidth + padding; //middle padding
    p_parentY = parentY + topPadding; //top padding
    lastParentY = p_parentY;
    const QList<BasicBlock*> elseBlocks = ifElseBlock->getElseBlocks();
    for(const BasicBlock* basicBlock : elseBlocks) {
        basicBlock->acceptVisitor(*this);
        elseHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks;
        elseWidth = qMax(elseWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + elseHeight;
    }

    ifElseWidth = ifWidth + elseWidth + 3*padding; //left, middle, right padding
    ifElseHeight = qMax(ifHeight, elseHeight) + topPadding; //top padding
    PositionBlock positionBlock(parentX, parentY, ifElseWidth, ifElseHeight, ifElseBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth()
             <<" "<<positionBlock.getHeight();
    p_parentX = parentX;
    p_parentY = parentY + ifElseHeight;
    qDebug()<<"ifelse end " <<p_parentY;
}

void PositionVisitor::visitWhileBlock(const WhileBlock* whileBlock) {
        qDebug()<<" while begin " <<p_parentY;
    const QList<BasicBlock*> blocks = whileBlock->getBlocks();
    int parentX = p_parentX;
    int parentY = p_parentY;
    int lastParentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;

    p_parentX += padding; //left padding
    p_parentY += topPadding; //top padding
    lastParentY = p_parentY;
    Q_ASSERT(!p_positionBlocks.empty());
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + blockHeight;
    }

    blockWidth += 2*padding; //left and right padding
    blockHeight += topPadding; //top padding
    PositionBlock positionBlock(parentX, parentY, blockWidth, blockHeight, whileBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight() <<" " <<p_parentY;
    p_parentX = parentX;
}

void PositionVisitor::visitForBlock(const ForBlock* forBlock) {
    qDebug() << " for begin " << p_parentY;
    const QList<BasicBlock*> blocks = forBlock->getBlocks();
    int parentX = p_parentX;
    int parentY = p_parentY;
    int lastParentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;

    p_parentX += padding; //left padding
    p_parentY += topPadding; //top padding
    lastParentY = p_parentY;
    Q_ASSERT(!p_positionBlocks.empty());
    for (const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + blockHeight;
    }

    blockWidth += 2 * padding; //left and right padding
    blockHeight += topPadding; //top padding
    PositionBlock positionBlock(parentX, parentY, blockWidth, blockHeight, forBlock);
    p_positionBlocks.append(positionBlock);
    qDebug() << Q_FUNC_INFO << positionBlock.getX() << " " << positionBlock.getY() << " " << positionBlock.getWidth() << " " << positionBlock.getHeight() << " " << p_parentY;
    p_parentX = parentX;
}

void PositionVisitor::visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock) {
    const QList<BasicBlock*> blocks = functionDeclBlock->getBlocks();
    int parentX = p_parentX;
    int parentY = p_parentY;
    int lastParentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;

    p_parentX += padding; //left padding
    p_parentY += topPadding; //top padding
    lastParentY = p_parentY;
    Q_ASSERT(!p_positionBlocks.empty());
    for(const BasicBlock* basicBlock : blocks) {
        basicBlock->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + blockHeight;
    }

    blockWidth += 2*padding; //left and right padding
    blockHeight += topPadding; //top padding
    PositionBlock positionBlock(parentX, parentY, blockWidth, blockHeight, functionDeclBlock);
    p_positionBlocks.append(positionBlock);
    p_parentX = parentX;
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
}

void PositionVisitor::visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock) {
    int parentX = p_parentX;
    int parentY = p_parentY;
    int lastParentY = p_parentY;
    int blockWidth = 0;
    int blockHeight = 0;

    p_parentX += padding; //left padding
    p_parentY += topPadding; //top padding
    Q_ASSERT(!p_positionBlocks.empty());
    const BasicBlock* args = functionCallBlock->getArgs();
    if(args != nullptr) {
        lastParentY = p_parentY;
        args->acceptVisitor(*this);
        blockHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
        blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
        p_parentY = lastParentY + blockHeight;
    }

    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    lastParentY = p_parentY;
    fnDecl->acceptVisitor(*this);
    blockHeight += p_positionBlocks.constLast().getHeight() + topPadding; //padding between intermediate blocks
    blockWidth = qMax(blockWidth, p_positionBlocks.constLast().getWidth());
    p_parentY = lastParentY + blockHeight;

    blockWidth += 2*padding; //left and right padding
    blockHeight += topPadding; //top padding
    PositionBlock positionBlock(parentX, parentY, blockWidth, blockHeight, functionCallBlock);
    p_positionBlocks.append(positionBlock);
    QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "Visitor::fncallblock x,y " << positionBlock.getX() << " " << positionBlock.getY() << " "
        << positionBlock.getWidth() << " " << positionBlock.getHeight() << Qt::endl;
        Qt::endl;
    file.close();
    qDebug() << Q_FUNC_INFO << positionBlock.getX() <<" " <<positionBlock.getY() <<" "<<positionBlock.getWidth() <<" "<<positionBlock.getHeight();
    p_parentX = parentX;
}
