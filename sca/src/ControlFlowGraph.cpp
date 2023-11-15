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

Status ControlFlowGraph::buildCFG(const Block* block) {
	Status status = SUCCESS;
	head = new BasicBlock(block->getSymbolTable());
	BasicBlock* currBlock = head;
	status = buildBlock(currBlock, block);
	return status;
}

Status ControlFlowGraph::buildBlock(BasicBlock*& currBlock, const Block* block) {
	Status status = SUCCESS;
	bool beginNewBlock = false;
	auto& stmtList = block->getSubStatements();
	for(Stmt* stmt : stmtList ) {
		StmtType type = stmt->getType();
		switch(type) {
		case DECL:
		case ASSIGN: {
			AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
			AssignmentNode* newNode = new AssignmentNode(*assignStmt);
			cout << "Assignment Node: " << *newNode << " " <<block->getSubStatements().size() <<endl;

			if(beginNewBlock) {
				beginNewBlock = false;
				BasicBlock* newBlock = new BasicBlock(block->getSymbolTable());
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
			cout << "If Node: " << *ifNode << " " <<block->getSubStatements().size() <<endl;

			first = new BasicBlock(ifStmt->getBlock()->getSymbolTable());
			first->addNode(ifNode);

			ifLast = first;
			status = buildBlock(ifLast, ifStmt->getBlock());

			const IfStmt* elseStmt = ifStmt->getElse();
			if(elseStmt) {
				ConditionNode* elseNode = new ConditionNode(*elseStmt);
				cout << "Else Node: " << *elseNode << " " <<block->getSubStatements().size() <<endl;

				elseFirst = new BasicBlock(elseStmt->getBlock()->getSymbolTable());
				elseFirst->addNode(elseNode);

				elseLast = elseFirst;
				status = buildBlock(elseLast, elseStmt->getBlock());
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
			cout << "While Node: " << *whileNode << " " <<block->getSubStatements().size() <<endl;

			first = new BasicBlock(whileStmt->getBlock()->getSymbolTable());
			first->addNode(whileNode);

			last = first;
			status = buildBlock(last, whileStmt->getBlock());

			WhileBlock* whileBlock = new WhileBlock(0,first,last);
			//whileBlock->setSelf();
			currBlock->setNext(whileBlock);
			currBlock = whileBlock;
			beginNewBlock = true;
		}
		break;
		case FUNC_DECL: {
			BasicBlock* first(0);
			BasicBlock* last(0);

			FunctionDeclStmt* functionDeclStmt = static_cast<FunctionDeclStmt*>(stmt);

			first = new BasicBlock(functionDeclStmt->getBlock()->getSymbolTable());
			/*auto formalArguments = functionDeclStmt->getFormalArguments();
			//cout << "Function Decl Node: " << *functionDeclNode << " " <<block->getSubStatements().size() <<endl;
			for(Variable* var : formalArguments) {
				AssignmentNode* functionDeclNode = new AssignmentNode(var);
				first->addNode(functionDeclNode);
			}*/

			last = first;
			status = buildBlock(last, functionDeclStmt->getBlock());
			cout <<"function decl block "  <<functionDeclStmt->getName() <<endl;
			FunctionDeclBlock* functionDeclBlock = new FunctionDeclBlock(0,functionDeclStmt->getName(),first,last);
			head->addFnSymbol(functionDeclBlock);
			beginNewBlock = true;
			/*PrintVisitor printVisitor;
			TraverserOne tOne(&printVisitor);
			tOne.traverseFunctionDeclBlock(functionDeclBlock);
			DeleteVisitor deleteVisitor;
			TraverserOne tOne2(&deleteVisitor);
			tOne2.traverseFunctionDeclBlock(functionDeclBlock);*/
			//currBlock->setNext(whileBlock);
			//currBlock = whileBlock;
		}
		break;
		case FUNC_CALL: {
			BasicBlock* first(0);
			FunctionDeclBlock* fnDecl(0);

			FunctionCallStmt* functionCallStmt = static_cast<FunctionCallStmt*>(stmt);
			first = new BasicBlock(block->getSymbolTable());

			fnDecl = head->fetchFunctionDeclBlock(functionCallStmt->getName());

			first->setNext(fnDecl);
			FunctionCallBlock* functionCallBlock = new FunctionCallBlock(0,functionCallStmt->getName(),first,fnDecl);
			cout <<"function call block "  <<functionCallStmt->getName() <<endl;

			currBlock->setNext(functionCallBlock);
			currBlock = functionCallBlock;
			beginNewBlock = true;
		}
		break;
		default: {
			cout <<"error " <<endl;
		}
		break;
		}
	}
	return status;
}

void ControlFlowGraph::print(ostream& os) {
	PrintVisitor printVisitor;
	TraverserOne tOne(&printVisitor);
	tOne.traverseCFG(this->head);
}

void ControlFlowGraph::variableInitCheck() {
	VariableInitCheckVisitor variableInitCheckVisitor;
	TraverserAllPath tAllPath(&variableInitCheckVisitor);
	tAllPath.traverseCFG(this->head);
}

void ControlFlowGraph::clear() {
	DeleteVisitor deleteVisitor;
	TraverserOne tOne(&deleteVisitor);
	tOne.traverseCFG(this->head);
}
