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

VariableInitCheckVisitor::VariableInitCheckVisitor() {

}

VariableInitCheckVisitor::~VariableInitCheckVisitor() {

}

void VariableInitCheckVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    cout <<"Beginning of Block: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    vector<const Expr*> variables;
    bool found = false;
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
        for(auto variableIt = variables.begin(); variableIt != variables.end(); variableIt++) {
            const Variable* variable = static_cast<const Variable*>(*variableIt);
            if (!variable) cout <<"cast error " <<endl;
            found = false;
            for(auto variableNodeIt=p_variableNodes.begin(); variableNodeIt != p_variableNodes.end(); variableNodeIt++ ) {
                if(variable == (*variableNodeIt)->getVariable()) {
                    found = true;
                }
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
                p_results.push_back(r);
            }
        }
        for(auto variableNodeIt=p_variableNodes.begin(); variableNodeIt != p_variableNodes.end(); ) {
            if(*assignNode==*(*variableNodeIt)) {
                variableNodeIt = p_variableNodes.erase(variableNodeIt);
            }
            else variableNodeIt++;
        }
        p_variableNodes.push_back(assignNode);
        cout<<" added " <<*assignNode <<endl;
    }
    cout <<"End of Block: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
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
    cout <<"Beginning of IfElseBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    vector<AssignmentNode*> variableNodesBegin = p_variableNodes;
    vector<AssignmentNode*> variableNodesEnd;
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);
    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    lastBlock->acceptVisitor(*this);

    variableNodesEnd = std::move(p_variableNodes);
    p_variableNodes = std::move(variableNodesBegin);

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
    intersect(p_variableNodes, variableNodesEnd);
    cout <<"End of IfElseBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    cout <<"Beginning of WhileBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    cout <<"End of WhileBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    cout <<"End of FunctionDeclBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    cout <<"Beginning of FunctionCallBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = functionCallBlock->getFirst();
    block->acceptVisitor(*this);

    block = functionCallBlock->getFnDecl();
    block->acceptVisitor(*this);
    cout <<"End of FunctionCallBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitCFG(BasicBlock* block) {
    cout <<"Beginning of CFG: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    while(block) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    cout <<"End of CFG: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}
