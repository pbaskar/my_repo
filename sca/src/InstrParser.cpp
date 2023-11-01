/*
 * InstrParser.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include<cstring>
#include "InstrParser.h"

InstrParser::InstrParser(): p_mainBlock(0), p_pos(0) {

}

InstrParser::~InstrParser() {
	delete p_mainBlock;
}

Status InstrParser::parseFile(char* fileName) {
	Status status = p_tokenizer.openFile(fileName);
	if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
	status = p_tokenizer.nextLine();
	if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
	p_mainBlock = new Block;
	status = parseFunctionDecl(p_mainBlock);
	p_tokenizer.closeFile();
	return status;
}

Status InstrParser::parseBlock(Block* block) {
	Status status = SUCCESS;
	p_exprParser.setSymbolTable(block->getSymbolTable());
	char closeBrace = p_tokenizer.nextChar(true);
	while(closeBrace != '}' && closeBrace != '\0') {
		Status status = parseStmt(block);
		if(status == FAILURE) {
			return FAILURE;
		}
		closeBrace = p_tokenizer.nextChar(true);
	}
	return status;
}

Status InstrParser::parseStmt(Block* block) {
	char* next = p_tokenizer.nextWord(true);
	Status status = SUCCESS;
	if(strcmp(next, "int")==0) {
		p_tokenizer.consumeWord();
		status = parseDecl(block);
	}
	else if(strcmp(next, "if")==0) {
		p_tokenizer.consumeWord();
		status = parseIfElse(block);
	}
	else if(strcmp(next, "else")==0) {
		p_tokenizer.consumeWord();
		//status = parseElse(block);
	}
	else if(strcmp(next, "while")==0) {
		p_tokenizer.consumeWord();
		status = parseWhile(block);
	}
	else {
		status = parseAssign(block);
	}
	delete next;
	return status;
}

Status InstrParser::parseDecl(Block* block) {
	Status status = SUCCESS;
	AssignStmt* stmt = new AssignStmt(DECL);
	block->addStatement(stmt);
	char* name = p_tokenizer.nextWord();
	if(name == nullptr) { cout<<"InstrParser::parseDecl: name not found "<<endl; return FAILURE; }
	stmt->setVar(block->addSymbol(name));

	char equal = p_tokenizer.nextChar();
	if(equal == ';') { 	p_tokenizer.nextLine(); return status; }
	if(equal != '=') { cout<<"InstrParser::parseDecl: = not found "<<endl; return FAILURE; }

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) { cout<<"InstrParser::parseDecl: value not found "<<endl; return FAILURE; }
	Expr* value = p_exprParser.parseExpressionStr(next);
	delete next;
	if(value == nullptr) { cout<<"InstrParser::parseDecl: could not parse value "<<endl; return FAILURE; }
	stmt->setValue(value);
	p_tokenizer.nextLine();
	cout <<"Declarative stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseAssign(Block* block) {
	Status status = SUCCESS;
	AssignStmt* stmt = new AssignStmt(ASSIGN);
	block->addStatement(stmt);

	char* name = p_tokenizer.nextWord();
	if(name == nullptr) return FAILURE;
	stmt->setVar(block->fetchVariable(name));

	char equal = p_tokenizer.nextChar();
	if ( equal != '=' ) return FAILURE;

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* value = p_exprParser.parseExpressionStr(next);
	delete next;
	if(value == nullptr) return FAILURE;
	stmt->setValue(value);
	p_tokenizer.nextLine();
	cout <<"Assignment stmt: size = " << block->getSubStatements().size()  <<" " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseIfElse(Block* block) {
	Status status = SUCCESS;
	IfStmt* ifStmt = new IfStmt(IF);
	ifStmt->setBlock(new Block(block));
	block->addStatement(ifStmt);

	status = parseIf(ifStmt);
	if(status == FAILURE) return FAILURE;

	p_tokenizer.nextLine();
	char* next = p_tokenizer.nextWord(true);
	if(strcmp(next, "else") != 0) { cout <<"no else " <<endl; delete next; return status; }
	delete next;
	p_tokenizer.consumeWord();

	IfStmt* elseStmt = new IfStmt(ELSE);
	elseStmt->setBlock(new Block(block));
	status = parseElse(elseStmt);
	if(status == FAILURE) { delete elseStmt; return FAILURE; }

	ifStmt->setElse(elseStmt);
	p_tokenizer.nextLine();
	return status;
}

Status InstrParser::parseIf(IfStmt* stmt) {
	Status status = SUCCESS;

	char openBrace = p_tokenizer.nextChar();
	if ( openBrace != '(')	return FAILURE;
	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* condition = p_exprParser.parseExpressionStr(next);
	delete next;
	if(condition == nullptr) return FAILURE;
	stmt->setCondition(condition);
	p_tokenizer.nextLine();
	status = parseBlock(stmt->getBlock());

	cout <<"If stmt: " <<stmt->getBlock()->getSubStatements().size() << " "<<*stmt <<endl;
	return status;
}

Status InstrParser::parseElse(IfStmt* stmt) {
	Status status = SUCCESS;

	p_tokenizer.nextLine();
	status = parseBlock(stmt->getBlock());

	cout <<"Else stmt: " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseWhile(Block* block) {
	Status status = SUCCESS;
	WhileStmt* stmt = new WhileStmt(WHILE);
	stmt->setBlock(new Block(block));
	block->addStatement(stmt);

	char openBrace = p_tokenizer.nextChar();
	if ( openBrace != '(') return FAILURE;

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* condition = p_exprParser.parseExpressionStr(next);
	delete next;
	if(condition == nullptr) return FAILURE;
	stmt->setCondition(condition);

	p_tokenizer.nextLine();
	status = parseBlock(stmt->getBlock());
	p_tokenizer.nextLine();
	cout <<"While stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseFunctionDecl(Block* block) {
	Status status = SUCCESS;
	FunctionDeclStmt* stmt = new FunctionDeclStmt(FUNC_DECL);
	stmt->setBlock(new Block(block));
	block->addStatement(stmt);

	char* next = p_tokenizer.nextWord(); //return type
	if(next == nullptr) { cout<<"InstrParser::parseFunctionDecl: return type not found "<<endl; return FAILURE; }

	next = p_tokenizer.nextWord();	//function name - add to symbol table
	if(next == nullptr) { cout<<"InstrParser::parseFunctionDecl: function name not found "<<endl; return FAILURE; }

	char openBrace = p_tokenizer.nextChar();
	if ( openBrace != '(') { cout<<"InstrParser::parseFunctionDecl: open brace not found "<<endl; return FAILURE; }

	next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Variable* argument = block->addSymbol(next);
	stmt->addFormalArgument(argument);

	p_tokenizer.nextLine();
	status = parseBlock(stmt->getBlock());
	p_tokenizer.nextLine();
	cout <<"Function Decl stmt: size = " <<stmt->getBlock()->getSubStatements().size() << " parent size "
			<<block->getSubStatements().size() << " " <<*stmt <<" status " <<status <<endl;
	return status;
}

Status InstrParser::parseFunctionCall(Block* block) {
	Status status = SUCCESS;
	FunctionCallStmt* stmt = new FunctionCallStmt(FUNC_CALL);
	block->addStatement(stmt);

	char* next = p_tokenizer.nextWord(); //function name
	if(next == nullptr) { cout<<"InstrParser::parseFunctionCall: function name not found "<<endl; return FAILURE; }

	char openBrace = p_tokenizer.nextChar();
	if ( openBrace != '(') { cout<<"InstrParser::parseFunctionCall: open brace not found "<<endl; return FAILURE; }

	next = p_tokenizer.nextWord();
	if(next == nullptr) { return status; }
	Expr* argument = p_exprParser.parseExpressionStr(next);
	delete next;
	if(argument == nullptr) { cout<<"InstrParser::parseFunctionCall: could not parse argument "<<endl; return FAILURE; }
	stmt->addActualArgument(argument);

	p_tokenizer.nextLine();
	cout <<"Function Call stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
	return status;
}

Block* InstrParser::getBlock() const {
	return p_mainBlock;
}
