/*
 * Visitor.cpp
 *
 *  Created on: Oct 5, 2023
 *      Author: prbas_000
 */

#include<cstring>
#include<vector>
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
	//delete functionDeclBlock;
}

void DeleteVisitor::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
	delete functionCallBlock;
}

VariableInitCheckVisitor::VariableInitCheckVisitor() {

}

VariableInitCheckVisitor::~VariableInitCheckVisitor() {

}

void VariableInitCheckVisitor::visitBasicBlock(BasicBlock* basicBlock) {
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
				AssignmentNode* variableAssignNode = static_cast<AssignmentNode*>(*variableNodeIt);
				if(variable == variableAssignNode->getVariable()) {
					found = true;
				}
			}
			if(!found) cout <<"Not initialized " << *variable <<endl;
		}
		for(auto variableNodeIt=p_variableNodes.begin(); variableNodeIt != p_variableNodes.end(); ) {
			AssignmentNode* variableAssignNode = static_cast<AssignmentNode*>(*variableNodeIt);
			if(*variableAssignNode==*assignNode) {
				variableNodeIt = p_variableNodes.erase(variableNodeIt);
			}
			else variableNodeIt++;
		}
		p_variableNodes.push_back(node);
		cout<<" added " <<*assignNode <<endl;
	}
	cout <<"End of Block: variableNodes size " <<p_variableNodes.size() <<endl <<endl;
}

void VariableInitCheckVisitor::visitIfElseBlock(IfElseBlock* ifElseBlock) {
	const vector<Node*>& nodeList = ifElseBlock->getNodeList();
	for(Node* node: nodeList) {
		p_variableNodes.push_back(node);
	}
	cout <<"variableNodes ifelse size " <<p_variableNodes.size() <<endl;
}

void VariableInitCheckVisitor::visitWhileBlock(WhileBlock* whileBlock) {
	const vector<Node*>& nodeList = whileBlock->getNodeList();
	for(Node* node: nodeList) {
		p_variableNodes.push_back(node);
	}
	cout <<"variableNodes while size " <<p_variableNodes.size() <<endl;
}

void VariableInitCheckVisitor::visitFunctionDeclBlock(FunctionDeclBlock* whileBlock) {

}

void VariableInitCheckVisitor::visitFunctionCallBlock(FunctionCallBlock* whileBlock) {

}
