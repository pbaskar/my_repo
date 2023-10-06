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

using namespace std;
enum StmtType { DECL, ASSIGN, IF, ELSE, WHILE };
enum DataType { INT };

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
	AssignStmt(): p_name(0), p_value(0), p_dataType(INT) {}
	AssignStmt(char* name, Expr* value) : p_name(name), p_value(value), p_dataType(INT){
	}
	virtual ~AssignStmt() {
		//delete p_name;
		//delete p_value;
	}
	virtual void print(ostream& os) {
		os << "Assign statement: name " <<p_name << " type " << p_type << " value " << *p_value << " ";
	}
	char* p_name;
	Expr* p_value;
	DataType p_dataType;
};

class WhileStmt : public Stmt {
public:
	WhileStmt(): p_condition(0) {}
	WhileStmt(Expr* condition): p_condition(condition) {}
	virtual ~WhileStmt() {
		//delete p_condition;
		for(Stmt* stmt : p_subStatements) {
			delete stmt;
		}
	}
	virtual void print(ostream& os) {
		os << "type " << p_type << " substatements " <<p_subStatements.size() << " ";
		if(p_condition)
			os << " condition " <<*p_condition <<" ";
	}
	Expr* p_condition;
	vector<Stmt*> p_subStatements;
};

class IfStmt : public Stmt {
public:
	IfStmt(): p_condition(0), p_else(0) {}
	IfStmt(Expr* condition): p_condition(condition), p_else(0) {}
	virtual ~IfStmt() {
		//delete p_condition;
		delete p_else;
		for(Stmt* stmt : p_subStatements) {
			delete stmt;
		}
	}
	virtual void print(ostream& os) {
		os << " type " << p_type << " substatements " <<p_subStatements.size() <<" ";
		if(p_condition)
			os <<" condition " <<*p_condition <<" ";
	}
	Expr* p_condition;
	vector<Stmt*> p_subStatements;
	IfStmt* p_else;
};

class InstrParser {
public:
	InstrParser();
	virtual ~InstrParser();

	Status parseFile(char* fileName);
	Status parseStmt(vector<Stmt*>& stmtList);
	Status parseDecl(vector<Stmt*>& stmtList);
	Status parseAssign(vector<Stmt*>& stmtList);
	Status parseIf(IfStmt* stmt);
	Status parseElse(IfStmt* stmt);
	Status parseIfElse(vector<Stmt*>& stmtList);
	Status parseWhile(vector<Stmt*>& stmtList);
	Status parseBlock(vector<Stmt*>& stmtList);
	const vector<Stmt*>& getStatementList();

private:
	vector<Stmt*> p_stmtList;
	Tokenizer p_tokenizer;
	ExpressionParser p_exprParser;

	int p_pos;
};

#endif /* INSTRPARSER_H_ */
