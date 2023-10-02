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
	head = new BasicBlock;
	BasicBlock* currBlock = head;
	s = buildBlock(currBlock, stmtList);
	return s;
}

Status ControlFlowGraph::buildBlock(BasicBlock*& currBlock, const vector<Stmt*>& stmtList) {
	Status s = SUCCESS;
	bool beginNewBlock = false;

	for(Stmt* stmt : stmtList) {
		StmtType type = stmt->p_type;
		switch(type) {
			case DECL:
			case ASSIGN: {
				    AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
					AssignmentNode* newNode = new AssignmentNode(*assignStmt);
					cout << "Assignment Node: " << *newNode << " " <<stmtList.size() <<endl;

					if(beginNewBlock) {
						beginNewBlock = false;
						BasicBlock* newBlock = new BasicBlock;
						currBlock->setNext(newBlock);
						currBlock = newBlock;
					}
					currBlock->addNode(newNode);
				}
				break;
		   case IF:
		   case ELSE: {
					IfElseBlock* ifElseBlock = new IfElseBlock;
					currBlock->setNext(ifElseBlock);

					IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
					ConditionNode* ifNode = new ConditionNode(*ifStmt);
					cout << "If Node: " << *ifNode << " " <<stmtList.size() <<endl;

					BasicBlock* ifBlock = new BasicBlock;
					ifBlock->addNode(ifNode);

					ifElseBlock->p_ifFirst = ifBlock;
					s = buildBlock(ifBlock, ifStmt->p_subStatements);
					ifElseBlock->p_ifLast = ifBlock;

					IfStmt* elseStmt = ifStmt->p_else;
					ConditionNode* elseNode = new ConditionNode(*elseStmt);
					cout << "Else Node: " << *elseNode << " " <<stmtList.size() <<endl;

					BasicBlock* elseBlock = new BasicBlock;
					elseBlock->addNode(elseNode);

					ifElseBlock->p_elseFirst = elseBlock;
					s = buildBlock(elseBlock, elseStmt->p_subStatements);
					ifElseBlock->p_elseLast = elseBlock;

					currBlock = ifElseBlock;
					beginNewBlock = true;
				}
				break;
			case WHILE: {
					WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
					ConditionNode* whileNode = new ConditionNode(*whileStmt);
					cout << "While Node: " << *whileNode << " " <<stmtList.size() <<endl;

					WhileBlock* whileBlock = new WhileBlock;
					whileBlock->addNode(whileNode);

					currBlock->setNext(whileBlock);
					BasicBlock* whileBlk = whileBlock;
					s = buildBlock(whileBlk, whileStmt->p_subStatements);
					whileBlock->p_last = whileBlk;
					whileBlk->p_back = whileBlock;

					currBlock = whileBlock;
					beginNewBlock = true;
				}
				break;
			default: {
					cout <<"error " <<endl;
				}
				break;
		}
	}
	return s;
}

void ControlFlowGraph::print(ostream& os) {
	BasicBlock* curr = head;
	while(curr) {
		os << *curr <<endl;
		curr = curr->p_next;
	}
}
