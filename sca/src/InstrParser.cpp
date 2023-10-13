/*
 * InstrParser.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include<cstring>
#include "InstrParser.h"

InstrParser::InstrParser(): p_pos(0) {
	// TODO Auto-generated constructor stub

}

InstrParser::~InstrParser() {
	delete p_mainBlock;
}

Status InstrParser::parseFile(char* fileName) {
	Status status = p_tokenizer.openFile(fileName);
	if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
	status = p_tokenizer.nextLine();
	if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
	char next = p_tokenizer.nextChar();
	if(next=='{') {
		status = p_tokenizer.nextLine();
		if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
		p_mainBlock = new Block;
		status = parseBlock(p_mainBlock);
	}
	p_tokenizer.closeFile();
	return status;
}

Status InstrParser::parseBlock(Block* block) {
	Status status = SUCCESS;
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
	AssignStmt* stmt = new AssignStmt;
	stmt->p_type = DECL;
	block->addStatement(stmt);

	char* name = p_tokenizer.nextWord();
	if(name == nullptr) return FAILURE;
	stmt->p_var = block->addSymbol(name);

	char equal = p_tokenizer.nextChar();
	if (equal != '=') return FAILURE;

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* value = p_exprParser.parseExpressionStr(next);
	delete next;
	if(value == nullptr) return FAILURE;
	stmt->p_value = value;
	p_tokenizer.nextLine();
	cout <<"Declarative stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseAssign(Block* block) {
	Status status = SUCCESS;
	AssignStmt* stmt = new AssignStmt;
	stmt->p_type = ASSIGN;
	block->addStatement(stmt);

	char* name = p_tokenizer.nextWord();
	if(name == nullptr) return FAILURE;
	stmt->p_var = block->fetchVariable(name);

	char equal = p_tokenizer.nextChar();
	if ( equal != '=' ) return FAILURE;

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* value = p_exprParser.parseExpressionStr(next);
	delete next;
	if(value == nullptr) return FAILURE;
	stmt->p_value = value;
	p_tokenizer.nextLine();
	cout <<"Assignment stmt: size = " << block->getSubStatements().size()  <<" " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseIfElse(Block* block) {
	Status status = SUCCESS;
	IfStmt* ifStmt = new IfStmt;
	ifStmt->p_type = IF;
	ifStmt->p_block = new Block(block);
	block->addStatement(ifStmt);

	status = parseIf(ifStmt);
	if(status == FAILURE) return FAILURE;

	p_tokenizer.nextLine();
	char* next = p_tokenizer.nextWord(true);
	if(strcmp(next, "else") != 0) { cout <<"no else " <<endl; delete next; return status; }
	delete next;
	p_tokenizer.consumeWord();

	IfStmt* elseStmt = new IfStmt;
	elseStmt->p_type = ELSE;
	elseStmt->p_block = new Block(block);
	status = parseElse(elseStmt);
	if(status == FAILURE) { delete elseStmt; return FAILURE; }

	ifStmt->p_else = elseStmt;
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
	stmt->p_condition = condition;
	p_tokenizer.nextLine();
	//SymbolTable* symTable = stmt->makeNewSymbolTable();
	status = parseBlock(stmt->p_block);

	cout <<"If stmt: " <<stmt->p_block->getSubStatements().size() << " "<<*stmt <<endl;
	return status;
}

Status InstrParser::parseElse(IfStmt* stmt) {
	Status status = SUCCESS;

	p_tokenizer.nextLine();
	status = parseBlock(stmt->p_block);

	cout <<"Else stmt: " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseWhile(Block* block) {
	Status status = SUCCESS;
	WhileStmt* stmt = new WhileStmt;
	stmt->p_type = WHILE;
	stmt->p_block = new Block(block);
	block->addStatement(stmt);

	char openBrace = p_tokenizer.nextChar();
	if ( openBrace != '(') return FAILURE;

	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	Expr* condition = p_exprParser.parseExpressionStr(next);
	delete next;
	if(condition == nullptr) return FAILURE;
	stmt->p_condition = condition;

	p_tokenizer.nextLine();
	status = parseBlock(stmt->p_block);  //handle return value
	p_tokenizer.nextLine();
	cout <<"While stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
	return status;
}

const Block* InstrParser::getBlock() const {
	return p_mainBlock;
}

