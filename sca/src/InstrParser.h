/*
 * InstrParser.h
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */

#ifndef INSTRPARSER_H_
#define INSTRPARSER_H_
#include<vector>
#include "Tokenizer.h"
#include "ExpressionParser.h"
#include "SymbolTable.h"

using namespace std;
enum StmtType { DECL, ASSIGN, IF, ELSE, WHILE };

class Stmt {
public:
	Stmt(): p_type(DECL) {}
	virtual ~Stmt(){}
	friend ostream& operator<<(ostream& os, Stmt& stmt) {
		stmt.print(os);
		return os;
	}
	virtual void print(ostream& os)=0;
	StmtType p_type;
};

class AssignStmt : public Stmt {
public:
	AssignStmt(): p_var(0), p_value(0), p_dataType(INT) {}
	AssignStmt(Variable* var, Expr* value) : p_var(var), p_value(value), p_dataType(INT){
	}
	virtual ~AssignStmt() {
		//delete p_name;
		//delete p_value;
	}
	virtual void print(ostream& os) {
		os << "Assign statement: name " <<*p_var << " type " << p_type << " value " << *p_value << " ";
	}
	Variable* p_var;
	Expr* p_value;
	DataType p_dataType;
};

class Block {
public:
	Block(){ p_symbolTable = new SymbolTable; }
	Block(const Block* parent) { p_symbolTable = new SymbolTable(parent->p_symbolTable); }
	~Block() {
		for(Stmt* stmt : p_subStatements) {
			delete stmt;
		}
		//delete p_symbolTable;
	}
	void addStatement(Stmt* stmt) {
		p_subStatements.push_back(stmt);
	}
	Variable* addSymbol(char* name) {
		return p_symbolTable->addSymbol(name);
	}
	Variable* fetchVariable(char* name) {
		return p_symbolTable->fetchVariable(name);
	}
	const vector<Stmt*>& getSubStatements() const { return p_subStatements; }
	const SymbolTable* getSymbolTable() const { return p_symbolTable; }
private:
	vector<Stmt*> p_subStatements;
	SymbolTable* p_symbolTable;
};

class WhileStmt : public Stmt {
public:
	WhileStmt(): p_condition(0), p_block(0) {}
	virtual ~WhileStmt() {
		//delete p_condition;
		delete p_block;
	}
	virtual void print(ostream& os) {
		//os << "type " << p_type <<
		if(p_condition)
			os << " condition " <<*p_condition <<" ";
	}
	const Block* getBlock() const { return p_block; }
	Expr* p_condition;
	Block* p_block;
};

class IfStmt : public Stmt {
public:
	IfStmt(): p_condition(0), p_else(0), p_block(0) {}
	virtual ~IfStmt() {
		//delete p_condition;
		delete p_else;
		delete p_block;
	}
	virtual void print(ostream& os) {
		//os << " type " << p_type << " substatements " <<p_subStatements.size() <<" ";
		if(p_condition)
			os <<" condition " <<*p_condition <<" ";
	}
	const Block* getBlock() const { return p_block; }
	Expr* p_condition;
	IfStmt* p_else;
	Block* p_block;
};

class InstrParser {
public:
	InstrParser();
	virtual ~InstrParser();

	Status parseFile(char* fileName);
	Status parseStmt(Block* block);
	Status parseDecl(Block* block);
	Status parseAssign(Block* block);
	Status parseIf(IfStmt* stmt);
	Status parseElse(IfStmt* stmt);
	Status parseIfElse(Block* block);
	Status parseWhile(Block* block);
	Status parseBlock(Block* block);
	const Block* getBlock() const;

private:
	Block* p_mainBlock;
	Tokenizer p_tokenizer;
	ExpressionParser p_exprParser;

	int p_pos;
};

#endif /* INSTRPARSER_H_ */
