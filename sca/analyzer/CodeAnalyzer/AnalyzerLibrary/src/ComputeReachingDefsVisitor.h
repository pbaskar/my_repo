#ifndef COMPUTEREACHINGDEFSVISITOR_H
#define COMPUTEREACHINGDEFSVISITOR_H

#include "Visitor.h"
#include <map>

class Variable;
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
    void meet(BasicBlock* basicBlock, map<const Variable*, vector<AssignmentNode*>>& inVariableNodes);
    void detectChange(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& oldVariableNodes,
                      BasicBlock* basicBlock,
                      const map<const Variable*, vector<AssignmentNode*>>& newVariableNodes);
    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> getInVariableNodes() { return p_inVariableNodes; }
private:
    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> p_inVariableNodes;
    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> p_outVariableNodes;
    bool p_variableNodesChanged = true;
};

#endif // COMPUTEREACHINGDEFSVISITOR_H
