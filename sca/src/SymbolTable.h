/*
 * SymbolTable.h
 *
 *  Created on: Oct 11, 2023
 *      Author: prbas_000
 */

#ifndef SRC_SYMBOLTABLE_H_
#define SRC_SYMBOLTABLE_H_
#include<vector>
#include<cstring>
#include "ExpressionTokenizer.h"
using namespace std;

enum DataType {
	INT,
	DOUBLE
};

class SymbolTableEntry {
public:
	SymbolTableEntry();
	SymbolTableEntry(Variable* var, DataType dataType, int lineNum);
	virtual ~SymbolTableEntry();
	bool operator==(const SymbolTableEntry& other) {
		return p_var==other.p_var;
	}
	Variable* fetchVariable(char* name);
private:
	Variable* p_var;
	DataType p_dataType;
	int p_lineNum;
};

class SymbolTable {
public:
	SymbolTable();
	SymbolTable(SymbolTable* outerScope);
	virtual ~SymbolTable();
	Variable* addSymbol(char* name);
	Variable* fetchVariable(char* name);
private:
	vector<SymbolTableEntry*> p_symbolEntries;
	SymbolTable* p_outerScope;
};

#endif /* SRC_SYMBOLTABLE_H_ */
