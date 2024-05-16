#include "edgevisitor.h"
#include "basicblock.h"
#include "positionblock.h"
#include "edge.h"

EdgeVisitor::EdgeVisitor() {

}

EdgeVisitor::~EdgeVisitor() {
}

const QList<Edge> EdgeVisitor::getEdges() {
    return p_edges;
}

void EdgeVisitor::visitBasicBlock(const BasicBlock* basicBlock) {

}

void EdgeVisitor::visitIfElseBlock(const IfElseBlock* ifElseBlock) {
    PositionBlock lastPositionBlock, positionBlock;
    const QList<BasicBlock*> ifBlocks = ifElseBlock->getIfBlocks();
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [ifElseBlock] (PositionBlock p) {return p.getBlock() == ifElseBlock;});
                    it != p_positionBlocks.end()) {
        lastPositionBlock = *it;
    }
    Side ifSide = Side::TOP;
    for(const BasicBlock* basicBlock : ifBlocks) {
        if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                [basicBlock] (PositionBlock p) {return p.getBlock() == basicBlock;});
                it != p_positionBlocks.end()) {
            positionBlock = *it;
        }
        Edge e(ifSide, Side::TOP, lastPositionBlock, positionBlock);
        qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
        p_edges.append(e);
        basicBlock->acceptVisitor(*this);
        lastPositionBlock = positionBlock;
        ifSide = Side::BOTTOM;
    }
    const QList<BasicBlock*> elseBlocks = ifElseBlock->getElseBlocks();
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [ifElseBlock] (PositionBlock p) {return p.getBlock() == ifElseBlock;});
                    it != p_positionBlocks.end()) {
        lastPositionBlock = *it;
    }
    Side elseSide = Side::TOP;
    for(const BasicBlock* basicBlock : elseBlocks) {
        if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                [basicBlock] (PositionBlock p) {return p.getBlock() == basicBlock;});
                it != p_positionBlocks.end()) {
            positionBlock = *it;
        }
        Edge e(elseSide, Side::TOP, lastPositionBlock, positionBlock);
        qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
        p_edges.append(e);
        basicBlock->acceptVisitor(*this);
        lastPositionBlock = positionBlock;
        elseSide = Side::BOTTOM;
    }
}

void EdgeVisitor::visitWhileBlock(const WhileBlock* whileBlock) {
    PositionBlock lastPositionBlock, positionBlock;
    const QList<BasicBlock*> blocks = whileBlock->getBlocks();
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [whileBlock] (PositionBlock p) {return p.getBlock() == whileBlock;});
                    it != p_positionBlocks.end()) {
        lastPositionBlock = *it;
    }
    Side side = Side::TOP;
    for(const BasicBlock* basicBlock : blocks) {
        if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                [basicBlock] (PositionBlock p) {return p.getBlock() == basicBlock;});
                it != p_positionBlocks.end()) {
            positionBlock = *it;
        }
        Edge e(side, Side::TOP, lastPositionBlock, positionBlock);
        qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
        p_edges.append(e);
        basicBlock->acceptVisitor(*this);
        lastPositionBlock = positionBlock;
        side = Side::BOTTOM;
    }
}

void EdgeVisitor::visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock) {
    PositionBlock lastPositionBlock, positionBlock;
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [functionDeclBlock] (PositionBlock p) {return p.getBlock() == functionDeclBlock;});
                    it != p_positionBlocks.end()) {
        lastPositionBlock = *it;
    }
    Side side = Side::TOP;
    const QList<BasicBlock*> blocks = functionDeclBlock->getBlocks();
    for(const BasicBlock* basicBlock : blocks) {
        if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                [basicBlock] (PositionBlock p) {return p.getBlock() == basicBlock;});
                it != p_positionBlocks.end()) {
            positionBlock = *it;
        }
        Edge e(side, Side::TOP, lastPositionBlock, positionBlock);
        qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
        p_edges.append(e);
        basicBlock->acceptVisitor(*this);
        lastPositionBlock = positionBlock;
        side = Side::BOTTOM;
    }
}

void EdgeVisitor::visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock) {
    PositionBlock lastPositionBlock, positionBlock;
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [functionCallBlock] (PositionBlock p) {return p.getBlock() == functionCallBlock;});
                    it != p_positionBlocks.end()) {
        lastPositionBlock = *it;
    }
    Side side = Side::TOP;
    const BasicBlock* args = functionCallBlock->getArgs();
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [args] (PositionBlock p) {return p.getBlock() == args;});
                    it != p_positionBlocks.end()) {
        positionBlock = *it;
    }
    Edge e1(side, Side::TOP, lastPositionBlock, positionBlock);
    qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
    p_edges.append(e1);
    args->acceptVisitor(*this);
    lastPositionBlock = positionBlock;
    side = Side::BOTTOM;

    const FunctionDeclBlock* fnDecl = functionCallBlock->getFnDecl();
    if(auto it = std::find_if(p_positionBlocks.begin(), p_positionBlocks.end(),
                    [fnDecl] (PositionBlock p) {return p.getBlock() == fnDecl;});
                    it != p_positionBlocks.end()) {
        positionBlock = *it;
    }
    Edge e2(side, Side::TOP, lastPositionBlock, positionBlock);
    qDebug() <<"Edge from to " <<lastPositionBlock.getBottomConnection() <<" " <<positionBlock.getTopConnection();
    p_edges.append(e2);
    fnDecl->acceptVisitor(*this);
}

void EdgeVisitor::visitCFG(const BasicBlock* block) {
    block->acceptVisitor(*this);
}
