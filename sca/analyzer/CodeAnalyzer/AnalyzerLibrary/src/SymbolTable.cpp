/*
 * SymbolTable.cpp
 *
 *  Created on: Oct 11, 2023
 *      Author: prbas_000
 */

#include "SymbolTable.h"
#include <cassert>
#include<cstring>
#include "Expr.h"
#include "BasicBlock.h" // FunctionDeclBlock
#include "Traverser.h"

SymbolTable::SymbolTable() : p_outerScope(0) {
}

SymbolTable::SymbolTable(SymbolTable* outerScope) : p_outerScope(outerScope) {
}

void SymbolTable::clearFnSymbolEntries() {
    for(auto fnSymbol : p_fnSymbolEntries) {
        delete(fnSymbol);
    }
}

SymbolTable::~SymbolTable() {
    for(auto symbol : p_symbolEntries) {
        delete(symbol);
    }
}

Variable* SymbolTable::addSymbol(const char* name, VarType varType) {
    Variable* var(0);

    for(auto symbol : p_symbolEntries) {
        var = symbol->fetchVariable(name);
        if(var) break;
    }
    if(var) { //error
    }
    var = new Variable(name, varType);
    cout <<"Variable symbol added " <<name <<endl;
    SymbolTableEntry* symbolTableEntry = new SymbolTableEntry(var, INT, 0);
    p_symbolEntries.push_back(symbolTableEntry);
    return var;
}

Variable* SymbolTable::addSymbol(Variable* newVar) {
    Variable* var(0);
    for(vector<SymbolTableEntry*>::iterator it = p_symbolEntries.begin(); it != p_symbolEntries.end(); it++) {
        var = (*it)->fetchVariable(newVar->getName());
        if(var) {
            break;
        }
    }
    if(var) { //error
        cout <<"Adding duplicate variable into symbol table " <<endl;
        assert(false);
    }
    cout <<"Variable symbol added " <<newVar->getName() <<endl;
    SymbolTableEntry* symbolTableEntry = new SymbolTableEntry(newVar, INT, 0);
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

void SymbolTable::addFnSymbol(const char* name) {
    FnSymbolTableEntry* symbolTableEntry = new FnSymbolTableEntry(name, 0, INT, 0);
    p_fnSymbolEntries.push_back(symbolTableEntry);
}

void SymbolTable::setFunctionDeclBlock(FunctionDeclBlock* fnDeclBlock) {
    if(!fnDeclBlock) {
        cout << "SymbolTable::setFunctionDeclBlock null " <<endl;
        return; //error
    }
    const char* name = fnDeclBlock->getName();
    for(auto symbol : p_fnSymbolEntries) {
        if(strcmp(symbol->getName(), name) == 0 ) {
            symbol->setFunctionDeclBlock(fnDeclBlock);
        }
    }
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

FnSymbolTableEntry::FnSymbolTableEntry() : p_name(0), p_functionDeclBlock(0), p_dataType(INT), p_lineNum(0){

}

FnSymbolTableEntry::FnSymbolTableEntry(const char* name, FunctionDeclBlock* fnDeclBlock, DataType dataType, int lineNum) :
                                p_name(name), p_functionDeclBlock(fnDeclBlock), p_dataType(dataType), p_lineNum(lineNum){

}

FnSymbolTableEntry::~FnSymbolTableEntry() {
    DeleteVisitor deleteVisitor;
    TraverserOne tOne(&deleteVisitor, true);
    tOne.traverseFunctionDeclBlock(p_functionDeclBlock);
}

FunctionDeclBlock* FnSymbolTableEntry::fetchFunctionDeclBlock(const char* name) {
    //p_functionDeclBlock could be null until populated while building cfg
    //cout <<"FnSymbolTableEntry::fetchFunctionDeclBlock " <<p_functionDeclBlock <<endl;
    if(p_functionDeclBlock && p_functionDeclBlock->match(name)) return p_functionDeclBlock;
    return 0;
}
