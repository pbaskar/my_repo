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
    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(basicBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> inDefinitions = p_inDefinitions.at(basicBlock);

    cout <<"Beginning of Block: variableNodes size " <<variableNodes.size() <<endl <<endl;
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

        cout <<"RHS value " << *value <<" variables count " <<RHSVariables.size() <<endl;


        //Check nodes and definitions for invalid assignments and definitions
        for(auto variableIt = RHSVariables.begin(); variableIt != RHSVariables.end(); variableIt++) {
            const Variable* variable = dynamic_cast<const Variable*>(*variableIt);
            if (!variable) { cout <<"RHS cast error " <<variable <<endl; continue; }
            else cout <<"rhs variable " << *variable <<endl;
            found = true;

            map<const Variable*, vector<AssignmentNode*>>::iterator nodesIt = variableNodes.find(variable);
            if(nodesIt != variableNodes.end()) {
                const vector<AssignmentNode*>& nodes= nodesIt->second;
                if(!nodes.empty()) {
                    for(int i=0; i<nodes.size(); i++) {
                        if(nodes[i]->getValue()->getExprType() == ExprType::DEFINITION) {
                            cout <<"Invalid assignment: var " <<*variable<< " value = " <<*value <<endl;
                            found = false;
                            break;
                        }
                    }
                } else {
                    found = false;
                }
            }
            map<const Variable*, vector<pair<const Definition*, bool>>>::iterator inDefinitionIt = inDefinitions.find(variable);
            if(inDefinitionIt != inDefinitions.end()) {
                const vector<pair<const Definition*, bool>>& definitions= inDefinitionIt->second;
                if(!definitions.empty()) {
                    for(int i=0; i<definitions.size(); i++) {
                        if(!(definitions[i].second)) {
                            cout <<"Invalid definition: var " <<*variable<< " value = " <<*value <<endl;
                            found = false;
                            break;
                        }
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

            //replace with or add current assignNode for both Variable and PointerVariable ptr_ptr_p(top level)
            auto variableNodeIt = variableNodes.find(var);
            if(variableNodeIt != variableNodes.end()) {
                auto& nodes = variableNodes.at(var);
                nodes.clear();
                nodes.push_back(assignNode);
                cout<<" replaced var " <<*var << " " <<*assignNode <<endl;
            }
            else {
                vector<AssignmentNode*> v;
                v.push_back(assignNode);
                cout<<" added var " <<*var << " " << *assignNode <<endl;
                variableNodes.insert_or_assign(var, v);
            }

            if(var->getExprType() == ExprType::POINTERVARIABLE || value->getExprType()==ExprType::DELETEFNCALL) {
               switch(value->getExprType()) {
                   case ExprType::MALLOCFNCALL: {
                        const MallocFnCall* mallocFnCall = static_cast<const MallocFnCall*>(value);
                        const Definition* rhsDefinition = mallocFnCall->getDefinition();

                        pair<const Definition*, bool> p(rhsDefinition, true);
                        //copy new definition as LHS var malloc definition
                        auto definitionIt = inDefinitions.find(var);
                        if(definitionIt != inDefinitions.end()) {
                            auto& definitions = inDefinitions.at(var);
                            definitions.clear();
                            definitions.push_back(p);
                            cout<<" replaced definition of var " <<*var << " rhs definition valid " <<*rhsDefinition
                               <<*assignNode <<endl;
                        }
                        else {
                            vector<pair<const Definition*, bool>> v;
                            v.push_back(p);
                            cout<<" added definition var " <<*var << " assignNode ptr " << *assignNode
                               << " outDefinitions size " << inDefinitions.size()<<endl;
                            inDefinitions.insert_or_assign(var, v);
                        }

                   break;
                   }
                   case ExprType::DELETEFNCALL: {
                           if(RHSVariables.empty()) { cout <<"RHS variables empty"; break; }
                           const Expr* rhsExpr = RHSVariables[0];
                           const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                           if(rhs==nullptr) { cout << "pointer assignment rhs cast error " <<endl; break; }
                           auto definitionIt = inDefinitions.find(rhs);
                           if(definitionIt != inDefinitions.end()) {
                               auto& definitions = inDefinitions.at(rhs);
                               for(int i=0; i<definitions.size(); i++) {
                                   auto& definition = definitions[i];
                                   definition.second = false;
                                   cout <<"delete : set false to " <<*rhs <<endl;
                               }
                           }
                           else {
                               assert(false);
                           }
                    }
                   break;
                   case ExprType::DEFINITION: {
                        const Variable* pointsToLHS = var;
                        const Definition* rhsDefinition = static_cast<const Definition*>(value);

                        while(true) {
                            const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(pointsToLHS);
                            if(lhsPointer == nullptr) break;

                            //add dummy definition for ptr_ptr_p and ptr_p
                            auto definitionIt = inDefinitions.find(lhsPointer);
                            if(definitionIt != inDefinitions.end()) {
                                assert(false);
                            }
                            else {
                                cout<<" added definition lhs " <<*lhsPointer << " outDefinitions size " << inDefinitions.size()<<endl;
                                vector<pair<const Definition*, bool>> v;
                                v.push_back(pair<const Definition*, bool>(rhsDefinition, false));
                                inDefinitions.insert_or_assign(lhsPointer, v);
                            }

                            pointsToLHS = lhsPointer->getPointsTo();
                            assert(pointsToLHS != nullptr);

                            //add dummy assignment node for ptr_p and p
                            auto pointsToVariableNodeIt = variableNodes.find(pointsToLHS);
                            if(pointsToVariableNodeIt != variableNodes.end()) {
                                assert(false);
                            }
                            else {
                                cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode <<endl;
                                vector<AssignmentNode*> v;
                                v.push_back(assignNode);
                                variableNodes.insert_or_assign(pointsToLHS, v);
                            }
                        }
                   }
                   break;
                   //Variable, PointerVariable, Dereference Operator
                   default: {
                       if(RHSVariables.empty()) { cout <<"RHS variables empty"; break; }
                       const Expr* rhsExpr = RHSVariables[0];
                       const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                       if(rhs==nullptr) { cout << "pointer assignment rhs cast error " <<endl; break; }
                        const Variable* pointsToLHS = var;
                        const Variable* pointsToRHS = rhs;

                        while(true) {
                            const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(pointsToLHS);
                            if(lhsPointer == nullptr) break;
                            const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(pointsToRHS);
                            if(rhsPointer == nullptr) break;

                            //Only for Pointer variables ptr_ptr_p, ptr_p
                            vector<pair<const Definition*, bool>> rhsDefinitions;
                            auto rhsDefinitionIt = inDefinitions.find(rhsPointer);
                            if(rhsDefinitionIt != inDefinitions.end()) {
                                rhsDefinitions = inDefinitions.at(rhsPointer);
                                cout<<" found rhsDefinitions for LHS " <<*lhsPointer << " RHS pointer " << *rhsPointer <<
                                      " rhsDefinitions size " << rhsDefinitions.size()<<endl;
                            }
                            else {
                                cout <<"not found rhsDefinitions for LHS " <<*lhsPointer << " RHS pointer" << *rhsPointer <<endl;
                            }

                            //find malloc definition for RHS and copy to LHS
                            auto definitionIt = inDefinitions.find(lhsPointer);
                            if(definitionIt != inDefinitions.end()) {
                                auto& definitions = inDefinitions.at(lhsPointer);
                                definitions.clear();
                                definitions = /*std::move*/(rhsDefinitions);
                                cout<<" replaced definition of lhs " <<*lhsPointer << " rhs pointer " << *rhsPointer <<
                                   " rhsDefinitions size " <<rhsDefinitions.size() <<endl;
                            }
                            else {
                                cout<<" added definition lhs " <<*lhsPointer << " outDefinitions size " << inDefinitions.size()<<endl;
                                inDefinitions.insert_or_assign(lhsPointer, rhsDefinitions);
                            }

                            pointsToLHS = lhsPointer->getPointsTo();
                            pointsToRHS = rhsPointer->getPointsTo();
                            assert(pointsToLHS != nullptr);
                            assert(pointsToRHS != nullptr);

                            // for ptr_p and p
                            vector<AssignmentNode*> rhsNodes;
                            auto pointsToRHSVariableNodeIt = variableNodes.find(pointsToRHS);
                            if(pointsToRHSVariableNodeIt != variableNodes.end()) {
                                rhsNodes = variableNodes.at(pointsToRHS);
                                cout<<" found rhsNodes for LHS " <<*pointsToLHS << " " <<*assignNode << " rhsNodes size " << rhsNodes.size()<<endl;
                            }
                            else {
                                cout <<"not found rhsNodes for LHS " <<*pointsToLHS << " " << *assignNode << endl;

                            }

                            //copy assignment nodes from pointsToRHS to pointsToLHS
                            auto pointsToVariableNodeIt = variableNodes.find(pointsToLHS);
                            if(pointsToVariableNodeIt != variableNodes.end()) {
                                auto& nodes = variableNodes.at(pointsToLHS);
                                nodes.clear();
                                nodes = /*std::move*/(rhsNodes);
                                cout<<" replaced pointsto var " <<* pointsToLHS << " " <<*assignNode
                                   << " rhsNodes size " <<rhsNodes.size() <<endl;
                            }
                            else {
                                cout<<" added pointsto var " <<*pointsToLHS << " " << *assignNode << " rhsNodes size " <<rhsNodes.size()<<endl;
                                variableNodes.insert_or_assign(pointsToLHS, rhsNodes);
                            }
                        }
                   }
                   break; //default
               } //switch end
            }
        }
    }
    cout <<"End of Block: variableNodes size " <<variableNodes.size() <<endl <<endl;
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
    BasicBlock* block = functionCallBlock->getFirst();
    block->acceptVisitor(*this);

    block = functionCallBlock->getFnDecl();
    block->acceptVisitor(*this);

    block = functionCallBlock->getLast();
    block->acceptVisitor(*this);
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
