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
    map<const Variable*, vector<pair<const Definition*, bool>>> inDefinitions;
    map<const Definition*, vector<vector<const Variable*>>> inVariableGroups;
    meet(basicBlock, inVariableNodes, inDefinitions, inVariableGroups);
}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock, map<const Variable*, vector<AssignmentNode*>>& inVariableNodes,
                                      map<const Variable*, vector<pair<const Definition*, bool>>>& inDefinitions,
                                      map<const Definition*, vector<vector<const Variable*>>>& inVariableGroups) {

    const vector<BasicBlock*>& predecessors = basicBlock->getPredecessors();
    cout <<"Beginning of meet :: predecessors count " <<predecessors.size() <<endl;
    if(!predecessors.empty()) {
        auto predecessorIt = predecessors.begin();
        while(predecessorIt != predecessors.end()) {
            if(p_outVariableNodes.find(*predecessorIt) != p_outVariableNodes.end())
                break;
            predecessorIt++;
        }
        for(;predecessorIt != predecessors.end(); predecessorIt++) {

            // Copy variableNodes
            if(p_outVariableNodes.find(*predecessorIt) == p_outVariableNodes.end())
                continue;
            const map<const Variable*, vector<AssignmentNode*>>& variableNodes = p_outVariableNodes.at(*predecessorIt);
            for(auto variableNodeIt = variableNodes.begin(); variableNodeIt != variableNodes.end(); variableNodeIt++) {
                //cout << "Meet1 :: Variable " <<*(variableNodeIt->first) <<endl;

                auto inVariableNodesIt = inVariableNodes.find(variableNodeIt->first);
                if(inVariableNodesIt != inVariableNodes.end()) {
                    auto& inVariableNodes_r = inVariableNodesIt->second;
                    inVariableNodes_r.insert(inVariableNodes_r.end(),variableNodeIt->second.begin(),
                                                     variableNodeIt->second.end());
                    sort(inVariableNodes_r.begin(), inVariableNodes_r.end());
                    auto it = unique(inVariableNodes_r.begin(), inVariableNodes_r.end());
                    inVariableNodes_r.resize(distance(inVariableNodes_r.begin(), it));
                    for(auto assign : inVariableNodes_r) { cout <<"assign node " <<assign <<endl; }
                    cout << "Meet :: Variable appended " <<*(variableNodeIt->first)
                         <<" size " <<inVariableNodes_r.size()<<endl;
                }
                else {
                    cout << "Meet :: Variable added " <<*(variableNodeIt->first)<<endl;
                    inVariableNodes[variableNodeIt->first] = variableNodeIt->second;
                }
            }

            // Copy definitions
            if(p_outDefinitions.find(*predecessorIt) == p_outDefinitions.end())
                continue;
            const map<const Variable*, vector<pair<const Definition*, bool>>>& definitions = p_outDefinitions.at(*predecessorIt);
            for(auto definitionIt = definitions.begin(); definitionIt != definitions.end(); definitionIt++) {
                cout << "Meet1 :: Definition " <<*(definitionIt->first) <<endl;

                auto inDefinitionsIt = inDefinitions.find(definitionIt->first);
                if(inDefinitionsIt != inDefinitions.end()) {
                    auto& inDefinitions_r = inDefinitionsIt->second;
                    inDefinitions_r.insert(inDefinitions_r.end(),definitionIt->second.begin(),
                                                     definitionIt->second.end());
                    sort(inDefinitions_r.begin(), inDefinitions_r.end());
                    auto it = unique(inDefinitions_r.begin(), inDefinitions_r.end());
                    inDefinitions_r.resize(distance(inDefinitions_r.begin(), it));
                    for(auto assign : inDefinitions_r) { cout <<"assign  " <<assign.first <<endl; }
                    cout << "Meet :: Definition appended " <<*(definitionIt->first) <<" size " <<inDefinitions_r.size()<<endl;
                }
                else {
                    cout << "Meet :: Definition added " <<*(definitionIt->first)<<endl;
                    inDefinitions[definitionIt->first] = definitionIt->second;
                }
            }

            // Copy VariableGroups
            if(p_outVariableGroups.find(*predecessorIt) == p_outVariableGroups.end())
                continue;
            const map<const Definition*, vector<vector<const Variable*>>>& variableGroups = p_outVariableGroups.at(*predecessorIt);
            for(auto variableGroupIt = variableGroups.begin(); variableGroupIt != variableGroups.end(); variableGroupIt++) {
                cout << "Meet1 :: VariableGroup " <<*(variableGroupIt->first) <<endl;

                auto inVariableGroupIt = inVariableGroups.find(variableGroupIt->first);
                if(inVariableGroupIt != inVariableGroups.end()) {
                    auto& inVariableGroups_r = inVariableGroupIt->second;
                    inVariableGroups_r.insert(inVariableGroups_r.end(),variableGroupIt->second.begin(),
                                                     variableGroupIt->second.end());
                    sort(inVariableGroups_r.begin(), inVariableGroups_r.end());
                    auto it = unique(inVariableGroups_r.begin(), inVariableGroups_r.end());
                    inVariableGroups_r.resize(distance(inVariableGroups_r.begin(), it));
                    for(const auto& variableGroup : inVariableGroups_r) { cout <<"assign  " <<variableGroup.size() <<endl; }
                    cout << "Meet :: VariableGroup appended " <<*(inVariableGroupIt->first) <<" size " <<inVariableGroups_r.size()<<endl;
                }
                else {
                    cout << "Meet :: VariableGroup added " <<*(variableGroupIt->first)
                         <<" " <<variableGroupIt->second.size() <<endl;
                    if(variableGroupIt->second.size() > 0) {
                        cout << "first " <<variableGroupIt->second[0].size() <<endl;
                        for(const Variable* var : variableGroupIt->second[0]) {
                            cout <<*var <<", ";
                        }
                        cout <<endl;
                    }
                    inVariableGroups[variableGroupIt->first] = variableGroupIt->second;
                }
            }
        }
    }
    detectChange(p_inVariableNodes, basicBlock, inVariableNodes);
    p_inVariableNodes.erase(basicBlock);
    p_inVariableNodes[basicBlock] = inVariableNodes;

    p_inDefinitions.erase(basicBlock);
    p_inDefinitions[basicBlock] = inDefinitions;

    p_inVariableGroups.erase(basicBlock);
    p_inVariableGroups[basicBlock] = inVariableGroups;
    cout <<"End of meet variableNodes size " <<inVariableNodes.size() << " definitions size " <<inDefinitions.size() <<endl;
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
                cout <<"end of detectchange " <<*variable <<endl;
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
                    cout <<"end of detectchange " <<*variable <<endl;
                    return;
                }
            }
            if(!oldVariableNodes.empty()) {
                p_variableNodesChanged = true;
                cout <<"end of detectchange " <<*variable <<endl;
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
    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_inDefinitions.at(basicBlock);
    map<const Definition*, vector<vector<const Variable*>>> outVariableGroups = p_inVariableGroups.at(basicBlock);
    cout <<"Beginning of Block: variableNodes size " <<outVariableNodes.size() <<" " << outDefinitions.size()<<endl <<endl;

    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        if(node->type() != ASSIGNMENT) continue;
        vector<const Expr*> RHSVariables;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        const Expr* value=assignNode->getValue();
        if(value) {
            value->getRHSVariables(RHSVariables);
        }
        else {
            continue;
        }
        cout <<"RHS value " << *value <<" size " <<RHSVariables.size()<<" lhs var " <<assignNode->getVariable() <<endl;

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
            const Variable* var = dynamic_cast<const Variable*>(*variableIt);
            if (var==nullptr) { cout <<"var null cast error " <<endl; continue; }
            cout <<"LHS variable " << **variableIt <<" LHS Var size " << LHSVariables.size()
                << " RHS Var size "<<RHSVariables.size() <<endl;
            visitBasicBlockHelper(var, value, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
        }
    }
    detectChange(p_outVariableNodes, basicBlock, outVariableNodes);
    p_outVariableNodes.erase(basicBlock);
    p_outVariableNodes[basicBlock] = outVariableNodes;
    p_outDefinitions.erase(basicBlock);
    p_outDefinitions[basicBlock] = outDefinitions;
    p_outVariableGroups.erase(basicBlock);
    p_outVariableGroups[basicBlock] = outVariableGroups;
    cout <<"End of Block: variableNodes size " <<outVariableNodes.size() <<" basicBlock " <<basicBlock <<
           "definitions size " <<outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(ifElseBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(ifElseBlock);
    map<const Definition*, vector<vector<const Variable*>>> variableGroups = p_inVariableGroups.at(ifElseBlock);
    cout <<"Beginning of IfElseBlock: variableNodes size " <<variableNodes.size() << " definitions size "
        <<definitions.size() <<endl <<endl;
    meet(ifElseBlock->getIfFirst(), variableNodes, definitions, variableGroups);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            cout <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        if(block->getType() == JUMPBLOCK) {
            cout <<"Unreachable code following jump " <<endl;
            break;
        }
        meet(block);
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        meet(ifElseBlock->getElseFirst(), variableNodes, definitions, variableGroups);
        block = ifElseBlock->getElseFirst();
        lastBlock = ifElseBlock->getElseLast();
        while(block != lastBlock) {
            if(block->getType() == JUMPBLOCK) {
                cout <<"Unreachable code following jump " <<endl;
                break;
            }
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

    const map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions = p_outDefinitions.at(ifElseBlock->getLast());
    p_outDefinitions.erase(ifElseBlock);
    p_outDefinitions[ifElseBlock] = outDefinitions;

    const map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups = p_outVariableGroups.at(ifElseBlock->getLast());
    p_outVariableGroups.erase(ifElseBlock);
    p_outVariableGroups[ifElseBlock] = outVariableGroups;

    cout <<"End of IfElseBlock: variableNodes size " <<outVariableNodes.size() <<" definitions size " <<outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(whileBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(whileBlock);
    map<const Definition*, vector<vector<const Variable*>>> variableGroups = p_inVariableGroups.at(whileBlock);
    cout <<"Beginning of WhileBlock: variableNodes size " <<variableNodes.size() <<" definitions size " << definitions.size() <<endl <<endl;
    meet(block, variableNodes, definitions, variableGroups);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            cout <<"Unreachable code following jump " <<endl;
            break;
        }
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

    const map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions = p_outDefinitions.at(whileBlock->getLast());
    p_outDefinitions.erase(whileBlock);
    p_outDefinitions[whileBlock] = outDefinitions;

    const map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups = p_outVariableGroups.at(whileBlock->getLast());
    p_outVariableGroups.erase(whileBlock);
    p_outVariableGroups[whileBlock] = outVariableGroups;

    cout <<"End of WhileBlock: variableNodes size " <<outVariableNodes.size() <<" " << definitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitForBlock(ForBlock* forBlock) {
    BasicBlock* block = forBlock->getFirst();
    BasicBlock* lastBlock = forBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(forBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(forBlock);
    map<const Definition*, vector<vector<const Variable*>>> variableGroups = p_inVariableGroups.at(forBlock);
    cout <<"Beginning of ForBlock: variableNodes size " <<variableNodes.size() <<" " << definitions.size() <<endl <<endl;
    meet(block, variableNodes, definitions, variableGroups);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            cout <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    block->acceptVisitor(*this);

    const map<const Variable*, vector<AssignmentNode*>>& outVariableNodes = p_outVariableNodes.at(forBlock->getLast());
    detectChange(p_outVariableNodes, forBlock, outVariableNodes);
    p_outVariableNodes.erase(forBlock);
    p_outVariableNodes[forBlock] = outVariableNodes;

    const map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions = p_outDefinitions.at(forBlock->getLast());
    p_outDefinitions.erase(forBlock);
    p_outDefinitions[forBlock] = outDefinitions;

    const map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups = p_outVariableGroups.at(forBlock->getLast());
    p_outVariableGroups.erase(forBlock);
    p_outVariableGroups[forBlock] = outVariableGroups;

    cout <<"End of forBlock: variableNodes size " <<outVariableNodes.size() <<" definitions size " << outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionDeclBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(functionDeclBlock);
    map<const Definition*, vector<vector<const Variable*>>> variableGroups = p_inVariableGroups.at(functionDeclBlock);

    cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<variableNodes.size() <<" definitions size "
        << definitions.size() <<endl <<endl;
    meet(functionDeclBlock->getFirst(), variableNodes, definitions, variableGroups);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            cout <<"Unreachable code following jump " <<endl;
            break;
        }
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

    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_outDefinitions.at(functionDeclBlock->getLast());
    p_outDefinitions.erase(functionDeclBlock);
    p_outDefinitions[functionDeclBlock] = outDefinitions;

    const map<const Definition*, vector<vector<const Variable*>>> outVariableGroups = p_outVariableGroups.at(functionDeclBlock->getLast());
    p_outVariableGroups.erase(functionDeclBlock);
    p_outVariableGroups[functionDeclBlock] = outVariableGroups;

    cout <<"End of FunctionDeclBlock: variableNodes size " <<outVariableNodes.size() <<" definitions size "
        <<outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionCallBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(functionCallBlock);
    map<const Definition*, vector<vector<const Variable*>>> variableGroups = p_inVariableGroups.at(functionCallBlock);

    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<variableNodes.size() <<" definitions size "
        << definitions.size() <<endl <<endl;
    vector<FunctionCallInstance*>& functionCallInstances = functionCallBlock->getFnCallInstances();
    for(int i=0; i<functionCallInstances.size(); i++) {
        FunctionCallInstance* fnCallInstance = functionCallInstances[i];
        meet(fnCallInstance->getFirst(), variableNodes, definitions, variableGroups);

        BasicBlock* block = fnCallInstance->getFirst();
        block->acceptVisitor(*this);

        block = fnCallInstance->getFnDecl();
        meet(block);
        block->acceptVisitor(*this);

        block = fnCallInstance->getLast();
        meet(block);
        block->acceptVisitor(*this);
    }

    meet(functionCallBlock->getLast());
    functionCallBlock->getLast()->acceptVisitor(*this);
    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_outVariableNodes.at(functionCallBlock->getLast());
    detectChange(p_outVariableNodes, functionCallBlock, outVariableNodes);
    p_outVariableNodes.erase(functionCallBlock);
    p_outVariableNodes[functionCallBlock] = outVariableNodes;

    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_outDefinitions.at(functionCallBlock->getLast());
    p_outDefinitions.erase(functionCallBlock);
    p_outDefinitions[functionCallBlock] = outDefinitions;

    const map<const Definition*, vector<vector<const Variable*>>> outVariableGroups = p_outVariableGroups.at(functionCallBlock->getLast());
    p_outVariableGroups.erase(functionCallBlock);
    p_outVariableGroups[functionCallBlock] = outVariableGroups;

    cout <<"End of FunctionCallBlock: variableNodes size " <<outVariableNodes.size() <<" definitions size "
        << outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitCFG(BasicBlock* block) {
    cout <<"Beginning of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    BasicBlock* curr(block);
    int numIt = 0;
    //while(p_variableNodesChanged) {
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
    //}
    cout <<"End of CFG: variableNodes size " <<p_outVariableNodes.size() << " definitions size "
        << p_outDefinitions.size()
        << " Iterations " << numIt <<endl <<endl;
}
