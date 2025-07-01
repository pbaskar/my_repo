#include "ComputeReachingDefsVisitor.h"
#include "BasicBlock.h"
#include <cassert>
#include <algorithm>

ComputeReachingDefsVisitor::ComputeReachingDefsVisitor() {

}

ComputeReachingDefsVisitor::~ComputeReachingDefsVisitor() {

}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock) {
    map<const Variable*, vector<AssignmentNode*>> inVariableNodes;
    meet(basicBlock, inVariableNodes);
}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock, map<const Variable*, vector<AssignmentNode*>>& inVariableNodes) {

    const vector<BasicBlock*>& predecessors = basicBlock->getPredecessors();
    cout <<"Beginning of meet :: predecessors count " <<predecessors.size() <<endl;
    if(!predecessors.empty()) {
        map<const Variable*, vector<AssignmentNode*>> intersectVariableNodes;
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
            const map<const Variable*, vector<AssignmentNode*>>& variableNodes = p_outVariableNodes.at(*predecessorIt);
            for(auto intersectVariableIt = intersectVariableNodes.begin(); intersectVariableIt != intersectVariableNodes.end(); intersectVariableIt++) {
                auto match = [=](auto variableNode) { return intersectVariableIt->first == variableNode.first; };
                if(find_if(variableNodes.begin(), variableNodes.end(), match) != variableNodes.end())
                    continue;
                intersectVariableIt = intersectVariableNodes.erase(intersectVariableIt);
            }
        }
        cout << "Meet11 :: Intersected nodes " <<intersectVariableNodes.size() <<endl;
        predecessorIt = predecessors.begin();
        while(predecessorIt != predecessors.end()) {
            if(p_outVariableNodes.find(*predecessorIt) != p_outVariableNodes.end())
                break;
            predecessorIt++;
        }
        for(;predecessorIt != predecessors.end(); predecessorIt++) {
            if(p_outVariableNodes.find(*predecessorIt) == p_outVariableNodes.end())
                continue;
            const map<const Variable*, vector<AssignmentNode*>>& variableNodes = p_outVariableNodes.at(*predecessorIt);
            for(auto variableNodeIt = variableNodes.begin(); variableNodeIt != variableNodes.end(); variableNodeIt++) {
                cout << "Meet1 :: Variable " <<variableNodeIt->first->getName() <<endl;
                auto match = [=](auto intersectVariableNode) { return variableNodeIt->first == intersectVariableNode.first; };
                if(find_if(intersectVariableNodes.begin(), intersectVariableNodes.end(), match) != intersectVariableNodes.end()) {
                    auto inVariableNodesIt = inVariableNodes.find(variableNodeIt->first);
                    if(inVariableNodesIt != inVariableNodes.end()) {
                        auto& inVariableNodes_r = inVariableNodesIt->second;
                        inVariableNodes_r.insert(inVariableNodes_r.end(),variableNodeIt->second.begin(),
                                                         variableNodeIt->second.end());
                        sort(inVariableNodes_r.begin(), inVariableNodes_r.end());
                        auto it = unique(inVariableNodes_r.begin(), inVariableNodes_r.end());
                        inVariableNodes_r.resize(distance(inVariableNodes_r.begin(), it));
                        for(auto assign : inVariableNodes_r) { cout <<"assign node " <<assign <<endl; }
                        cout << "Meet :: Variable appended " <<variableNodeIt->first->getName() << " count " <<distance(inVariableNodes_r.begin(), it) <<" size " <<inVariableNodes_r.size()<<endl;
                    }
                    else {
                        cout << "Meet :: Variable added " <<variableNodeIt->first->getName()<<endl;
                        inVariableNodes[variableNodeIt->first] = variableNodeIt->second;
                    }
                }
            }
        }
    }
    detectChange(p_inVariableNodes, basicBlock, inVariableNodes);
    p_inVariableNodes.erase(basicBlock);
    p_inVariableNodes[basicBlock] = inVariableNodes;
    cout <<"End of meet " <<inVariableNodes.size() <<endl;
}

