/*
 * Visitor.cpp
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#include <cassert>
#include<cstring>
#include<vector>
#include<map>

#include <algorithm>
#include "Visitor.h"
#include "BasicBlock.h"

Visitor::Visitor() {
    // TODO Auto-generated constructor stub

}

Visitor::~Visitor() {
    // TODO Auto-generated destructor stub
}

PrintVisitor::PrintVisitor() {

}

PrintVisitor::~PrintVisitor() {

}

void PrintVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    basicBlock->print();
}

void PrintVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    ifElseBlock->print();
}

void PrintVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    whileBlock->print();
}

void PrintVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    functionDeclBlock->print();
}

void PrintVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    functionCallBlock->print();
}

DeleteVisitor::DeleteVisitor() {

}

DeleteVisitor::~DeleteVisitor() {

}

void DeleteVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    delete basicBlock;
}

void DeleteVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    delete ifElseBlock;
}

void DeleteVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    delete whileBlock;
}

void DeleteVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    delete functionDeclBlock;
}

void DeleteVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    delete functionCallBlock;
}

VariableInitCheckVisitor::VariableInitCheckVisitor(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& inVariableNodes,
                                                   map<BasicBlock*, map<const Variable*, vector<pair<const Definition*, bool>>>>& inDefinitions) {
    p_inVariableNodes = std::move(inVariableNodes);
    p_inDefinitions = std::move(inDefinitions);
}

VariableInitCheckVisitor::~VariableInitCheckVisitor() {

}

void VariableInitCheckVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_inVariableNodes.at(basicBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_inDefinitions.at(basicBlock);

    cout <<"Beginning of Block: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
    bool found = false;
    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        if(node->type() != ASSIGNMENT) continue;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        const Expr* value=node->getValue();
        vector<const Expr*> RHSVariables;
        if(value) {
            value->getRHSVariables(RHSVariables);
        }
        else continue;

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
        if(var) {
            LHSVariables.push_back(var);
            if(value->getExprType() == ExprType::DEFINITION) {
                const Definition* def = static_cast<const Definition*>(value);
                if(!def->isValid()) {
                    const char* m = "Not defined ";
                    const char* name = var->getName();
                    Result r;
                    r.errorMessage = new char[strlen(m)+strlen(name)+1];
                    r.errorMessage[0] = '\0';
                    strncat(r.errorMessage,m,strlen(m));
                    strncat(r.errorMessage,name,strlen(name));
                    r.errorMessage[strlen(m)+strlen(name)] = '\0';
                    cout << "Error message: Declaration " <<r.errorMessage << " " <<*var <<endl;
                    p_results.push_back(r);
                }
            }
        }

        cout <<"RHS value " << *value <<" variables count " <<RHSVariables.size() <<endl;


        //Check nodes and definitions for invalid assignments and definitions
        for(auto variableIt = RHSVariables.begin(); variableIt != RHSVariables.end(); variableIt++) {
            const Variable* variable = dynamic_cast<const Variable*>(*variableIt);
            if (!variable) { cout <<"RHS cast error " <<variable <<endl; continue; }
            else cout <<"rhs variable " << *variable <<endl;
            found = true;

            map<const Variable*, vector<AssignmentNode*>>::iterator nodesIt = outVariableNodes.find(variable);
            if(nodesIt != outVariableNodes.end()) {
                const vector<AssignmentNode*>& nodes= nodesIt->second;
                if(!nodes.empty()) {
                    for(int i=0; i<nodes.size(); i++) {
                        const Expr* nodeValue = nodes[i]->getValue();
                        if(nodeValue->getExprType() == ExprType::DEFINITION ||
                                nodeValue->getExprType() == ExprType::ARRAYDEFINITION) {
                            const Definition* def = static_cast<const Definition*>(nodeValue);
                            if(!def->isValid()) {
                                cout <<"Invalid assignment: var " <<*variable<< " value = " <<*value <<endl;
                                found = false;
                                break;
                            }
                        }
                    }
                } else {
                    found = false;
                }
            }
            else {
                found = false;
            }
            map<const Variable*, vector<pair<const Definition*, bool>>>::iterator outDefinitionIt = outDefinitions.find(variable);
            if(outDefinitionIt != outDefinitions.end()) {
                const vector<pair<const Definition*, bool>>& definitions= outDefinitionIt->second;
                if(!definitions.empty()) {
                    for(int i=0; i<definitions.size(); i++) {
                        if(!(definitions[i].second)) {
                            cout <<"Invalid definition: var " <<*variable<< " value = " <<*value <<endl;
                            found = false;
                            break;
                        }
                        cout <<"definitions for var " <<*variable << " " <<definitions[i].first<<" " <<definitions[i].second <<endl;
                    }
                }
                else {
                    found = false;
                }
            }
            if(!found) {
                const char* m = "Not defined ";
                const char* name = variable->getName();
                Result r;
                r.errorMessage = new char[strlen(m)+strlen(name)+1];
                r.errorMessage[0] = '\0';
                strncat(r.errorMessage,m,strlen(m));
                strncat(r.errorMessage,name,strlen(name));
                r.errorMessage[strlen(m)+strlen(name)] = '\0';
                cout << "Error message: " <<r.errorMessage << " " <<*variable <<endl;
                p_results.push_back(r);
            }
        }

        // compute result after each node inside basic block
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
                            //one extra definition added for pointedTo variable
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
                            // for pointedto variable
                            vector<AssignmentNode*> v;
                            v.push_back(assignNode);
                            pointsToAssignNodes.push_back(v);
                        }
                   break;
                   case ExprType::ARRAYDEFINITION:
                   case ExprType::DEFINITION: {
                        const Definition* rhsDefinition = static_cast<const Definition*>(value);
                        const vector<const Definition*>& pointsToDefinitions = rhsDefinition->getPointsToDefinitions();
                        cout <<"defintions " <<pointsToDefinitions.size() <<endl;
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
                        cout <<"rhs definitions " <<pointsToDefinitions.size() << " assign nodes " <<pointsToAssignNodes.size() <<endl;
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
                          "assign nodes size " <<pointsToAssignNodes.size() <<" i " <<i <<endl; break;
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
    cout <<"End of Block: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::intersect(vector<AssignmentNode*>& dest, vector<AssignmentNode*>& source) {
    map<AssignmentNode*, int> sourceMap;
    vector<AssignmentNode*> result;
    for(auto sourceIt=source.begin(); sourceIt != source.end(); sourceIt++) {
        sourceMap.insert(pair<AssignmentNode*,int>(*sourceIt, 0));
    }
    for(auto destIt=dest.begin(); destIt != dest.end(); destIt++) {
        auto match = sourceMap.find(*destIt);
        if(match != sourceMap.end()) {
            result.push_back(*destIt);
            sourceMap.erase(match);
        }
    }
    for(auto sourceMapIt: sourceMap) {
        result.push_back(sourceMapIt.first);
    }
    dest = result;
}

void VariableInitCheckVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    cout <<"Beginning of IfElseBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);
    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        block = ifElseBlock->getElseFirst();
        lastBlock = ifElseBlock->getElseLast();
        while(block != lastBlock) {
            next = block->getNext();
            block->acceptVisitor(*this);
            block = next;
        }
        if(block) {
            block->acceptVisitor(*this);
        }
    }
    cout <<"End of IfElseBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    cout <<"Beginning of WhileBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    cout <<"End of WhileBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    cout <<"End of FunctionDeclBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    vector<FunctionCallInstance*>& functionCallInstances = functionCallBlock->getFnCallInstances();
    for(int i=0; i<functionCallInstances.size(); i++) {
        FunctionCallInstance* fnCallInstance = functionCallInstances[i];
        BasicBlock* block = fnCallInstance->getFirst();
        block->acceptVisitor(*this);

        block = fnCallInstance->getFnDecl();
        block->acceptVisitor(*this);

        block = fnCallInstance->getLast();
        block->acceptVisitor(*this);
    }
    cout <<"End of FunctionCallBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitCFG(BasicBlock* block) {
    cout <<"Beginning of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    while(block) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    cout <<"End of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}
