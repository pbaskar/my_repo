#ifndef VISITOR_H_
#define VISITOR_H_

class BasicBlock;
class IfElseBlock;
class WhileBlock;
class FunctionDeclBlock;
class FunctionCallBlock;
class Visitor {
public:
    Visitor();
    virtual ~Visitor();
    virtual void visitBasicBlock(const BasicBlock* basicBlock)=0;
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock)=0;
    virtual void visitWhileBlock(const WhileBlock* whileBlock)=0;
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
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock);
};

class PositionVisitor : public Visitor {
public:
    PositionVisitor();
    virtual ~PositionVisitor();

    virtual void visitBasicBlock(const BasicBlock* basicBlock);
    virtual void visitIfElseBlock(const IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(const WhileBlock* whileBlock);
    virtual void visitFunctionDeclBlock(const FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(const FunctionCallBlock* functionCallBlock);
};

#endif /* VISITOR_H_ */
