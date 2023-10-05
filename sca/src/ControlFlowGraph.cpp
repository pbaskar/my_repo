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
			BasicBlock* first(0);
			BasicBlock* ifLast(0);
			BasicBlock* elseFirst(0);
			BasicBlock* elseLast(0);

			IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
			ConditionNode* ifNode = new ConditionNode(*ifStmt);
			cout << "If Node: " << *ifNode << " " <<stmtList.size() <<endl;

			first = new BasicBlock;
			first->addNode(ifNode);

			ifLast = first;
			s = buildBlock(ifLast, ifStmt->p_subStatements);

			IfStmt* elseStmt = ifStmt->p_else;
			if(elseStmt) {
				ConditionNode* elseNode = new ConditionNode(*elseStmt);
				cout << "Else Node: " << *elseNode << " " <<stmtList.size() <<endl;

				elseFirst = new BasicBlock;
				elseFirst->addNode(elseNode);

				elseLast = elseFirst;
				s = buildBlock(elseLast, elseStmt->p_subStatements);
			}

			IfElseBlock* ifElseBlock = new IfElseBlock(0, first, ifLast, elseFirst, elseLast);
			currBlock->setNext(ifElseBlock);
			currBlock = ifElseBlock;
			beginNewBlock = true;
		}
		break;
		case WHILE: {
			BasicBlock* first(0);
			BasicBlock* last(0);

			WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
			ConditionNode* whileNode = new ConditionNode(*whileStmt);
			cout << "While Node: " << *whileNode << " " <<stmtList.size() <<endl;

			first = new BasicBlock;
			first->addNode(whileNode);

			last = first;
			s = buildBlock(last, whileStmt->p_subStatements);

			WhileBlock* whileBlock = new WhileBlock(0,first,last);
			//whileBlock->setSelf();
			currBlock->setNext(whileBlock);
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
	PrintVisitor printVisitor;
	traverse(printVisitor);
}

void ControlFlowGraph::traverse(Visitor& visitor) {
	if(head)
		head->acceptVisitor(visitor);
}
