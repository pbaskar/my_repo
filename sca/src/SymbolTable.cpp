/*
 * SymbolTable.cpp
 *
 *  Created on: Oct 11, 2023
 *      Author: prbas_000
 */

#include "SymbolTable.h"
#include<cstring>
#include "Expr.h"
#include "BasicBlock.h" // FunctionDeclBlock

SymbolTable::SymbolTable() : p_outerScope(0) {
}

SymbolTable::SymbolTable(SymbolTable* outerScope) : p_outerScope(outerScope) {
}

SymbolTable::~SymbolTable() {
	for(auto symbol : p_symbolEntries) {
		delete(symbol);
	}
	for(auto fnSymbol : p_fnSymbolEntries) {
		delete(fnSymbol);
	}
}

Variable* SymbolTable::addSymbol(const char* name) {
	Variable* var(0);

	for(auto symbol : p_symbolEntries) {
		var = symbol->fetchVariable(name);
		if(var) break;
	}
	if(var) { //error
	}
	var = new Variable(name);
	SymbolTableEntry* symbolTableEntry = new SymbolTableEntry(var, INT, 0);
	p_symbolEntries.push_back(symbolTableEntry);
	return var;
}

Variable* SymbolTable::fetchVariable(const char* name) const {
	Variable* var(0);
	for(auto symbol : p_symbolEntries) {
		var = symbol->fetchVariable(name);
		if(var) break;
	}
	if(var) { return var; }

	const SymbolTable* outerScope(p_outerScope);
	while(outerScope) {
		var = outerScope->fetchVariable(name);
		if(var) break;
		outerScope = outerScope->getOuterScope();
	}
	if(var) { return var; }
	return 0;
}

FunctionDeclBlock* SymbolTable::addFnSymbol(FunctionDeclBlock* fnDeclBlock) {
	if(!fnDeclBlock) { //error
	}
	FnSymbolTableEntry* symbolTableEntry = new FnSymbolTableEntry(fnDeclBlock, INT, 0);
	p_fnSymbolEntries.push_back(symbolTableEntry);
	return fnDeclBlock;
}

FunctionDeclBlock* SymbolTable::fetchFunctionDeclBlock(const char* name) const {
	FunctionDeclBlock* fnDeclBlock(0);

	for(auto symbol : p_fnSymbolEntries) {
		fnDeclBlock = symbol->fetchFunctionDeclBlock(name);
		if(fnDeclBlock) break;
	}
	if(fnDeclBlock) { return fnDeclBlock; }
	const SymbolTable* outerScope(p_outerScope);
	while(outerScope) {
		fnDeclBlock = outerScope->fetchFunctionDeclBlock(name);
		if(fnDeclBlock) break;
		outerScope = outerScope->getOuterScope();
	}
	if(fnDeclBlock) { return fnDeclBlock; }
	return 0;
}

SymbolTableEntry::SymbolTableEntry() : p_var(0), p_dataType(INT), p_lineNum(0){

}

SymbolTableEntry::SymbolTableEntry(Variable* var, DataType dataType, int lineNum) :
								p_var(var), p_dataType(dataType), p_lineNum(lineNum){

}

SymbolTableEntry::~SymbolTableEntry() {
	delete p_var;
}

Variable* SymbolTableEntry::fetchVariable(const char* name) {
	if(p_var->match(name)) return p_var;
	return 0;
}

FnSymbolTableEntry::FnSymbolTableEntry() : p_functionDeclBlock(0), p_dataType(INT), p_lineNum(0){

}

FnSymbolTableEntry::FnSymbolTableEntry(FunctionDeclBlock* fnDeclBlock, DataType dataType, int lineNum) :
								p_functionDeclBlock(fnDeclBlock), p_dataType(dataType), p_lineNum(lineNum){

}

FnSymbolTableEntry::~FnSymbolTableEntry() {
	delete p_functionDeclBlock;
}

FunctionDeclBlock* FnSymbolTableEntry::fetchFunctionDeclBlock(const char* name) {
	if(p_functionDeclBlock->match(name)) return p_functionDeclBlock;
	return 0;
}
