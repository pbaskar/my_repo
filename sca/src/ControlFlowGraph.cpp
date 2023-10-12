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
}

Status ControlFlowGraph::buildCFG(const Block& block) {
	Status s = SUCCESS;
	head = new BasicBlock;
	BasicBlock* currBlock = head;
	s = buildBlock(currBlock, block);
	return s;
}

Status ControlFlowGraph::buildBlock(BasicBlock*& currBlock, const Block& block) {
	Status s = SUCCESS;
	bool beginNewBlock = false;
	auto& stmtList = block.getSubStatements();
	for(Stmt* stmt : stmtList ) {
		StmtType type = stmt->p_type;
		switch(type) {
		case DECL:
		case ASSIGN: {
			AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
			AssignmentNode* newNode = new AssignmentNode(*assignStmt);
			cout << "Assignment Node: " << *newNode << " " <<block.getSubStatements().size() <<endl;

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
			cout << "If Node: " << *ifNode << " " <<block.getSubStatements().size() <<endl;

			first = new BasicBlock;
			first->addNode(ifNode);

			ifLast = first;
			s = buildBlock(ifLast, ifStmt->getBlock());

			IfStmt* elseStmt = ifStmt->p_else;
			if(elseStmt) {
				ConditionNode* elseNode = new ConditionNode(*elseStmt);
				cout << "Else Node: " << *elseNode << " " <<block.getSubStatements().size() <<endl;

				elseFirst = new BasicBlock;
				elseFirst->addNode(elseNode);

				elseLast = elseFirst;
				s = buildBlock(elseLast, elseStmt->getBlock());
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
			cout << "While Node: " << *whileNode << " " <<block.getSubStatements().size() <<endl;

			first = new BasicBlock;
			first->addNode(whileNode);

			last = first;
			s = buildBlock(last, whileStmt->getBlock());

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
	TraverserAllPath tAllPath(&printVisitor);
	tAllPath.traverseCFG(this);
}

void ControlFlowGraph::variableInitCheck() {
	VariableInitCheckVisitor variableInitCheckVisitor;
	TraverserAllPath tAllPath(&variableInitCheckVisitor);
	tAllPath.traverseCFG(this);
}

void ControlFlowGraph::clear() {
	DeleteVisitor deleteVisitor;
	TraverserOne tOne(&deleteVisitor);
	tOne.traverseCFG(this);
}
