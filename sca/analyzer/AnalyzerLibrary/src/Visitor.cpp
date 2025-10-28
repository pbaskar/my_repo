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
#include<sstream>

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

void PrintVisitor::visitForBlock(ForBlock* forBlock) {
    forBlock->print();
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

void DeleteVisitor::visitForBlock(ForBlock* forBlock) {
    delete forBlock;
}

void DeleteVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    delete functionDeclBlock;
}

void DeleteVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    delete functionCallBlock;
}

void copyDefinitionsToVar(const Variable* lhs, const Definition* pointsToDefinition,
                                    map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions) {
    pair<const Definition*, bool> pointsToDefinitionPair(pointsToDefinition,pointsToDefinition->isValid());
    auto definitionIt = outDefinitions.find(lhs);
    if(definitionIt != outDefinitions.end()) {
        auto& definitions = outDefinitions.at(lhs);
        definitions.clear();
        definitions.push_back(pointsToDefinitionPair);
        Logger::getDebugStreamInstance()<<" replaced definition of lhs " <<*lhs <<
           " rhsDefinitions size " <<definitions.size() <<endl;
    }
    else {
        Logger::getDebugStreamInstance()<<" added definition lhs " <<*lhs << " outDefinitions size " << outDefinitions.size()<<endl;
        vector<pair<const Definition*, bool>> v;
        v.push_back(pointsToDefinitionPair);
        outDefinitions.insert_or_assign(lhs, v);
    }
}

void copyDefinitionsToPointerVar(const Variable* lhs, const Definition* pointsToDefinition,
                                    map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions) {
    while(true) {
        const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(lhs);
        if(lhsPointer == nullptr) {
            break;
        }
        assert(pointsToDefinition);
        pair<const Definition*, bool> pointsToDefinitionPair(pointsToDefinition,pointsToDefinition->isValid());
        auto definitionIt = outDefinitions.find(lhsPointer);
        if(definitionIt != outDefinitions.end()) {
            auto& definitions = outDefinitions.at(lhsPointer);
            definitions.clear();
            definitions.push_back(pointsToDefinitionPair);
            Logger::getDebugStreamInstance()<<" replaced definition of lhs " <<*lhsPointer <<
               " rhsDefinitions size " <<definitions.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added definition lhs " <<*lhsPointer << " outDefinitions size " << outDefinitions.size()<<endl;
            vector<pair<const Definition*, bool>> v;
            v.push_back(pointsToDefinitionPair);
            outDefinitions.insert_or_assign(lhsPointer, v);
        }
        lhs = lhsPointer->getPointsTo();
        assert(lhs != nullptr);
        const PointerDefinition* pointerDefinition = dynamic_cast<const PointerDefinition*>(pointsToDefinition);
        assert(pointerDefinition);
        pointsToDefinition = pointerDefinition->getPointsTo();
    }
    assert(pointsToDefinition);
    if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
        copyDefinitionsToStructVar(lhs, pointsToDefinition->getMemDefinitions(), outDefinitions);
    } else {
        copyDefinitionsToVar(lhs, pointsToDefinition, outDefinitions);
    }

}

void copyNodesToPointerVar(const Variable* lhs, AssignmentNode* assignNode,
                                map<const Variable*, vector<AssignmentNode*>>& outVariableNodes) {

    while(true) {
        assert(lhs != nullptr);
        auto pointsToVariableNodeIt = outVariableNodes.find(lhs);
        if(pointsToVariableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(lhs);
            nodes.clear();
            nodes.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" replaced pointsto var " <<*lhs << " " <<*assignNode
               << " rhsNodes size " <<nodes.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added pointsto var " <<*lhs << " " << *assignNode << " rhsNodes " <<assignNode <<endl;
            vector<AssignmentNode*> v;
            v.push_back(assignNode);
            outVariableNodes.insert_or_assign(lhs, v);
        }
        const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(lhs);
        if(lhsPointer == nullptr) {
            assert(lhs);
            if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
                copyNodesToStructVar(lhs, assignNode, outVariableNodes);
            }
            break;
        }
        lhs = lhsPointer->getPointsTo();
    }
}

void copyDefinitionsToStructVar(const Variable* lhs, const vector<const Definition*>& pointsToDefinitions,
                                map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions) {
    const StructVariable* lhsStruct=static_cast<const StructVariable*>(lhs);
    const vector<const Variable*> memVars = lhsStruct->getMemVars();
    for(int i=0; i<memVars.size(); i++) {
        const Variable* memVar = memVars[i];
        assert(i<pointsToDefinitions.size());
        if(memVar->getExprType() == ExprType::POINTERVARIABLE) {
            copyDefinitionsToPointerVar(memVar, pointsToDefinitions[i], outDefinitions);
            continue;
        }
        copyDefinitionsToVar(memVar, pointsToDefinitions[i], outDefinitions);
    }
}

void copyNodesToStructVar(const Variable* lhs, AssignmentNode* assignNode,
                                map<const Variable*, vector<AssignmentNode*>>& outVariableNodes) {

    const StructVariable* lhsStruct=static_cast<const StructVariable*>(lhs);
    const vector<const Variable*> memVars = lhsStruct->getMemVars();
    for(int i=0; i<memVars.size(); i++) {
        const Variable* memVar = memVars[i];
        if(memVar->getExprType() == ExprType::POINTERVARIABLE) {
            copyNodesToPointerVar(memVar, assignNode, outVariableNodes);
            continue;
        }
        auto pointsToVariableNodeIt = outVariableNodes.find(memVar);
        if(pointsToVariableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(memVar);
            nodes.clear();
            nodes.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" replaced pointsto var " <<*memVar << " " <<*assignNode
               << " rhsNodes size " <<nodes.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added pointsto var " <<*memVar << " " << *assignNode << " rhsNodes " <<assignNode <<endl;
            vector<AssignmentNode*> v;
            v.push_back(assignNode);
            outVariableNodes.insert_or_assign(memVar, v);
        }
    }
}

