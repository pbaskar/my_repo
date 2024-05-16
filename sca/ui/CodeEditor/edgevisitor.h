#ifndef EDGEVISITOR_H
#define EDGEVISITOR_H
#include <QList>
#include <QMap>
#include "visitor.h"

class BasicBlock;
class IfElseBlock;
class WhileBlock;
class FunctionDeclBlock;
class FunctionCallBlock;
class PositionBlock;
class Edge;
class EdgeVisitor : public Visitor
{
public:
    EdgeVisitor();
    virtual ~EdgeVisitor();
    const QList<Edge> getEdges();
    void setPositionBlocks(const QList<PositionBlock> positionBlocks) { p_positionBlocks = positionBlocks; }

    virtual void visitBasicBlock(const BasicBlock* basicBlock);
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(const WhileBlock* whileBlock);
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock);
    virtual void visitCFG(const BasicBlock* block);
private:
    QList<Edge> p_edges;
    QList<PositionBlock> p_positionBlocks;
};


#endif // EDGEVISITOR_H