void ComputeReachingDefsVisitor::detectChange(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& variableNodesAllBlocks,
                                              BasicBlock* basicBlock,
                                              const map<const Variable*, vector<AssignmentNode*>>& newAllVariableNodes)
{
    cout <<"Beginning of detectChange " <<endl;
    if(variableNodesAllBlocks.find(basicBlock) != variableNodesAllBlocks.end()) {
        map<const Variable*, vector<AssignmentNode*>>& oldAllVariableNodes = variableNodesAllBlocks.at(basicBlock);
        for(const auto& newAllVariableNode : newAllVariableNodes) {
            const Variable* variable = newAllVariableNode.first;
            const vector<AssignmentNode*>& newVariableNodes = newAllVariableNode.second;
            if(oldAllVariableNodes.find(variable) == oldAllVariableNodes.end()) {
                p_variableNodesChanged = true;
                cout <<"end of detectchange " <<variable->getName() <<endl;
                return;
            }
            vector<AssignmentNode*>& oldVariableNodes = oldAllVariableNodes.at(variable);
            for(const AssignmentNode* assignmentNode : newVariableNodes) {
                auto assignNodeIt = find(oldVariableNodes.begin(), oldVariableNodes.end(), assignmentNode);
                if( assignNodeIt != oldVariableNodes.end()) {
                    oldVariableNodes.erase(assignNodeIt);
                }
                else {
                    p_variableNodesChanged = true;
                    cout <<"end of detectchange " <<variable->getName() <<endl;
                    return;
                }
            }
            if(!oldVariableNodes.empty()) {
                p_variableNodesChanged = true;
                cout <<"end of detectchange " <<variable->getName() <<endl;
                return;
            }
            else
                oldAllVariableNodes.erase(variable);
        }
        if(!oldAllVariableNodes.empty())
            p_variableNodesChanged = true;
    }
    else
        p_variableNodesChanged = true;
    cout <<"End of detectChange:: variable changed " <<p_variableNodesChanged <<endl;
}

void ComputeReachingDefsVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_inVariableNodes.at(basicBlock);
    cout <<"Beginning of Block: variableNodes size " <<outVariableNodes.size() <<endl <<endl;

    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        if(node->type() != ASSIGNMENT) continue;
        vector<const Expr*> RHSVariables;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        const Expr* value=assignNode->getValue();
        if(value) {
            value->getRHSVariables(RHSVariables);
        }
        else continue;
        cout <<"RHS value " << *value <<RHSVariables.size()<<endl;

        //Save variables in assignNode both LHS and RHSVariables
        vector<const Expr*> LHSIdentifiers;
        vector<const Expr*> LHSVariables;
        value->getLHS(LHSIdentifiers);
        for(int i=0; i<LHSIdentifiers.size(); i++) {
            const Identifier* identifier = static_cast<const Identifier*>(LHSIdentifiers[i]);
            identifier->getLHSOnLeft(LHSVariables);
        }
        for(int i=0; i<LHSIdentifiers.size(); i++) {
            const Identifier* identifier = static_cast<const Identifier*>(LHSIdentifiers[i]);
            identifier->getRHSOnLeft(RHSVariables);
        }
        const Variable* var = assignNode->getVariable();
        if(var) LHSVariables.push_back(var);

        for(auto variableIt = LHSVariables.begin(); variableIt != LHSVariables.end(); variableIt++) {
            //cout <<"LHS variable ptr " <<*variableIt <<" " << LHSVariables.size() << " "<<RHSVariables.size() <<endl;
            const Variable* var = dynamic_cast<const Variable*>(*variableIt);
            if (var==nullptr) { cout <<"cast error " <<endl; continue; }

            auto variableNodeIt = outVariableNodes.find(var);
            if(variableNodeIt != outVariableNodes.end()) {
                auto& nodes = outVariableNodes.at(var);
                nodes.clear();
                nodes.push_back(assignNode);
                cout<<" replaced var " <<* var << " " <<*assignNode << " " <<assignNode <<endl;
            }
            else {
                vector<AssignmentNode*> v;
                v.push_back(assignNode);
                cout<<" added var " <<*var << " " << *assignNode << " " <<assignNode <<endl;
                outVariableNodes.insert_or_assign(var, v);
            }
           if(var->getExprType() == ExprType::POINTERVARIABLE) {
                const Variable* rhs = dynamic_cast<const Variable*>(RHSVariables[0]);
                if(rhs==nullptr) { cout << "pointer assignment rhs null error " <<endl; break;}
                const Variable* pointsToLHS = var;
                const Variable* pointsToRHS = rhs;

                while(true) {
                    const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(pointsToLHS);
                    if(lhsPointer == nullptr) break;
                    const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(pointsToRHS);
                    if(rhsPointer == nullptr) break;
                    pointsToLHS = lhsPointer->getPointsTo();
                    pointsToRHS = rhsPointer->getPointsTo();
                    assert(pointsToLHS != nullptr);
                    assert(pointsToRHS != nullptr);

                    vector<AssignmentNode*> rhsNodes;
                    auto pointsToRHSVariableNodeIt = outVariableNodes.find(pointsToRHS);
                    if(pointsToRHSVariableNodeIt != outVariableNodes.end()) {
                        rhsNodes = outVariableNodes.at(pointsToRHS);
                        cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode << " " <<assignNode << rhsNodes.size()<<endl;
                    }
                    else {
                        cout <<"pointsTo var not changed " <<*pointsToLHS << " " << *assignNode << " " <<assignNode <<endl;
                    }

                    //copy assignment nodes from pointsToRHS to pointsToLHS
                    auto pointsToVariableNodeIt = outVariableNodes.find(pointsToLHS);
                    if(pointsToVariableNodeIt != outVariableNodes.end()) {
                        auto& nodes = outVariableNodes.at(pointsToLHS);
                        nodes.clear();
                        nodes = /*std::move*/(rhsNodes);
                        cout<<" replaced pointsto var " <<* pointsToLHS << " " <<*assignNode << " " <<assignNode <<endl;
                    }
                    else {
                        cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode << " " <<assignNode << rhsNodes.size()<<endl;
                        outVariableNodes.insert_or_assign(pointsToLHS, rhsNodes);
                    }
                }
            }
        }
    }
    detectChange(p_outVariableNodes, basicBlock, outVariableNodes);
    p_outVariableNodes.erase(basicBlock);
    p_outVariableNodes[basicBlock] = outVariableNodes;
    cout <<"End of Block: variableNodes size " <<outVariableNodes.size() <<" " <<basicBlock <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(ifElseBlock);
    cout <<"Beginning of IfElseBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(ifElseBlock->getIfFirst(), variableNodes);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        meet(ifElseBlock->getElseFirst(), variableNodes);
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
    }

    meet(ifElseBlock->getLast());
    ifElseBlock->getLast()->acceptVisitor(*this);
    const map<const Variable*, vector<AssignmentNode*>>& outVariableNodes = p_outVariableNodes.at(ifElseBlock->getLast());
    detectChange(p_outVariableNodes, ifElseBlock, outVariableNodes);
    p_outVariableNodes.erase(ifElseBlock);
    p_outVariableNodes[ifElseBlock] = outVariableNodes;

    cout <<"End of IfElseBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(whileBlock);
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

    const map<const Variable*, vector<AssignmentNode*>>& outVariableNodes = p_outVariableNodes.at(whileBlock->getLast());
    detectChange(p_outVariableNodes, whileBlock, outVariableNodes);
    p_outVariableNodes.erase(whileBlock);
    p_outVariableNodes[whileBlock] = outVariableNodes;

    cout <<"End of WhileBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionDeclBlock);
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

    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_outVariableNodes.at(functionDeclBlock->getLast());
    detectChange(p_outVariableNodes, functionDeclBlock, outVariableNodes);
    p_outVariableNodes.erase(functionDeclBlock);
    p_outVariableNodes[functionDeclBlock] = outVariableNodes;

    cout <<"End of FunctionDeclBlock: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionCallBlock);
    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<variableNodes.size() <<endl <<endl;
    meet(functionCallBlock->getFirst(), variableNodes);

    BasicBlock* block = functionCallBlock->getFirst();
    block->acceptVisitor(*this);

    block = functionCallBlock->getFnDecl();
    meet(block);
    block->acceptVisitor(*this);

    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_outVariableNodes.at(block);
    detectChange(p_outVariableNodes, functionCallBlock, outVariableNodes);
    p_outVariableNodes.erase(functionCallBlock);
    p_outVariableNodes[functionCallBlock] = outVariableNodes;

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
        //if(numIt >=2 ) break;
    }
    cout <<"End of CFG: variableNodes size " <<p_outVariableNodes.size() <<" Iterations " << numIt <<endl <<endl;
}