void updateVariableGroup(const Variable* lhs, const Variable* rhs,
                         map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                         map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    //find definition of rhs in the map and entry that has rhs, add lhs to that entry of variablegroup
    //if not present, create one
    auto rhsDefinitionIt = outDefinitions.find(rhs);
    if(rhsDefinitionIt == outDefinitions.end()) return;
    vector<pair<const Definition*, bool>>& defPairs = rhsDefinitionIt->second;
    for(int j=0; j<defPairs.size(); j++) {
        auto variableGroupsIt = outVariableGroups.find(defPairs[j].first);
        if(variableGroupsIt != outVariableGroups.end()) {
            bool added = false;
            vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
            for(int i=0; i<outVariableGroup.size(); i++) {
                vector<const Variable*>& outVariables = outVariableGroup[i];
                auto variableRhsIt = std::find(outVariables.begin(), outVariables.end(),rhs);
                if(variableRhsIt != outVariables.end()) {
                    outVariables.push_back(lhs);
                    added = true;
                }
            }
            if(!added) { //new variable group for existing definition
                vector<const Variable*> newVariables;
                newVariables.push_back(lhs);
                newVariables.push_back(rhs);
                outVariableGroup.push_back(newVariables);
            }
        }
        else { //new definition entry
            vector<vector<const Variable*>> newVariableGroup;
            vector<const Variable*> newVariables;
            newVariables.push_back(lhs);
            newVariables.push_back(rhs);
            newVariableGroup.push_back(newVariables);
            pair<const Definition*, vector<vector<const Variable*>>> defPair(defPairs[j].first, newVariableGroup);
            outVariableGroups.insert(defPair);
        }
    }

    //find definition of lhs in the map and entry that has lhs, remove lhs from entry of variable group if present
    auto lhsDefinitionIt = outDefinitions.find(lhs);
    if(lhsDefinitionIt == outDefinitions.end()) return;
    vector<pair<const Definition*, bool>>& lhsDefPairs = lhsDefinitionIt->second;
    for(int j=0; j<lhsDefPairs.size(); j++) {
        auto variableGroupsIt = outVariableGroups.find(lhsDefPairs[j].first);
        if(variableGroupsIt != outVariableGroups.end()) {
            bool added = false;
            vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
            for(auto variableGroupIt=outVariableGroup.begin(); variableGroupIt != outVariableGroup.end(); variableGroupIt++) {
                vector<const Variable*>& outVariables = *variableGroupIt;

                auto variableLhsIt = std::find(outVariables.begin(), outVariables.end(),lhs);
                if(variableLhsIt != outVariables.end()) {
                    outVariables.erase(variableLhsIt);
                    if(outVariables.empty()) {
                        outVariableGroup.erase(variableGroupIt);
                    }
                    break;
                }
            }
        }
    }
}

void updateVariableGroupDefOne(const vector<const Variable*>& outVariables, const Definition* rhs,
                                  map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                  map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    auto variableGroupsIt = outVariableGroups.find(rhs);
    Logger::getDebugStreamInstance() <<"outVariableGroups " <<outVariableGroups.size() << " " <<outVariableGroups.begin()->first <<" " <<rhs <<endl;
    if(variableGroupsIt != outVariableGroups.end()) {
        vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
        Logger::getDebugStreamInstance() <<"outVariable Group " <<outVariableGroup.size() <<endl;
        outVariableGroup.push_back(outVariables);
    } else {
        vector<vector<const Variable*>> outVariableGroup;
        outVariableGroup.push_back(outVariables);
        pair<const Definition*, vector<vector<const Variable*>>> p(rhs, outVariableGroup);
        outVariableGroups.insert(p);
    }
}

void updateVariableGroupDef(const vector<const Variable*>& outVariables, const Variable* rhs,
                            map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                            map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    //add variable group to the entry corresponding to rhs definition in outVariableGroup

    auto rhsDefinitionIt = outDefinitions.find(rhs);
    if(rhsDefinitionIt == outDefinitions.end()) { Logger::getDebugStreamInstance() << "rhs no def found " <<endl; return; }
    vector<pair<const Definition*, bool>>& rhsDefPairs = rhsDefinitionIt->second;
    Logger::getDebugStreamInstance() <<"rhs def pairs " <<rhsDefPairs.size() <<endl;
    for(int j=0; j<rhsDefPairs.size(); j++) {
        updateVariableGroupDefOne(outVariables, rhsDefPairs[j].first, outDefinitions, outVariableGroups);
    }
}

void copyDefinitionsFromVar(const Variable* lhs, const Variable* rhs,
                            map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                            map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    //Only for variables p
    vector<pair<const Definition*, bool>> rhsDefinitions;
    auto rhsDefinitionIt = outDefinitions.find(rhs);
    if(rhsDefinitionIt != outDefinitions.end()) {
        rhsDefinitions = outDefinitions.at(rhs);
        Logger::getDebugStreamInstance()<<" found rhsDefinitions RHS variable " << *rhs <<
           " rhsDefinitions size " << rhsDefinitions.size()<<endl;
    }
    else {
        Logger::getDebugStreamInstance() <<"not found rhsDefinitions RHS pointer" << *rhs <<endl;
    }

    auto definitionIt = outDefinitions.find(lhs);
    if(definitionIt != outDefinitions.end()) {
        auto& definitions = outDefinitions.at(lhs);
        definitions.clear();
        definitions = /*std::move*/(rhsDefinitions);
        Logger::getDebugStreamInstance()<<" replaced definition of lhs " <<*lhs <<"  "<<lhs <<
        " rhsDefinitions size " <<definitions.size() <<endl;
    }
    else {
        Logger::getDebugStreamInstance()<<" added definition lhs " <<*lhs << " " <<lhs <<" outDefinitions size " << outDefinitions.size()<<endl;
        outDefinitions.insert_or_assign(lhs, rhsDefinitions);
    }
}

