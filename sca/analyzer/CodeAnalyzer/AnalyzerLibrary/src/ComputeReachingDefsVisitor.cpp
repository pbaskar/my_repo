#include "ComputeReachingDefsVisitor.h"
#include "BasicBlock.h"
#include <cassert>
#include <algorithm>

ComputeReachingDefsVisitor::ComputeReachingDefsVisitor() {

}

ComputeReachingDefsVisitor::~ComputeReachingDefsVisitor() {

}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock) {
    vector<AssignmentNode*> inVariableNodes;
    meet(basicBlock, inVariableNodes);
}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock, vector<AssignmentNode*>& inVariableNodes) {
    cout <<"Beginning of meet " <<endl;
    const vector<BasicBlock*>& predecessors = basicBlock->getPredecessors();

    if(!predecessors.empty()) {
        vector<AssignmentNode*> intersectVariableNodes;
        auto predecessorIt = predecessors.begin();
        while(predecessorIt != predecessors.end()) {
            if(p_outVariableNodes.find(*predecessorIt) != p_outVariableNodes.end()) {
                intersectVariableNodes = p_outVariableNodes.find(*predecessorIt)->second;
                break;
            }
            predecessorIt++;
        }
        for(;predecessorIt != predecessors.end(); predecessorIt++) {
            if(p_outVariableNodes.find(*predecessorIt) == p_outVariableNodes.end())
                continue;
            vector<AssignmentNode*> variableNodes = p_outVariableNodes.at(*predecessorIt);
            for(auto intersectVariableIt = intersectVariableNodes.begin(); intersectVariableIt != intersectVariableNodes.end(); intersectVariableIt++) {
                auto match = [=](AssignmentNode* variableNode) { return (**intersectVariableIt) == (*variableNode); };
                if(find_if(variableNodes.begin(), variableNodes.end(), match) != variableNodes.end())
                    continue;
                intersectVariableIt = intersectVariableNodes.erase(intersectVariableIt);
            }
        }

        predecessorIt = predecessors.begin();
        while(predecessorIt != predecessors.end()) {
            if(p_outVariableNodes.find(*predecessorIt) != p_outVariableNodes.end())
                break;
            predecessorIt++;
        }
        for(;predecessorIt != predecessors.end(); predecessorIt++) {
            if(p_outVariableNodes.find(*predecessorIt) == p_outVariableNodes.end())
                continue;
            vector<AssignmentNode*> variableNodes = p_outVariableNodes.at(*predecessorIt);
            for(auto variableNodeIt = variableNodes.begin(); variableNodeIt != variableNodes.end(); variableNodeIt++) {
                auto match = [=](AssignmentNode* intersectVariableNode) { return (**variableNodeIt) == (*intersectVariableNode); };
                if(find_if(intersectVariableNodes.begin(), intersectVariableNodes.end(), match) != intersectVariableNodes.end()) {
                    if(find(inVariableNodes.begin(), inVariableNodes.end(), *variableNodeIt) != inVariableNodes.end())
                        continue;
                    inVariableNodes.push_back(*variableNodeIt);
                }
            }
        }
    }
    detectChange(p_inVariableNodes, basicBlock, inVariableNodes);
    p_inVariableNodes.insert_or_assign(basicBlock, inVariableNodes);
    cout <<"End of meet " <<inVariableNodes.size() <<endl;
}

void ComputeReachingDefsVisitor::detectChange(map<BasicBlock*, vector<AssignmentNode*>>& variableNodesAllBlocks,
                                              BasicBlock* basicBlock,
                                              const vector<AssignmentNode*>& newVariableNodes)
{
    cout <<"Beginning of detectChange " <<endl;
    if(variableNodesAllBlocks.find(basicBlock) != variableNodesAllBlocks.end()) {
        vector<AssignmentNode*> oldVariableNodes = variableNodesAllBlocks.at(basicBlock);
        for(AssignmentNode* assignmentNode : newVariableNodes) {
            auto assignNodeIt = find(oldVariableNodes.begin(), oldVariableNodes.end(), assignmentNode);
            if( assignNodeIt != oldVariableNodes.end()) {
                oldVariableNodes.erase(assignNodeIt);
            }
            else {
                p_variableNodesChanged = true;
                break;
            }
        }
        if(!oldVariableNodes.empty())
            p_variableNodesChanged = true;
    }
    else
        p_variableNodesChanged = true;
    cout <<"End of detectChange:: variable changed " <<p_variableNodesChanged <<endl;
}

void ComputeReachingDefsVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    vector<AssignmentNode*> outVariableNodes = p_inVariableNodes.at(basicBlock);
    cout <<"Beginning of Block: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
    vector<const Expr*> variables;
    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        if(node->type() != ASSIGNMENT) continue;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        const Expr* value=assignNode->getValue();
        if(value) {
            value->getVariables(variables);
        }
        else continue;
        cout <<"RHS value " << *value <<endl;
        for(auto variableNodeIt=outVariableNodes.begin(); variableNodeIt != outVariableNodes.end(); ) {
            if(*assignNode==*(*variableNodeIt)) {
                variableNodeIt = outVariableNodes.erase(variableNodeIt);
            }
            else variableNodeIt++;
        }
        outVariableNodes.push_back(assignNode);
        cout<<" added " <<*assignNode << " " <<assignNode <<endl;
    }
    detectChange(p_outVariableNodes, basicBlock, outVariableNodes);
    p_outVariableNodes.insert_or_assign(basicBlock, outVariableNodes);
    cout <<"End of Block: variableNodes size " <<outVariableNodes.size() <<" " <<basicBlock <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);

    vector<AssignmentNode*> variableNodes = p_inVariableNodes.at(ifElseBlock);
    cout <<"Beginning of IfElseBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(ifElseBlock->getIfFirst(), variableNodes);
    meet(ifElseBlock->getElseFirst(), variableNodes);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    lastBlock->acceptVisitor(*this);

    block = ifElseBlock->getElseFirst();
    lastBlock = ifElseBlock->getElseLast();
    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    if(block) {
        block->acceptVisitor(*this);
    }

    meet(ifElseBlock->getLast());
    ifElseBlock->getLast()->acceptVisitor(*this);
    vector<AssignmentNode*> outVariableNodes = p_outVariableNodes.at(ifElseBlock->getLast());
    detectChange(p_outVariableNodes, ifElseBlock, outVariableNodes);
    p_outVariableNodes.insert_or_assign(ifElseBlock, outVariableNodes);

    cout <<"End of IfElseBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    vector<AssignmentNode*> variableNodes = p_inVariableNodes.at(whileBlock);
    cout <<"Beginning of WhileBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(block, variableNodes);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    block->acceptVisitor(*this);

    vector<AssignmentNode*> outVariableNodes = p_outVariableNodes.at(whileBlock->getLast());
    detectChange(p_outVariableNodes, whileBlock, outVariableNodes);
    p_outVariableNodes.insert_or_assign(whileBlock, outVariableNodes);

    cout <<"End of WhileBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    vector<AssignmentNode*> variableNodes = p_inVariableNodes.at(functionDeclBlock);
    cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(functionDeclBlock->getFirst(), variableNodes);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    block->acceptVisitor(*this);

    vector<AssignmentNode*> outVariableNodes = p_outVariableNodes.at(functionDeclBlock->getLast());
    detectChange(p_outVariableNodes, functionDeclBlock, outVariableNodes);
    p_outVariableNodes.insert_or_assign(functionDeclBlock, outVariableNodes);

    cout <<"End of FunctionDeclBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    vector<AssignmentNode*> variableNodes = p_inVariableNodes.at(functionCallBlock);
    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(functionCallBlock->getFirst(), variableNodes);

    BasicBlock* block = functionCallBlock->getFirst();
    block->acceptVisitor(*this);

    block = functionCallBlock->getFnDecl();
    meet(block);
    block->acceptVisitor(*this);

    vector<AssignmentNode*> outVariableNodes = p_outVariableNodes.at(block);
    detectChange(p_outVariableNodes, functionCallBlock, outVariableNodes);
    p_outVariableNodes.insert_or_assign(functionCallBlock, outVariableNodes);

    cout <<"End of FunctionCallBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitCFG(BasicBlock* block) {
    cout <<"Beginning of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    BasicBlock* curr(block);
    int numIt = 0;
    while(p_variableNodesChanged) {
        p_variableNodesChanged = false;
        curr = block;
        numIt++;
        while(curr) {
            meet(curr);
            next = curr->getNext();
            curr->acceptVisitor(*this);
            curr = next;
        }
        cout <<"variable changed " <<p_variableNodesChanged <<endl;
    }
    cout <<"End of CFG: variableNodes size " <<p_outVariableNodes.size() <<" Iterations " << numIt <<endl <<endl;
}
