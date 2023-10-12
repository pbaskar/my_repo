/*
 * SymbolTable.cpp
 *
 *  Created on: Oct 11, 2023
 *      Author: prbas_000
 */

#include "SymbolTable.h"

SymbolTable::SymbolTable() {

}

SymbolTable::~SymbolTable() {
	for(auto symbol : p_symbolSet) {
		delete(symbol);
	}
}

void SymbolTable::makeSymbolTableEntry(char* name) {
	SymbolTableEntry* symbolTableEntry = new SymbolTableEntry(new Variable(name), INT, 0);
	p_symbolSet.insert(symbolTableEntry);
}

SymbolTableEntry::SymbolTableEntry() : p_var(0), p_dataType(INT), p_lineNum(0){

}

SymbolTableEntry::SymbolTableEntry(Variable* var, DataType dataType, int lineNum) : p_var(var), p_dataType(dataType), p_lineNum(lineNum){

}

SymbolTableEntry::~SymbolTableEntry() {
	delete p_var;
}