void copyDefinitionsToVarGroup(const Variable* lhs, const Definition* rhs,
                                        map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                        map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    // find definition of lhs and entry that has lhs in the map and fetch all variables and recursively call this method.
    auto lhsDefinitionIt = outDefinitions.find(lhs);
    if(lhsDefinitionIt == outDefinitions.end()) { Logger::getDebugStreamInstance() << "no def found " <<endl; return; }
    vector<pair<const Definition*, bool>>& defPairs = lhsDefinitionIt->second;
    Logger::getDebugStreamInstance() <<"def pairs " <<defPairs.size() <<endl;
    for(int j=0; j<defPairs.size(); j++) {
        auto variableGroupsIt = outVariableGroups.find(defPairs[j].first);
        //Logger::getDebugStreamInstance() <<"outVariableGroups " <<outVariableGroups.size() << " " <<outVariableGroups.begin()->first <<" " <<defPairs[j].first <<endl;
        if(variableGroupsIt != outVariableGroups.end()) {
            vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
            //Logger::getDebugStreamInstance() <<"outVariable Group " <<outVariableGroup.size() <<endl;
            for(int i=0; i<outVariableGroup.size();) {
                vector<const Variable*> outVariables = outVariableGroup[i];
                auto variableLhsIt = std::find(outVariables.begin(), outVariables.end(),lhs);
                if(variableLhsIt != outVariables.end()) {
                    //Logger::getDebugStreamInstance() <<"outVariables " <<outVariables.size() <<endl;
                    updateVariableGroupDefOne(outVariables, rhs, outDefinitions, outVariableGroups);
                    outVariableGroup.erase(outVariableGroup.begin()+i);
                    for(int k=0; k<outVariables.size(); k++) {
                        copyDefinitionsToVar(outVariables[k], rhs, outDefinitions);
                        Logger::getDebugStreamInstance() <<"copy definitions from var " << *(outVariables[k]) <<endl;
                    }
                    break;
                }
                else {
                    i++;
                }
            }
        }
    }
}

void copyDefinitionsFromVarGroup(const Variable* lhs, const Variable* rhs,
                                        map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                        map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    // find definition of lhs and entry that has lhs in the map and fetch all variables and recursively call this method.
    auto lhsDefinitionIt = outDefinitions.find(lhs);
    if(lhsDefinitionIt == outDefinitions.end()) { Logger::getDebugStreamInstance() << "no def found " <<endl; return; }
    vector<pair<const Definition*, bool>>& defPairs = lhsDefinitionIt->second;
    Logger::getDebugStreamInstance() <<"def pairs " <<defPairs.size() <<endl;
    for(int j=0; j<defPairs.size(); j++) {
        auto variableGroupsIt = outVariableGroups.find(defPairs[j].first);
        //Logger::getDebugStreamInstance() <<"outVariableGroups " <<outVariableGroups.size() << " " <<outVariableGroups.begin()->first <<" " <<defPairs[j].first <<endl;
        if(variableGroupsIt != outVariableGroups.end()) {
            vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
            //Logger::getDebugStreamInstance() <<"outVariable Group " <<outVariableGroup.size() <<endl;
            for(int i=0; i<outVariableGroup.size();) {
                vector<const Variable*> outVariables = outVariableGroup[i];
                updateVariableGroupDef(outVariables, rhs, outDefinitions, outVariableGroups);
                auto variableLhsIt = std::find(outVariables.begin(), outVariables.end(),lhs);
                if(variableLhsIt != outVariables.end()) {
                    //Logger::getDebugStreamInstance() <<"outVariables " <<outVariables.size() <<endl;
                    outVariableGroup.erase(outVariableGroup.begin()+i);
                    for(int k=0; k<outVariables.size(); k++) {
                        copyDefinitionsFromVar(outVariables[k], rhs, outDefinitions, outVariableGroups);
                        Logger::getDebugStreamInstance() <<"copy definitions from var " << *(outVariables[k]) <<endl;
                    }
                    break;
                }
                else {
                    i++;
                }
            }
        }
    }
}

void copyDefinitionsFromPointerVarGroup(const Variable* lhs, const Variable* rhs,
                                        map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                        map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    // find definition of lhs and entry that has lhs in the map and fetch all variables and recursively call this method.
    auto lhsDefinitionIt = outDefinitions.find(lhs);
    if(lhsDefinitionIt == outDefinitions.end()) return;
    vector<pair<const Definition*, bool>>& defPairs = lhsDefinitionIt->second;
    for(int j=0; j<defPairs.size(); j++) {
        auto variableGroupsIt = outVariableGroups.find(defPairs[j].first);
        if(variableGroupsIt != outVariableGroups.end()) {
            vector<vector<const Variable*>>& outVariableGroup = (variableGroupsIt->second);
            for(int i=0; i < outVariableGroup.size();) {
                vector<const Variable*> outVariables = outVariableGroup[i];
                updateVariableGroupDef(outVariables, rhs, outDefinitions, outVariableGroups);
                auto variableLhsIt = std::find(outVariables.begin(), outVariables.end(),lhs);
                if(variableLhsIt != outVariables.end()) {
                    outVariableGroup.erase(outVariableGroup.begin()+i);
                    for(int k=0; k<outVariables.size(); k++) {
                        copyDefinitionsFromPointerVar(outVariables[k], rhs, outDefinitions, outVariableGroups);
                        Logger::getDebugStreamInstance() <<"copy definitions from pointer var " << *(outVariables[k]) <<endl;
                    }
                    break;
                }
                else {
                    i++;
                }
            }
        }
    }
}

