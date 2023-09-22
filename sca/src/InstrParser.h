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
enum dataType { DECL, ASSIGN, IF, ELSE, WHILE };

struct Stmt {

	Stmt(): name(0), type(DECL), value(0),condition(0) {}
	~Stmt(){
		cout<<"Stmt destroyed " << type <<endl;
		delete name;
		delete value;
		delete condition;
		for(Stmt* stmt : subStatements) {
			delete stmt;
		}
	}
    friend ostream& operator<<(ostream& os, Stmt& stmt) {
    	os << "statement: name " <<stmt.name << " type " << stmt.type << " value " << *stmt.value
				<< " substatements " <<stmt.subStatements.size() <<endl;
    	return os;
    }
    char* name;
    dataType type;
    Expr* value;

    Expr* condition;
    vector<Stmt*> subStatements;
};

class InstrParser {
public:
	InstrParser();
	virtual ~InstrParser();

	Status parseFile(char* fileName);
	Status parseStmt(Stmt* stmt);
	Status parseDecl(Stmt* stmt);
	Status parseAssign(Stmt* stmt);
	Status parseIf(Stmt* stmt);
	Status parseElse(Stmt* stmt);
	Status parseWhile(Stmt* stmt);
	Status parseBlock(vector<Stmt*>& stmtList);

private:
    vector<Stmt*> p_stmtList;
    Tokenizer p_tokenizer;
    ExpressionParser p_exprParser;
};

#endif /* INSTRPARSER_H_ */
