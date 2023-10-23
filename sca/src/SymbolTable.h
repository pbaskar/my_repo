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

class FunctionDeclBlock;
class Variable;
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

class FnSymbolTableEntry {
public:
	FnSymbolTableEntry();
	FnSymbolTableEntry(FunctionDeclBlock* functionDeclBlock, DataType dataType, int lineNum);
	virtual ~FnSymbolTableEntry();
	bool operator==(const FnSymbolTableEntry& other) {
		return p_functionDeclBlock==other.p_functionDeclBlock;
	}
	FunctionDeclBlock* fetchFunctionDeclBlock(char* name);
private:
	FunctionDeclBlock* p_functionDeclBlock;
	DataType p_dataType;
	int p_lineNum;
};

class SymbolTable {
public:
	SymbolTable();
	SymbolTable(SymbolTable* outerScope);
	virtual ~SymbolTable();
	Variable* addSymbol(char* name);
	Variable* fetchVariable(char* name) const;
	FunctionDeclBlock* addFnSymbol(FunctionDeclBlock* fnDeclBlock);
	FunctionDeclBlock* fetchFunctionDeclBlock(char* name) const;
private:
	vector<SymbolTableEntry*> p_symbolEntries;
	vector<FnSymbolTableEntry*> p_fnSymbolEntries;
	SymbolTable* p_outerScope;
};

#endif /* SRC_SYMBOLTABLE_H_ */