void copyDefinitionsFromPointerVar(const Variable* lhs, const Variable* rhs,
                                map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    while(true) {
        const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(rhs);
        if(rhsPointer == nullptr) break;
        const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(lhs);
        if(lhsPointer == nullptr) break;

        //Only for Pointer variables ptr_ptr_p, ptr_p
        vector<pair<const Definition*, bool>> rhsDefinitions;
        auto rhsDefinitionIt = outDefinitions.find(rhsPointer);
        if(rhsDefinitionIt != outDefinitions.end()) {
            rhsDefinitions = outDefinitions.at(rhsPointer);
            Logger::getDebugStreamInstance()<<" found rhsDefinitions RHS pointer " << *rhsPointer <<
               " rhsDefinitions size " << rhsDefinitions.size()<<endl;
        }
        else {
            Logger::getDebugStreamInstance() <<"not found rhsDefinitions RHS pointer" << *rhsPointer <<endl;
        }

        auto definitionIt = outDefinitions.find(lhsPointer);
        if(definitionIt != outDefinitions.end()) {
            auto& definitions = outDefinitions.at(lhsPointer);
            definitions.clear();
            definitions = /*std::move*/(rhsDefinitions);
            Logger::getDebugStreamInstance()<<" replaced definition of lhs " <<*lhsPointer <<
            " rhsDefinitions size " <<definitions.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added definition lhs " <<*lhsPointer << " outDefinitions size " << outDefinitions.size()<<endl;
            outDefinitions.insert_or_assign(lhsPointer, rhsDefinitions);
        }

        rhs = rhsPointer->getPointsTo();
        assert(rhs != nullptr);
        lhs = lhsPointer->getPointsTo();
        assert(lhs != nullptr);

        updateVariableGroup(lhs, rhs, outDefinitions, outVariableGroups);
    }
    if(lhs->getExprType() == VARIABLE) {
        copyDefinitionsFromVar(lhs, rhs, outDefinitions, outVariableGroups);
    }
}

void copyNodesFromPointerVar(const Variable* lhs, const Variable* rhs, AssignmentNode* assignNode,
                                map<const Variable*, vector<AssignmentNode*>>& outVariableNodes,
                                map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    while(true) {
        assert(rhs != nullptr);
        assert(lhs != nullptr);

        // for ptr_ptr_p and ptr_p
        vector<AssignmentNode*> rhsNodes;
        auto pointsToRHSVariableNodeIt = outVariableNodes.find(rhs);
        if(pointsToRHSVariableNodeIt != outVariableNodes.end()) {
            rhsNodes = outVariableNodes.at(rhs);
            Logger::getDebugStreamInstance()<<" found rhsNodes " <<*lhs <<" " <<*rhs <<" rhsNodes size " << rhsNodes.size()<<endl;
        }
        else {
            Logger::getDebugStreamInstance() <<"not found rhsNodes " << *rhs <<" " <<*rhs << endl;

        }

        auto pointsToVariableNodeIt = outVariableNodes.find(lhs);
        if(pointsToVariableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(lhs);
            nodes.clear();
            nodes = /*std::move*/(rhsNodes);
            Logger::getDebugStreamInstance()<<" replaced pointsto var " <<*lhs << " " << lhs <<" " <<rhs
               << " rhsNodes size " <<nodes.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added pointsto var " <<*lhs << " " << *lhs <<" " <<*rhs << " rhsNodes size " <<rhsNodes.size()<<endl;
            outVariableNodes.insert_or_assign(lhs, rhsNodes);
        }

        const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(rhs);
        if(rhsPointer == nullptr) break;

        const PointerVariable* lhsPointer=dynamic_cast<const PointerVariable*>(lhs);
        if(lhsPointer == nullptr) break;

        rhs = rhsPointer->getPointsTo();
        lhs = lhsPointer->getPointsTo();
    }
    if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
        visitBasicBlockHelper(lhs, rhs, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
    }
}

void copyDefinitionsFromStructVar(const Variable* lhs, const Variable* rhs,
                                map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    const StructVariable* rhsStruct=static_cast<const StructVariable*>(rhs);
    const vector<const Variable*> rhsMemVars = rhsStruct->getMemVars();

    const StructVariable* lhsStruct=static_cast<const StructVariable*>(lhs);
    const vector<const Variable*> lhsMemVars = lhsStruct->getMemVars();

    for(int i=0; i<lhsMemVars.size(); i++) {
        const Variable* rhsMemVar = rhsMemVars[i];
        //Only for Pointer variables ptr_ptr_p, ptr_p
        vector<pair<const Definition*, bool>> rhsDefinitions;
        auto rhsDefinitionIt = outDefinitions.find(rhsMemVar);
        if(rhsDefinitionIt != outDefinitions.end()) {
            rhsDefinitions = outDefinitions.at(rhsMemVar);
            Logger::getDebugStreamInstance()<<" found rhsDefinitions RHS pointer " << *rhsMemVar <<
                  " rhsDefinitions size " << rhsDefinitions.size()<<endl;
        }
        else {
            Logger::getDebugStreamInstance() <<"not found rhsDefinitions RHS pointer" << *rhsMemVar <<endl;
        }

        const Variable* lhsMemVar = lhsMemVars[i];
        auto definitionIt = outDefinitions.find(lhsMemVar);
        if(definitionIt != outDefinitions.end()) {
            auto& definitions = outDefinitions.at(lhsMemVar);
            definitions.clear();
            definitions = /*std::move*/(rhsDefinitions);
            Logger::getDebugStreamInstance()<<" replaced definition of lhs " <<*lhsMemVar <<
               " rhsDefinitions size " <<definitions.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added definition lhs " <<*lhsMemVar << " outDefinitions size " << outDefinitions.size()<<endl;
            outDefinitions.insert_or_assign(lhsMemVar, rhsDefinitions);
        }
    }
}

