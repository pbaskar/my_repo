#ifndef CONTROLFLOWGRAPHUPDATER_H
#define CONTROLFLOWGRAPHUPDATER_H
#include "Visitor.h"

class JumpNode;
class ControlFlowGraphUpdater : public Visitor
{
public:
    ControlFlowGraphUpdater();
    virtual ~ControlFlowGraphUpdater();

    virtual void visitBasicBlock(BasicBlock* basicBlock);
    virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(WhileBlock* whileBlock);
    virtual void visitForBlock(ForBlock* forBlock);
    virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void visitCFG(BasicBlock* block);
private:
    BasicBlock* p_loopBreakTargetBlock;
    BasicBlock* p_loopContinueTargetBlock;
    BasicBlock* p_returnTargetBlock;
};

#endif // CONTROLFLOWGRAPHUPDATER_H
