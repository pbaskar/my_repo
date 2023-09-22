/*
 * InstrParser.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include<cstring>
#include "InstrParser.h"

InstrParser::InstrParser() {
	// TODO Auto-generated constructor stub

}

InstrParser::~InstrParser() {
	for(Stmt* stmt : p_stmtList) {
		delete stmt;
	}
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
    	status = parseBlock(p_stmtList);
    }
    p_tokenizer.closeFile();
	return status;
}

Status InstrParser::parseBlock(vector<Stmt*>& stmtList) {
	Status status = SUCCESS;
	char closeBrace = p_tokenizer.nextChar(true);
	while(closeBrace != '}' && closeBrace != '\0') {
		Stmt* stmt = new Stmt;
		Status status = parseStmt(stmt);
		if(status == FAILURE) {
			delete stmt;
			return FAILURE;
		}
		stmtList.push_back(stmt);
		p_tokenizer.nextLine();
	    closeBrace = p_tokenizer.nextChar(true);
	}
	return status;
}

Status InstrParser::parseStmt(Stmt* stmt) {
    char* next = p_tokenizer.nextWord();
    Status status = SUCCESS;
    if(strcmp(next, "int")==0) {
    	status = parseDecl(stmt);
    }
    else if(strcmp(next, "if")==0) {
    	status = parseIf(stmt);
    }
    else if(strcmp(next, "else")==0) {
    	status = parseElse(stmt);
    }
    else if(strcmp(next, "while")==0) {
    	status = parseWhile(stmt);
    }
    else {
    	stmt->name = next;
    	status = parseAssign(stmt);
    }
	delete next;
	return status;
}

Status InstrParser::parseDecl(Stmt* stmt) {
	Status status = SUCCESS;
	stmt->type = DECL;
	char* next = p_tokenizer.nextWord();
	if(next == nullptr) return FAILURE;
	stmt->name = next;
	char equal = p_tokenizer.nextChar();
	if (equal == '=') {
		next = p_tokenizer.nextWord();
		if(next == nullptr) return FAILURE;
		Expr* value = p_exprParser.parseExpressionStr(next);
		if(value == nullptr) return FAILURE;
		stmt->value = value;
	}
	else return FAILURE;
	cout <<"Declarative stmt: " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseAssign(Stmt* stmt) {
	Status status = SUCCESS;
	stmt->type = ASSIGN;
	char equal = p_tokenizer.nextChar();
	if ( equal == '=' ) {
		char* next = p_tokenizer.nextWord();
		if(next == nullptr) return FAILURE;
		Expr* value = p_exprParser.parseExpressionStr(next);
		delete next;
		if(value == nullptr) return FAILURE;
		stmt->value = value;
	}
	else return FAILURE;
	cout <<"Assignment stmt: " <<*stmt <<endl;
	return status;
}

Status InstrParser::parseIf(Stmt* stmt) {
	Status status = SUCCESS;
	stmt->type = IF;
	char openBrace = p_tokenizer.nextChar();
	if ( openBrace == '(') {
		char* next = p_tokenizer.nextWord();
		if(next == nullptr) return FAILURE;
		Expr* value = p_exprParser.parseExpressionStr(next);
		delete next;
		if(value == nullptr) return FAILURE;
		stmt->condition = value;
		p_tokenizer.nextLine();
	    parseBlock(stmt->subStatements);
	    cout <<"If stmt: " <<stmt->condition << " " <<stmt->subStatements.size()<<endl;
	}
	else return FAILURE;
	return status;
}

Status InstrParser::parseElse(Stmt* stmt) {
	Status status = SUCCESS;
	stmt->type = ELSE;
	p_tokenizer.nextLine();
	parseBlock(stmt->subStatements);
	cout <<"Else stmt " <<stmt->subStatements.size()<<endl;
	return status;
}

Status InstrParser::parseWhile(Stmt* stmt) {
	Status status = SUCCESS;
	stmt->type = WHILE;
	char openBrace = p_tokenizer.nextChar();
	if ( openBrace == '(') {
		char* next = p_tokenizer.nextWord();
		if(next == nullptr) return FAILURE;
		Expr* value = p_exprParser.parseExpressionStr(next);
		delete next;
		if(value == nullptr) return FAILURE;
		stmt->condition = value;
		p_tokenizer.nextLine();
	    parseBlock(stmt->subStatements);
	    cout <<"While stmt " <<stmt->condition << " " <<stmt->subStatements.size()<<endl;
	}
	else return FAILURE;
	return status;
}

