#ifndef COMPUTEREACHINGDEFSVISITOR_H
#define COMPUTEREACHINGDEFSVISITOR_H

#include "Visitor.h"
#include <map>
class ComputeReachingDefsVisitor : public Visitor {
public:
    ComputeReachingDefsVisitor();
    virtual ~ComputeReachingDefsVisitor();

    virtual void visitBasicBlock(BasicBlock* basicBlock);
    virtual void visitIfElseBlock(IfElseBlock* ifElseBlock);
    virtual void visitWhileBlock(WhileBlock* whileBlock);
    virtual void visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock);
    virtual void visitFunctionCallBlock(FunctionCallBlock* functionCallBlock);
    virtual void visitCFG(BasicBlock* block);

    void meet(BasicBlock* basicBlock);
    void meet(BasicBlock* basicBlock, vector<AssignmentNode*>& inVariableNodes);
    void detectChange(map<BasicBlock*, vector<AssignmentNode*>>& oldVariableNodes,
                      BasicBlock* basicBlock,
                      const vector<AssignmentNode*>& newVariableNodes);
    map<BasicBlock*, vector<AssignmentNode*>> getInVariableNodes() { return p_inVariableNodes; }
private:
    map<BasicBlock*, vector<AssignmentNode*>> p_inVariableNodes;
    map<BasicBlock*, vector<AssignmentNode*>> p_outVariableNodes;
    bool p_variableNodesChanged = true;
};

#endif // COMPUTEREACHINGDEFSVISITOR_H