void copyNodesFromStructVar(const Variable* lhs, const Variable* rhs, AssignmentNode* assignNode,
                                map<const Variable*, vector<AssignmentNode*>>& outVariableNodes,
                                map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                                map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    const StructVariable* rhsStruct=static_cast<const StructVariable*>(rhs);
    const vector<const Variable*> rhsMemVars = rhsStruct->getMemVars();

    const StructVariable* lhsStruct=static_cast<const StructVariable*>(lhs);
    const vector<const Variable*> lhsMemVars = lhsStruct->getMemVars();

    for(int i=0; i<lhsMemVars.size(); i++) {
        const Variable* rhsMemVar = rhsMemVars[i];
        assert(rhsMemVar != nullptr);
        vector<AssignmentNode*> rhsNodes;
        auto pointsToRHSVariableNodeIt = outVariableNodes.find(rhsMemVar);
        if(pointsToRHSVariableNodeIt != outVariableNodes.end()) {
            rhsNodes = outVariableNodes.at(rhsMemVar);
            Logger::getDebugStreamInstance()<<" found rhsNodes " <<*rhsMemVar <<" rhsNodes size " << rhsNodes.size()<<endl;
        }
        else {
            Logger::getDebugStreamInstance() <<"not found rhsNodes " << *rhsMemVar << endl;

        }

        const Variable* lhsMemVar = lhsMemVars[i];
        //copy assignment nodes from pointsToRHS to pointsToLHS
        //for ptr_ptr_p ptr_p
        auto pointsToVariableNodeIt = outVariableNodes.find(rhsMemVar);
        if(pointsToVariableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(lhsMemVar);
            nodes.clear();
            nodes = /*std::move*/(rhsNodes);
            Logger::getDebugStreamInstance()<<" replaced pointsto var " <<*lhsMemVar << " " <<*rhsMemVar
               << " rhsNodes size " <<nodes.size() <<endl;
        }
        else {
            Logger::getDebugStreamInstance()<<" added pointsto var " <<*lhsMemVar << " " << *rhsMemVar << " rhsNodes size " <<rhsNodes.size()<<endl;
            outVariableNodes.insert_or_assign(lhsMemVar, rhsNodes);
        }
        if(lhsMemVar->getExprType() == ExprType::STRUCTVARIABLE || lhsMemVar->getExprType() == ExprType::POINTERVARIABLE) {
            visitBasicBlockHelper(lhs, rhs, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
        }
    }
}

void copyDefinitionsForAddressOfOperator(const Variable* var, const Expr* rhsExpr,
                                         map<const Variable*, vector<AssignmentNode*>>& outVariableNodes,
                                         map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions ) {
    const AddressOfVariable* rhs = dynamic_cast<const AddressOfVariable*>(rhsExpr);
    if(rhs != nullptr) {
        const Definition* rhsDefinition = rhs->getDefinition();
        pair<const Definition*, bool> p(rhsDefinition, rhsDefinition->isValid());

        auto definitionIt = outDefinitions.find(rhs);
        if(definitionIt != outDefinitions.end()) {
            auto& definitions = outDefinitions.at(rhs);
            definitions.clear();
            definitions.push_back(p);
            Logger::getDebugStreamInstance()<<" replaced definition of rhs " <<*rhs << " rhs definition valid " <<*rhsDefinition
                <<endl;
        }
        else {
            vector<pair<const Definition*, bool>> v;
            v.push_back(p);
            Logger::getDebugStreamInstance()<<" added definition rhs " <<*rhs
               << " outDefinitions size " << outDefinitions.size()<<endl;
            outDefinitions.insert_or_assign(rhs, v);
        }
    }
}

void copyNodesForAddressOfOperator(const Expr* rhsExpr, AssignmentNode* assignNode,
                                 map<const Variable*, vector<AssignmentNode*>>& outVariableNodes,
                                 map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions ) {
    //replace with or add current assignNode for AddressOfOperator
    const AddressOfVariable* rhs = dynamic_cast<const AddressOfVariable*>(rhsExpr);
    if(rhs != nullptr) {
        auto variableNodeIt = outVariableNodes.find(rhs);
        if(variableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(rhs);
            Logger::getDebugStreamInstance() <<"nodes size before clear " << nodes.size() <<endl;
            nodes.clear();
            nodes.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" replaced var " <<*rhs << " " <<*assignNode <<endl;
        }
        else {
            vector<AssignmentNode*> v;
            v.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" added var " <<*rhs << " " << *assignNode <<endl;
            outVariableNodes.insert_or_assign(rhs, v);
        }
    }
}

void visitBasicBlockHelper(const Variable* var, const Expr* value, AssignmentNode* assignNode,
                             map<const Variable*, vector<AssignmentNode*>>& outVariableNodes,
                             map<const Variable*, vector<pair<const Definition*, bool>>>& outDefinitions,
                             map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups) {
    vector<const Expr*> RHSVariables;
    assert(value);
    value->getRHSVariables(RHSVariables);
    if(var->getExprType() == ExprType::VARIABLE) {
        //replace with or add current assignNode for Variable
        auto variableNodeIt = outVariableNodes.find(var);
        if(variableNodeIt != outVariableNodes.end()) {
            auto& nodes = outVariableNodes.at(var);
            Logger::getDebugStreamInstance() <<"nodes size before clear " << nodes.size() <<endl;
            nodes.clear();
            nodes.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" replaced var " <<*var << " " <<*assignNode <<endl;
        }
        else {
            vector<AssignmentNode*> v;
            v.push_back(assignNode);
            Logger::getDebugStreamInstance()<<" added var " <<*var << " " << *assignNode <<endl;
            outVariableNodes.insert_or_assign(var, v);
        }
        switch(value->getExprType()) {
            case ExprType::CONSTANT: {
                const Constant* constant = static_cast<const Constant*>(value);
                copyDefinitionsToVarGroup(var, constant->getDefinition(), outDefinitions, outVariableGroups);
                copyDefinitionsToVar(var, constant->getDefinition(), outDefinitions);
            }
            break;
            case ExprType::OPERATOR: {
                const Operator* binaryOperator = static_cast<const Operator*>(value);
                copyDefinitionsToVarGroup(var, binaryOperator->getDefinition(), outDefinitions, outVariableGroups);
                copyDefinitionsToVar(var, binaryOperator->getDefinition(), outDefinitions);
            }
            break;
            case ExprType::UNARYOPERATOR: {
                const UnaryOperator* unaryOperator = static_cast<const UnaryOperator*>(value);
                copyDefinitionsToVarGroup(var, unaryOperator->getDefinition(), outDefinitions, outVariableGroups);
                copyDefinitionsToVar(var, unaryOperator->getDefinition(), outDefinitions);
            }
            break;
            case ExprType::DEFINITION: {
                 const Definition* rhsDefinition = static_cast<const Definition*>(value);
                 copyDefinitionsToVarGroup(var, rhsDefinition, outDefinitions, outVariableGroups);
                 copyDefinitionsToVar(var, rhsDefinition, outDefinitions);
            }
            break;
            case ExprType::ASSIGNOPERATOR: {
                const AssignOperator* rhs = static_cast<const AssignOperator*>(value);
                const Expr* rightOp = rhs->getRightOp();
                assert(rightOp != nullptr);
                switch(rightOp->getExprType()) {
                    case ExprType::CONSTANT: {
                        const Constant* constant = static_cast<const Constant*>(rightOp);
                        copyDefinitionsToVarGroup(var, constant->getDefinition(), outDefinitions, outVariableGroups);
                        copyDefinitionsToVar(var, constant->getDefinition(), outDefinitions);
                    }
                    break;
                    case ExprType::OPERATOR: {
                        const Operator* binaryOperator = static_cast<const Operator*>(rightOp);
                        copyDefinitionsToVarGroup(var, binaryOperator->getDefinition(), outDefinitions, outVariableGroups);
                        copyDefinitionsToVar(var, binaryOperator->getDefinition(), outDefinitions);
                    }
                    break;
                    case ExprType::UNARYOPERATOR: {
                        const UnaryOperator* unaryOperator = static_cast<const UnaryOperator*>(rightOp);
                        copyDefinitionsToVarGroup(var, unaryOperator->getDefinition(), outDefinitions, outVariableGroups);
                        copyDefinitionsToVar(var, unaryOperator->getDefinition(), outDefinitions);
                    }
                    break;
                    default:
                        goto CopyVariables;
                    break;
                }
            }
            break;
CopyVariables:
            default: {
                if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
                Logger::getDebugStreamInstance() <<"rhs variable " <<endl;
                const Expr* rhsExpr = RHSVariables[0];
                const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                if(rhs==nullptr) { Logger::getDebugStreamInstance() << "var rhs cast error " <<endl; break; }
                copyDefinitionsFromVarGroup(var, rhs, outDefinitions, outVariableGroups);
                copyDefinitionsFromVar(var, rhs, outDefinitions, outVariableGroups);
            }
        }// end switch
    }
    else if(var->getExprType() == ExprType::POINTERVARIABLE || value->getExprType()==ExprType::DELETEFNCALL) {
        switch(value->getExprType()) {
            case ExprType::DELETEFNCALL: {
               if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
               const Expr* rhsExpr = RHSVariables[0];
               const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
               if(rhs==nullptr) { Logger::getDebugStreamInstance() << "pointer assignment rhs cast error " <<endl; break; }
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
                               Logger::getDebugStreamInstance() <<"delete : set false to " <<itDef->second <<endl;
                           }
                       }
                   }
               }
               else {
                   Logger::getDebugStreamInstance() << "Delete variable that has no definition " <<endl;
                   assert(false);
               }
            }
            break;
            case ExprType::MALLOCFNCALL: {
                    const MallocFnCall* mallocFnCall = static_cast<const MallocFnCall*>(value);
                    const PointerDefinition* rhsDefinition = mallocFnCall->getDefinition();
                    copyDefinitionsToPointerVar(var, rhsDefinition, outDefinitions);
                    copyNodesToPointerVar(var, assignNode, outVariableNodes);
                }
            break;
            case ExprType::POINTERDEFINITION:
            case ExprType::ARRAYDEFINITION:
            case ExprType::DEFINITION: {
                const PointerDefinition* rhsDefinition = static_cast<const PointerDefinition*>(value);
                copyDefinitionsToPointerVar(var, rhsDefinition, outDefinitions);
                copyNodesToPointerVar(var, assignNode, outVariableNodes);
            }
            break;
            case ExprType::ASSIGNOPERATOR: {
                const AssignOperator* rhs = static_cast<const AssignOperator*>(value);
                const Expr* rightOp = rhs->getRightOp();
                assert(rightOp != nullptr);
                switch(rightOp->getExprType()) {
                    case ExprType::MALLOCFNCALL: {
                             const MallocFnCall* mallocFnCall = static_cast<const MallocFnCall*>(rightOp);
                             const Definition* rhsDefinition = mallocFnCall->getDefinition();
                             copyDefinitionsToPointerVar(var, rhsDefinition, outDefinitions);
                             copyNodesToPointerVar(var, assignNode, outVariableNodes);
                    }
                    break;
                    case ExprType::ADDRESSOFOPERATOR: {
                        if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
                        const Expr* rhsExpr = RHSVariables[0];
                        assert(rhsExpr != nullptr);
                        copyNodesForAddressOfOperator(rhsExpr, assignNode, outVariableNodes, outDefinitions);
                        copyDefinitionsForAddressOfOperator(var, rhsExpr, outVariableNodes, outDefinitions);
                    }
                    goto CopyPointerVariables;
                } //switch end
            }
            case ExprType::ADDRESSOFOPERATOR: {
                if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
                const Expr* rhsExpr = RHSVariables[0];
                assert(rhsExpr != nullptr);
                copyNodesForAddressOfOperator(var, assignNode, outVariableNodes, outDefinitions);
                copyDefinitionsForAddressOfOperator(var, rhsExpr, outVariableNodes, outDefinitions);
            } //Fallthrough
            //PointerVariable, Dereference Operator, AddressOfOperator
