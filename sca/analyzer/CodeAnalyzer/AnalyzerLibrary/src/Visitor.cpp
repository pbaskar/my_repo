/*
 * Visitor.cpp
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

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

VariableInitCheckVisitor::VariableInitCheckVisitor(map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>>& inVariableNodes) {
    p_inVariableNodes = std::move(inVariableNodes);
}

VariableInitCheckVisitor::~VariableInitCheckVisitor() {

}

void VariableInitCheckVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    map<const Variable*, vector<AssignmentNode*>> variableNodes = p_inVariableNodes.at(basicBlock);
    cout <<"Beginning of Block: variableNodes size " <<variableNodes.size() <<endl <<endl;
    bool found = false;
    const vector<Node*>& nodeList = basicBlock->getNodeList();
    for(Node* node: nodeList) {
        const Expr* value=node->getValue();
        vector<const Expr*> RHSVariables;
        if(value) {
            value->getRHSVariables(RHSVariables);
        }
        else continue;
        cout <<"RHS value " << *value <<" variables count " <<RHSVariables.size() <<endl;
        for(auto variableIt = RHSVariables.begin(); variableIt != RHSVariables.end(); variableIt++) {
            cout <<"variable ptr " << *variableIt <<endl;
            const Variable* variable = dynamic_cast<const Variable*>(*variableIt);
            if (!variable) { cout <<"RHS cast error " <<variable <<endl; continue; }
            found = false;
            if(variableNodes.find(variable) != variableNodes.end()) {
                found = true;
            }
            if(!found) {
                const char* m = "Not initialized ";
                const char* name = variable->getName();
                Result r;
                r.errorMessage = new char[strlen(m)+strlen(name)+1];
                r.errorMessage[0] = '\0';
                strncat(r.errorMessage,m,strlen(m));
                strncat(r.errorMessage,name,strlen(name));
                r.errorMessage[strlen(m)+strlen(name)] = '\0';
                cout << "Error message " <<r.errorMessage <<endl;
                p_results.push_back(r);
            }
        }
        if(node->type() != ASSIGNMENT) continue;
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
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
            cout <<"LHS variable ptr " <<*variableIt << " " <<LHSVariables.size() <<endl;
            const Variable* var = dynamic_cast<const Variable*>(*variableIt);
            if (!var) { cout <<"LHS cast error " << var <<endl; continue; }
            auto variableNodeIt = variableNodes.find(var);
            if(variableNodeIt != variableNodes.end()) {
                auto& nodes = variableNodes.at(var);
                nodes.clear();
                nodes.push_back(assignNode);
                cout<<" added " <<*assignNode << " " <<assignNode <<endl;
            }
            else {
                vector<AssignmentNode*> v;
                v.push_back(assignNode);
                cout<<" added " <<*assignNode << " " <<assignNode <<endl;
                variableNodes.insert_or_assign(var, v);
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
