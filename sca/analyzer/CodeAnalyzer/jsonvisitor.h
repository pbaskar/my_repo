#ifndef JSONVISITOR_H
#define JSONVISITOR_H
#include <QJsonArray>
#include "AnalyzerLibrary/analyzer.h"

class JsonVisitor : public Visitor
{
public:
    JsonVisitor();
    virtual ~JsonVisitor();

    virtual void visitBasicBlock(BasicBlock* basicBlock);
    virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(WhileBlock* whileBlock);
    virtual void visitForBlock(ForBlock* forBlock);
    virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void visitCFG(BasicBlock* block);

    QJsonArray getBlocks() { return p_blocks; }
private:
    QJsonArray p_blocks;
};

#endif // JSONVISITOR_H