CopyPointerVariables:
            default: {
               if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
               Logger::getDebugStreamInstance() <<"rhs pointer variable " <<endl;
               const Expr* rhsExpr = RHSVariables[0];
               const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
               if(rhs==nullptr) { Logger::getDebugStreamInstance() << "pointer assignment rhs cast error " <<endl; break; }
               copyDefinitionsFromPointerVarGroup(var, rhs, outDefinitions, outVariableGroups);
               copyDefinitionsFromPointerVar(var, rhs, outDefinitions, outVariableGroups);
               copyNodesFromPointerVar(var, rhs, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
           }
        } //switch end
    }
    else if(var->getExprType() == ExprType::STRUCTVARIABLE) {
        switch(value->getExprType()) {
            case ExprType::DEFINITION: {
                 const Definition* rhsDefinition = static_cast<const Definition*>(value);
                 const vector<const Definition*>& pointsToDefinitions = rhsDefinition->getMemDefinitions();
                 copyDefinitionsToStructVar(var, pointsToDefinitions, outDefinitions);
                 copyNodesToStructVar(var, assignNode, outVariableNodes);
            }
            break;
            default: {
                if(RHSVariables.empty()) { Logger::getDebugStreamInstance() <<"RHS variables empty"<<endl; break; }
                Logger::getDebugStreamInstance() <<"rhs pointer variable " <<endl;
                const Expr* rhsExpr = RHSVariables[0];
                const Variable* rhs = dynamic_cast<const Variable*>(rhsExpr);
                if(rhs==nullptr) { Logger::getDebugStreamInstance() << "pointer assignment rhs cast error " <<endl; break; }
                copyDefinitionsFromStructVar(var, rhs, outDefinitions, outVariableGroups);
                copyNodesFromStructVar(var, rhs, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
            }
        }// end switch
    }
}

