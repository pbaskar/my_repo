#ifndef VISITOR_H_
#define VISITOR_H_
#include <QList>

class BasicBlock;
class IfElseBlock;
class WhileBlock;
class ForBlock;
class FunctionDeclBlock;
class FunctionCallBlock;
class PositionBlock;
class Visitor {
public:
    Visitor();
    virtual ~Visitor();
    virtual void visitBasicBlock(const BasicBlock* basicBlock)=0;
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock)=0;
    virtual void visitWhileBlock(const WhileBlock* whileBlock)=0;
    virtual void visitForBlock(const ForBlock* forBlock) = 0;
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock)=0;
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock)=0;
    virtual void visitCFG(const BasicBlock* block);
};

class PrintVisitor : public Visitor {
public:
    PrintVisitor();
    virtual ~PrintVisitor();

    virtual void visitBasicBlock(const BasicBlock* basicBlock);
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(const WhileBlock* whileBlock);
    virtual void visitForBlock(const ForBlock* forBlock);
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock);
};

class PositionVisitor : public Visitor {
public:
    PositionVisitor();
    virtual ~PositionVisitor();
    const QList<PositionBlock> getPositionBlocks();

    virtual void visitBasicBlock(const BasicBlock* basicBlock);
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(const WhileBlock* whileBlock);
    virtual void visitForBlock(const ForBlock* forBlock);
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock);
private:
    int p_parentX;
    int p_parentY;
    QList<PositionBlock> p_positionBlocks;
};

#endif /* VISITOR_H_ */
