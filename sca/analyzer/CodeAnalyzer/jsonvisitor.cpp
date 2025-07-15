#include "jsonvisitor.h"
#include <sstream>
#include <QJsonObject>

JsonVisitor::JsonVisitor()
{

}

JsonVisitor::~JsonVisitor() {

}

void JsonVisitor::visitBasicBlock(BasicBlock* basicBlock) {
    //cout <<"Beginning of Block: " <<p_variableNodes.size() <<endl <<endl;
    const vector<Node*>& nodeList = basicBlock->getNodeList();
    QJsonArray nodes;
    for(Node* node: nodeList) {
        stringstream ss;
        ss<<*node;
        nodes.append(ss.str().c_str());
    }
    QJsonObject basic;
    basic["basic"] = nodes;
    p_blocks.append(basic);
    //cout <<"End of Block: " <<p_variableNodes.size() <<endl <<endl;
}

void JsonVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    //cout <<"Beginning of IfElseBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);

    QJsonArray blocks_begin = p_blocks;
    QJsonObject blocks_end;
    p_blocks = QJsonArray();

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    lastBlock->acceptVisitor(*this);
    blocks_end["if"] = p_blocks;
    p_blocks = QJsonArray();
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

    blocks_end["else"] = p_blocks;
    p_blocks = blocks_begin;
    p_blocks.append(blocks_end);
    //cout <<"End of IfElseBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void JsonVisitor::visitWhileBlock(WhileBlock* whileBlock) {
    //cout <<"Beginning of WhileBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    QJsonArray blocks_begin = p_blocks;
    QJsonObject blocks_end;
    p_blocks = QJsonArray();

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }

    block->acceptVisitor(*this);
    blocks_end["while"] = p_blocks;
    p_blocks = blocks_begin;
    p_blocks.append(blocks_end);
    //cout <<"End of WhileBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void JsonVisitor::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    //cout <<"Beginning of FunctionDeclBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    QJsonArray blocks_begin = p_blocks;
    QJsonObject blocks_end;
    p_blocks = QJsonArray();

    while(block != lastBlock) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    blocks_end["fnDecl"] = p_blocks;
    p_blocks = blocks_begin;
    p_blocks.append(blocks_end);
    //cout <<"End of FunctionDeclBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void JsonVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
   // cout <<"Beginning of FunctionCallBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    vector<FunctionCallInstance*>& functionCallInstances = functionCallBlock->getFnCallInstances();
    for(int i=0; i<functionCallInstances.size(); i++) {
        FunctionCallInstance* fnCallInstance = functionCallInstances[i];
        BasicBlock* block = fnCallInstance->getFirst();

        QJsonArray blocks_begin = p_blocks;
        QJsonObject blocks_end;
        p_blocks = QJsonArray();

        block->acceptVisitor(*this);

        block = fnCallInstance->getFnDecl();
        block->acceptVisitor(*this);
        blocks_end["fnCall"] = p_blocks;
        p_blocks = blocks_begin;
        p_blocks.append(blocks_end);
    }
   // cout <<"End of FunctionCallBlock: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void JsonVisitor::visitCFG(BasicBlock* block) {
    //cout <<"Beginning of CFG: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
    BasicBlock* next(0);
    while(block) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    //cout <<"End of CFG: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}
