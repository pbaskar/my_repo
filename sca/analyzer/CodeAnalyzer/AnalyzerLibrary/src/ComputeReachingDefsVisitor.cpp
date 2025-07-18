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
    meet(basicBlock, inVariableNodes, inDefinitions);
}

void ComputeReachingDefsVisitor::meet(BasicBlock* basicBlock, map<const Variable*, vector<AssignmentNode*>>& inVariableNodes,
                                      map<const Variable*, vector<pair<const Definition*, bool>>>& inDefinitions) {

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
                cout << "Meet1 :: Variable " <<*(variableNodeIt->first) <<endl;

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
        }
    }
    detectChange(p_inVariableNodes, basicBlock, inVariableNodes);
    p_inVariableNodes.erase(basicBlock);
    p_inVariableNodes[basicBlock] = inVariableNodes;

    p_inDefinitions.erase(basicBlock);
    p_inDefinitions[basicBlock] = inDefinitions;
    cout <<"End of meet " <<inVariableNodes.size() << " " <<inDefinitions.size() <<endl;
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
        cout <<"RHS value " << *value <<" size " <<RHSVariables.size()<<endl;

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
            assert(*variableIt != nullptr);
            cout <<"LHS variable " << **variableIt <<" LHS Var size " << LHSVariables.size()
                << " RHS Var size "<<RHSVariables.size() <<endl;
            const Variable* var = dynamic_cast<const Variable*>(*variableIt);
            if (var==nullptr) { cout <<"var null cast error " <<endl; continue; }

            if(var->getExprType() == ExprType::VARIABLE) {
                //replace with or add current assignNode for Variable
                auto variableNodeIt = outVariableNodes.find(var);
                if(variableNodeIt != outVariableNodes.end()) {
                    auto& nodes = outVariableNodes.at(var);
                    cout <<"nodes size before clear " << nodes.size() <<endl;
                    nodes.clear();
                    nodes.push_back(assignNode);
                    cout<<" replaced var " <<*var << " " <<*assignNode <<endl;
                }
                else {
                    vector<AssignmentNode*> v;
                    v.push_back(assignNode);
                    cout<<" added var " <<*var << " " << *assignNode <<endl;
                    outVariableNodes.insert_or_assign(var, v);
                }
            }
            else if(var->getExprType() == ExprType::POINTERVARIABLE || value->getExprType()==ExprType::DELETEFNCALL) {
                vector<vector<pair<const Definition*,bool>>> pointsToRhsDefinitions;
                vector<vector<AssignmentNode*>> pointsToAssignNodes;
                switch(value->getExprType()) {
                   case ExprType::DELETEFNCALL: {
                       if(RHSVariables.empty()) { cout <<"RHS variables empty"; break; }
                       const Expr* rhsExpr = RHSVariables[0];
                       const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                       if(rhs==nullptr) { cout << "pointer assignment rhs cast error " <<endl; break; }
                       auto definitionIt = outDefinitions.find(rhs);
                       if(definitionIt != outDefinitions.end()) {
                           auto deletedDefinitionPairs = outDefinitions.at(rhs);

                           // set definition invalid for all variables pointing to the deleted definition
                           for(auto it = outDefinitions.begin(); it != outDefinitions.end();  it++) {
                               auto& definitionPairs = it->second;
                               for(auto itDef =definitionPairs.begin(); itDef!= definitionPairs.end(); itDef++) {
                                   auto match = [=](auto deletedDefinitionPair) { return itDef->first == deletedDefinitionPair.first; };
                                   auto matched = find_if(deletedDefinitionPairs.begin(), deletedDefinitionPairs.end(), match);
                                   if(matched != deletedDefinitionPairs.end()) {
                                       itDef->second = false;
                                       cout <<"delete : set false to " <<itDef->second <<endl;
                                   }
                               }
                           }
                       }
                       else {
                           assert(false);
                       }
                    }
                   break;
                   case ExprType::MALLOCFNCALL: {
                            const MallocFnCall* mallocFnCall = static_cast<const MallocFnCall*>(value);
                            const Definition* rhsDefinition = mallocFnCall->getDefinition();

                            const vector<const Definition*>& pointsToDefinitions = rhsDefinition->getPointsToDefinitions();
                            for(int i=0; i<pointsToDefinitions.size()-1; i++) {
                                const Definition* pointsToDefinition = pointsToDefinitions[i];

                                pair<const Definition*, bool> pointsToDefinitionPair(pointsToDefinition,pointsToDefinition->isValid());
                                vector<pair<const Definition*, bool>> vp;
                                vp.push_back(pointsToDefinitionPair);
                                pointsToRhsDefinitions.push_back(vp);

                                vector<AssignmentNode*> v;
                                v.push_back(assignNode);
                                pointsToAssignNodes.push_back(v);
                            }
                            vector<AssignmentNode*> v;
                            v.push_back(assignNode);
                            pointsToAssignNodes.push_back(v);
                        }
                   break;
                   case ExprType::ARRAYDEFINITION:
                   case ExprType::DEFINITION: {
                        const Definition* rhsDefinition = static_cast<const Definition*>(value);
                        const vector<const Definition*>& pointsToDefinitions = rhsDefinition->getPointsToDefinitions();
                        for(int i=0; i<pointsToDefinitions.size()-1; i++) {
                            const Definition* pointsToDefinition = pointsToDefinitions[i];
                            pair<const Definition*, bool> pointsToDefinitionPair(pointsToDefinition,pointsToDefinition->isValid());
                            vector<pair<const Definition*, bool>> vp;
                            vp.push_back(pointsToDefinitionPair);
                            pointsToRhsDefinitions.push_back(vp);

                            vector<AssignmentNode*> v;
                            v.push_back(assignNode);
                            pointsToAssignNodes.push_back(v);
                        }
                        vector<AssignmentNode*> v;
                        v.push_back(assignNode);
                        pointsToAssignNodes.push_back(v);
                   }
                   break;
                   case ExprType::ADDRESSOFOPERATOR: {
                       if(RHSVariables.empty()) { cout <<"RHS variables empty"; break; }
                       const Expr* rhsExpr = RHSVariables[0];
                       assert(rhsExpr != nullptr);
                       const AddressOfVariable* rhs = dynamic_cast<const AddressOfVariable*>(rhsExpr);
                       if(rhs != nullptr) {
                           const Definition* rhsDefinition = rhs->getDefinition();
                           pair<const Definition*, bool> p(rhsDefinition, rhsDefinition->isValid());

                           auto definitionIt = outDefinitions.find(rhs);
                           if(definitionIt != outDefinitions.end()) {
                               auto& definitions = outDefinitions.at(rhs);
                               definitions.clear();
                               definitions.push_back(p);
                               cout<<" replaced definition of rhs " <<*rhs << " rhs definition valid " <<*rhsDefinition
                                  <<*assignNode <<endl;
                           }
                           else {
                               vector<pair<const Definition*, bool>> v;
                               v.push_back(p);
                               cout<<" added definition rhs " <<*rhs << " assignNode ptr " << *assignNode
                                  << " outDefinitions size " << outDefinitions.size()<<endl;
                               outDefinitions.insert_or_assign(rhs, v);
                           }
                       }
                   } //Fallthrough
                   //PointerVariable, Dereference Operator, AddressOfOperator
                   default: {
                       if(RHSVariables.empty()) { cout <<"RHS variables empty"; break; }
                       cout <<"rhs pointer variable " <<endl;
                       const Expr* rhsExpr = RHSVariables[0];
                       const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                       if(rhs==nullptr) { cout << "pointer assignment rhs cast error " <<endl; break; }
                        const Variable* pointsToRHS = rhs;

                        while(true) {
                            const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(pointsToRHS);
                            if(rhsPointer == nullptr) break;

                            //Only for Pointer variables ptr_ptr_p, ptr_p
                            vector<pair<const Definition*, bool>> rhsDefinitions;
                            auto rhsDefinitionIt = outDefinitions.find(rhsPointer);
                            if(rhsDefinitionIt != outDefinitions.end()) {
                                rhsDefinitions = outDefinitions.at(rhsPointer);
                                pointsToRhsDefinitions.push_back(rhsDefinitions);
                                cout<<" found rhsDefinitions RHS pointer " << *rhsPointer <<
                                      " rhsDefinitions size " << rhsDefinitions.size()<<endl;
                            }
                            else {
                                cout <<"not found rhsDefinitions RHS pointer" << *rhsPointer <<endl;
                            }

                            // for ptr_ptr_p and ptr_p
                            vector<AssignmentNode*> rhsNodes;
                            auto pointsToRHSVariableNodeIt = outVariableNodes.find(pointsToRHS);
                            if(pointsToRHSVariableNodeIt != outVariableNodes.end()) {
                                rhsNodes = outVariableNodes.at(pointsToRHS);
                                pointsToAssignNodes.push_back(rhsNodes);
                                cout<<" found rhsNodes " <<*assignNode <<" rhsNodes size " << rhsNodes.size()<<endl;
                            }
                            else {
                                cout <<"not found rhsNodes " << *assignNode << endl;

                            }
                            pointsToRHS = rhsPointer->getPointsTo();
                            assert(pointsToRHS != nullptr);
                        }
                        //pointedto variable
                        vector<AssignmentNode*> rhsNodes;
                        auto pointsToRHSVariableNodeIt = outVariableNodes.find(pointsToRHS);
                        if(pointsToRHSVariableNodeIt != outVariableNodes.end()) {
                            rhsNodes = outVariableNodes.at(pointsToRHS);
                            pointsToAssignNodes.push_back(rhsNodes);
                            cout<<" found rhsNodes " <<*assignNode <<" rhsNodes size " << rhsNodes.size()<<endl;
                        }
                        else {
                            cout <<"not found rhsNodes " << *assignNode << endl;
                        }
                    }
               } //switch end
               const Variable* pointsToLHS = var;
               int i=0;
               cout <<"definitions size " << pointsToRhsDefinitions.size() <<"  assign nodes size " <<pointsToAssignNodes.size() <<endl;
               while(true) {
                   const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(pointsToLHS);
                   if(lhsPointer == nullptr) break;
                   if(i >= pointsToRhsDefinitions.size() || i>= pointsToAssignNodes.size()) {
                       cout <<"LHS pointer has different levels of indirection than RHS "<< *assignNode <<endl; break;
                   }
                   //find malloc definition for RHS and copy to LHS
                   //for ptr_ptr_p ptr_p
                   auto definitionIt = outDefinitions.find(lhsPointer);
                   if(definitionIt != outDefinitions.end()) {
                       auto& definitions = outDefinitions.at(lhsPointer);
                       definitions.clear();
                       definitions = /*std::move*/(pointsToRhsDefinitions[i]);
                       cout<<" replaced definition of lhs " <<*lhsPointer <<
                          " rhsDefinitions size " <<definitions.size() <<endl;
                   }
                   else {
                       cout<<" added definition lhs " <<*lhsPointer << " outDefinitions size " << outDefinitions.size()<<endl;
                       outDefinitions.insert_or_assign(lhsPointer, pointsToRhsDefinitions[i]);
                   }

                   //copy assignment nodes from pointsToRHS to pointsToLHS
                   //for ptr_ptr_p ptr_p
                   auto pointsToVariableNodeIt = outVariableNodes.find(pointsToLHS);
                   if(pointsToVariableNodeIt != outVariableNodes.end()) {
                       auto& nodes = outVariableNodes.at(pointsToLHS);
                       nodes.clear();
                       nodes = /*std::move*/(pointsToAssignNodes[i]);
                       cout<<" replaced pointsto var " <<* pointsToLHS << " " <<*assignNode
                          << " rhsNodes size " <<nodes.size() <<endl;
                   }
                   else {
                       cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode << " rhsNodes size " <<pointsToAssignNodes[i].size()<<endl;
                       outVariableNodes.insert_or_assign(pointsToLHS, pointsToAssignNodes[i]);
                   }
                   pointsToLHS = lhsPointer->getPointsTo();
                   assert(pointsToLHS != nullptr);
                   i++;
               }
               if(i>= pointsToAssignNodes.size()) {
                   cout <<"LHS pointer has different levels of indirection than RHS "<< *assignNode <<
                          "rhs definitions size " <<pointsToRhsDefinitions.size() <<
                          "assign nodes size " <<pointsToAssignNodes.size() <<" i " <<i<<endl; break;
               }
               //PointedTo Variable
               //copy assignment nodes from pointsToRHS to pointsToLHS
               auto pointsToVariableNodeIt = outVariableNodes.find(pointsToLHS);
               if(pointsToVariableNodeIt != outVariableNodes.end()) {
                   auto& nodes = outVariableNodes.at(pointsToLHS);
                   nodes.clear();
                   nodes = /*std::move*/(pointsToAssignNodes[i]);
                   cout<<" replaced pointsto var " <<* pointsToLHS << " " <<*assignNode
                      << " rhsNodes size " <<pointsToAssignNodes[i].size() <<endl;
               }
               else {
                   cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode << " rhsNodes size " <<pointsToAssignNodes[i].size()<<endl;
                   outVariableNodes.insert_or_assign(pointsToLHS, pointsToAssignNodes[i]);
               }

            }
        }
    }
    detectChange(p_outVariableNodes, basicBlock, outVariableNodes);
    p_outVariableNodes.erase(basicBlock);
    p_outVariableNodes[basicBlock] = outVariableNodes;
    p_outDefinitions.erase(basicBlock);
    p_outDefinitions[basicBlock] = outDefinitions;
    cout <<"End of Block: variableNodes size " <<outVariableNodes.size() <<" " <<basicBlock <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(ifElseBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(ifElseBlock);
    cout <<"Beginning of IfElseBlock: variableNodes size " <<variableNodes.size() << " " <<definitions.size() <<endl <<endl;
    meet(ifElseBlock->getIfFirst(), variableNodes, definitions);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
        assert(block != nullptr);
        meet(block);
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        meet(ifElseBlock->getElseFirst(), variableNodes, definitions);
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

    const map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions = p_outDefinitions.at(ifElseBlock->getLast());
    p_outDefinitions.erase(ifElseBlock);
    p_outDefinitions[ifElseBlock] = outDefinitions;

    cout <<"End of IfElseBlock: variableNodes size " <<outVariableNodes.size() <<" " <<outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(whileBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(whileBlock);
    cout <<"Beginning of WhileBlock: variableNodes size " <<variableNodes.size() <<" " << definitions.size() <<endl <<endl;
    meet(block, variableNodes, definitions);

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

    const map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions = p_outDefinitions.at(whileBlock->getLast());
    p_outDefinitions.erase(whileBlock);
    p_outDefinitions[whileBlock] = outDefinitions;

    cout <<"End of WhileBlock: variableNodes size " <<outVariableNodes.size() <<" " << outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionDeclBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(functionDeclBlock);
    cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<variableNodes.size() <<" " << definitions.size() <<endl <<endl;
    meet(functionDeclBlock->getFirst(), variableNodes, definitions);

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

    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_outDefinitions.at(functionDeclBlock->getLast());
    p_outDefinitions.erase(functionDeclBlock);
    p_outDefinitions[functionDeclBlock] = outDefinitions;

    cout <<"End of FunctionDeclBlock: variableNodes size " <<outVariableNodes.size() <<" " <<outDefinitions.size() <<endl <<endl;
}

void ComputeReachingDefsVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(functionCallBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> definitions = p_inDefinitions.at(functionCallBlock);
    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<variableNodes.size() <<" " << definitions.size() <<endl <<endl;
    vector<FunctionCallInstance*>& functionCallInstances = functionCallBlock->getFnCallInstances();
    for(int i=0; i<functionCallInstances.size(); i++) {
        FunctionCallInstance* fnCallInstance = functionCallInstances[i];
        meet(fnCallInstance->getFirst(), variableNodes, definitions);

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

    cout <<"End of FunctionCallBlock: variableNodes size " <<outVariableNodes.size() <<" " << outDefinitions.size() <<endl <<endl;
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
    cout <<"End of CFG: variableNodes size " <<p_outVariableNodes.size() << " " << p_outDefinitions.size()
        << " Iterations " << numIt <<endl <<endl;
}