VariableInitCheckVisitor::VariableInitCheckVisitor(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& inVariableNodes,
                                                   map<BasicBlock*, map<const Variable*, vector<pair<const Definition*, bool>>>>& inDefinitions,
                                                   map<BasicBlock*, map<const Definition*, vector<vector<const Variable*>>>>& inVariableGroups) {
    p_inVariableNodes = std::move(inVariableNodes);
    p_inDefinitions = std::move(inDefinitions);
    p_inVariableGroups = std::move(inVariableGroups);
}

VariableInitCheckVisitor::~VariableInitCheckVisitor() {

}

void VariableInitCheckVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    map<const Variable*, vector<AssignmentNode*>> outVariableNodes = p_inVariableNodes.at(basicBlock);
    map<const Variable*, vector<pair<const Definition*, bool>>> outDefinitions = p_inDefinitions.at(basicBlock);
    map<const Definition*, vector<vector<const Variable*>>>& outVariableGroups = p_inVariableGroups.at(basicBlock);

    Logger::getDebugStreamInstance() <<"Beginning of Block: variableNodes size " <<outVariableNodes.size() <<endl <<endl;
    bool found = false;
    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        if(node->type() != ASSIGNMENT) continue;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        const Expr* value=assignNode->getValue();
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
            if(value->getExprType() == ExprType::DEFINITION ||
                    value->getExprType() == ExprType::POINTERDEFINITION) {
                const Definition* def = static_cast<const Definition*>(value);
                if(!def->isValid()) {
                    /*const char* m = "Not defined ";
                    const char* name = var->getName();
                    Result r;
                    r.errorMessage = new char[strlen(m)+strlen(name)+1];
                    r.errorMessage[0] = '\0';
                    strncat(r.errorMessage,m,strlen(m));
                    strncat(r.errorMessage,name,strlen(name));
                    r.errorMessage[strlen(m)+strlen(name)] = '\0';
                    Logger::getDebugStreamInstance() <<endl<<endl<<"Declaration " <<r.errorMessage << " " <<*var <<endl;
                    p_results.push_back(r);*/
                }
            }
        }

        Logger::getDebugStreamInstance() <<endl <<endl <<"RHS value " << *value <<" variables count " <<RHSVariables.size() <<endl;


        //Check nodes and definitions for invalid assignments and definitions
        for(auto variableIt = RHSVariables.begin(); variableIt != RHSVariables.end(); variableIt++) {
            const Variable* variable = dynamic_cast<const Variable*>(*variableIt);
            if (!variable) { Logger::getDebugStreamInstance() <<"RHS cast error " <<variable <<endl; continue; }
            else Logger::getDebugStreamInstance() <<"rhs variable " << *variable <<endl;
            found = true;

            map<const Variable*, vector<AssignmentNode*>>::iterator nodesIt = outVariableNodes.find(variable);
            if(nodesIt != outVariableNodes.end()) {
                const vector<AssignmentNode*>& nodes= nodesIt->second;
                if(!nodes.empty()) {
                    for(int i=0; i<nodes.size(); i++) {
                        const Expr* nodeValue = nodes[i]->getValue();
                        if(variable->getExprType() == VARIABLE) {
                            if(nodeValue->getExprType() == ExprType::DEFINITION ||
                                    nodeValue->getExprType() == ExprType::ARRAYDEFINITION) {
                                const Definition* def = static_cast<const Definition*>(nodeValue);
                                if(!def->isValid()) {
                                    Logger::getDebugStreamInstance() <<endl <<"Invalid assignment: var " <<*variable<< " value = " <<*value <<endl;
                                    //found = false;
                                    break;
                                }
                            }
                            else if(nodeValue->getExprType() == ExprType::MALLOCFNCALL) {
                                const MallocFnCall* mallocFnCall = static_cast<const MallocFnCall*>(nodeValue);
                                assert(mallocFnCall);
                                const Definition* def = mallocFnCall->getDefinition();
                                if(!def->isValid()) {
                                    Logger::getDebugStreamInstance() <<endl <<"Invalid assignment: var " <<*variable<< " value = " <<*value <<endl;
                                    //found = false;
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    Logger::getDebugStreamInstance() <<endl <<"Variable has no assignment node " <<endl;
                    //found = false;
                }
            }
            else {
                Logger::getDebugStreamInstance() <<endl <<"Variable has no entry in VariableNodes " <<endl;
                //found = false;
            }
            map<const Variable*, vector<pair<const Definition*, bool>>>::iterator outDefinitionIt = outDefinitions.find(variable);
            if(outDefinitionIt != outDefinitions.end()) {
                const vector<pair<const Definition*, bool>>& definitions= outDefinitionIt->second;
                if(!definitions.empty()) {
                    for(int i=0; i<definitions.size(); i++) {
                        if(!(definitions[i].second)) {
                            Logger::getDebugStreamInstance() <<"Invalid definition: var " <<*variable<< " value = " <<*value <<endl;
                            found = false;
                            break;
                        }
                        Logger::getDebugStreamInstance() <<"definitions for var " <<*variable << " " <<definitions[i].first<<" " <<definitions[i].second <<endl;
                    }
                }
                else {
                    Logger::getDebugStreamInstance() <<"Definition not found " <<endl;
                    found = false;
                }
            }
            if(!found) {
                const char* m = "Undefined variable ";
                stringstream ss;
                ss << *variable << " in " << *assignNode;
                string s = ss.str();
                const char* name = s.c_str();
                Result r;
                r.errorMessage = new char[strlen(m)+ strlen(name) +1];
                r.errorMessage[0] = '\0';
                strncat(r.errorMessage,m,strlen(m));
                strncat(r.errorMessage,name,strlen(name));
                r.errorMessage[strlen(m)+strlen(name)] = '\0';
                p_results.push_back(r);
            }
        }

        // compute result after each node inside basic block
        for(auto variableIt = LHSVariables.begin(); variableIt != LHSVariables.end(); variableIt++) {
            const Variable* var = dynamic_cast<const Variable*>(*variableIt);
            if (var==nullptr) { Logger::getDebugStreamInstance() <<"var null cast error " <<endl; continue; }
            Logger::getDebugStreamInstance() <<"LHS variable " << **variableIt <<" LHS Var size " << LHSVariables.size()
                << " RHS Var size "<<RHSVariables.size() <<endl;
            visitBasicBlockHelper(var, value, assignNode, outVariableNodes, outDefinitions, outVariableGroups);
        }
    }
    Logger::getDebugStreamInstance() <<"End of Block: variableNodes size " <<outVariableNodes.size() <<" definitions size " <<
           outDefinitions.size() <<endl <<endl;
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
    Logger::getDebugStreamInstance() <<"Beginning of IfElseBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);
    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        block = ifElseBlock->getElseFirst();
        lastBlock = ifElseBlock->getElseLast();
        while(block != lastBlock) {
            if(block->getType() == JUMPBLOCK) {
                Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
                break;
            }
            next = block->getNext();
            block->acceptVisitor(*this);
            block = next;
        }
        if(block) {
            block->acceptVisitor(*this);
        }
    }
    Logger::getDebugStreamInstance() <<"End of IfElseBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of WhileBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of WhileBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitForBlock(ForBlock* forBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of ForBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = forBlock->getFirst();
    BasicBlock* lastBlock = forBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of ForBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of FunctionDeclBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of FunctionDeclBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of FunctionCallBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
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
    Logger::getDebugStreamInstance() <<"End of FunctionCallBlock: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitCFG(BasicBlock* block) {
    Logger::getDebugStreamInstance() <<"Beginning of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    while(block) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    Logger::getDebugStreamInstance() <<"End of CFG: variableNodes size " <<p_inVariableNodes.size() <<endl <<endl;
}
