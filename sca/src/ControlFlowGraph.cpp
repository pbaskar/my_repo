/*
 * ControlFlowGraph.cpp
 *
 *  Created on: Sep 26, 2023
 *      Author: prbas_000
 */

#include "ControlFlowGraph.h"

ControlFlowGraph::ControlFlowGraph(): head(0) {

}

ControlFlowGraph::~ControlFlowGraph() {
	// delete all nodes
}

Status ControlFlowGraph::buildCFG(const vector<Stmt*>& stmtList) {
	Status s = SUCCESS;
	Node* end = 0;
	s = buildBlock(head, stmtList,end);
	end->p_next = 0;
	return s;
}

Status ControlFlowGraph::buildBlock(Node*& begin, const vector<Stmt*>& stmtList, Node*& end) {
	Status s = SUCCESS;
	Node* currNode = begin;
	vector<Node*> endStmtList;
	for(Stmt* stmt : stmtList) {
		StmtType type = stmt->type;
		switch(type) {
			case DECL:
			case ASSIGN: {
					AssignmentNode* newNode = new AssignmentNode(stmt);
					cout << "Node: " << *newNode << " " <<stmtList.size() <<endl;
					if(!begin) {
						begin = newNode;
					}
					else {
						currNode->p_next = newNode;
					}
					currNode = newNode;
					for(Node* n : endStmtList) {
						if ( n->type() == CONDITION ) {
							ConditionNode* c = static_cast<ConditionNode*>(n);
							c->p_exit = newNode;
						}
						else
							n->p_next = newNode;
					}
					endStmtList.clear();
				}
				break;
		   case IF:
		   case ELSE: {
					Node* endIf= 0;
					ConditionNode* ifCond = new ConditionNode(stmt);
					cout << "Node: " << *ifCond << " " <<stmtList.size() <<endl;
					if(!begin) {
						begin = ifCond;
					}
					else {
						currNode->p_next = ifCond;
					}
					currNode = ifCond;
					for(Node* n : endStmtList) {
						if ( n->type() == CONDITION ) {
							ConditionNode* c = static_cast<ConditionNode*>(n);
							c->p_exit = ifCond;
						}
						else
							n->p_next = ifCond;
					}
					endStmtList.clear();
					s = buildBlock(currNode, stmt->subStatements, endIf);
					endStmtList.push_back(endIf);
					endStmtList.push_back(ifCond);
				}
				break;
			case WHILE: {
					ConditionNode* whileCond = new ConditionNode(stmt);
					cout << "Node: " << *whileCond << " " <<stmtList.size() <<endl;
					if(!begin) {
						begin = whileCond;
					}
					else {
						currNode->p_next = whileCond;
					}
					currNode = whileCond;
					for(Node* n : endStmtList) {
						if ( n->type() == CONDITION ) {
							ConditionNode* c = static_cast<ConditionNode*>(n);
							c->p_exit = whileCond;
						}
						else
							n->p_next = whileCond;
					}
					endStmtList.clear();
					Node* endWhile =0;
					s = buildBlock(currNode, stmt->subStatements, endWhile);
					endWhile->p_next = currNode;
					endStmtList.push_back(whileCond);
				}
				break;
			default: {
					cout <<"error " <<endl;
				}
				break;
		}
	}
	end = currNode;
	return s;
}
