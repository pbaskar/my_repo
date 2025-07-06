/*
 * Stmt.h
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */

#ifndef SRC_STMT_H_
#define SRC_STMT_H_

#include "Expr.h"
#include "SymbolTable.h"
#include<list>

using namespace std;
enum StmtType { DECL, ASSIGN, IF, ELSE, WHILE, FUNC_DECL, FUNC_CALL };

class IdentifierName {
public:
    IdentifierName(const char* n): p_name(n) {}
    virtual ~IdentifierName() { /*delete p_name;*/ }
    friend ostream& operator<<(ostream& os, IdentifierName& identifierName) {
        identifierName.print(os);
        return os;
    }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const IdentifierName& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
};

class FunctionIdentifierName : public IdentifierName {
public:
    FunctionIdentifierName(const char* name, vector<IdentifierName*> parameterList):
        IdentifierName(name), p_parameterList(parameterList) {}
    virtual ~FunctionIdentifierName() {
        for(int i=0; i<p_parameterList.size(); i++) {
            delete p_parameterList[i];
        }
    }
    friend ostream& operator<<(ostream& os, FunctionIdentifierName& functionIdentifierName) {
        functionIdentifierName.print(os);
        return os;
    }
    virtual void print(ostream& os) const{
        IdentifierName::print(os);
        os <<"arguments ";
        for(int i=0; i<p_parameterList.size(); i++) {
            os <<*p_parameterList[i] << " ";
        }
    }
    const vector<IdentifierName*>& getParameterList() { return p_parameterList; }
private:
    vector<IdentifierName*> p_parameterList;
};

class PointerIdentifierName : public IdentifierName {
public:
    PointerIdentifierName(const char* name, IdentifierName* pointsTo):
        IdentifierName(name), p_pointsTo(pointsTo) {}
    virtual ~PointerIdentifierName() {
        delete p_pointsTo;
    }
    virtual void print(ostream& os) const{
        IdentifierName::print(os);
        os << *p_pointsTo;
    }
    void setPointsTo(IdentifierName* pointsTo) {
        p_pointsTo = pointsTo;
    }
    const IdentifierName* getPointsTo() const {
        return p_pointsTo;
    }
private:
    IdentifierName* p_pointsTo;
};

class Stmt {
public:
    Stmt(StmtType type): p_type(type) {}
    virtual ~Stmt(){}
    friend ostream& operator<<(ostream& os, Stmt& stmt) {
        stmt.print(os);
        return os;
    }
    virtual void print(ostream& os)=0;
    StmtType getType() { return p_type; }
    StmtType p_type;
};

class AssignStmt : public Stmt {
public:
    AssignStmt(StmtType type): Stmt(type), p_var(0), p_value(0), p_dataType(INT) {}
    AssignStmt(StmtType type, IdentifierName* var, Expr* value) : Stmt(type), p_var(var), p_value(value), p_dataType(INT){
    }
    virtual ~AssignStmt() {
        if(p_var)
            delete p_var;
        if(p_value)
            delete p_value;
    }
    virtual void print(ostream& os) {
        if(p_var) os << "Assign statement: name " <<*p_var << " type " << p_type;
        if(p_value) os << " value " << *p_value << " " << p_value->getExprType();
    }
    void setVar(IdentifierName* var) { p_var = var; }
    void setValue(Expr* value) { p_value = value; }
    const IdentifierName* getVar() const { return p_var; }
    const Expr* getValue() const { return p_value; }
    Expr* toSimplify() { return p_value; }
private:
    IdentifierName* p_var;
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
        delete p_symbolTable;
    }
    void addStatement(Stmt* stmt) {
        p_subStatements.push_back(stmt);
    }
    Variable* addSymbol(const char* name, VarType varType) {
        return p_symbolTable->addSymbol(name, varType);
    }
    Variable* fetchVariable(char* name) {
        return p_symbolTable->fetchVariable(name);
    }
    list<Stmt*>& toSimplify() { return p_subStatements; }
    const list<Stmt*>& getSubStatements() const { return p_subStatements; }
    SymbolTable* getSymbolTable() const { return p_symbolTable; }
private:
    list<Stmt*> p_subStatements;
    SymbolTable* p_symbolTable;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(StmtType type): Stmt(type), p_condition(0), p_block(0) {}
    virtual ~WhileStmt() {
        if(p_condition)
            delete p_condition;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << "type " << p_type <<
        if(p_condition)
            os << " condition " <<*p_condition <<" ";
    }
    const Expr* getCondition() const { return p_condition; }
    Expr* toSimplifyCondition() { return p_condition; }
    Block* getBlock() const { return p_block; }
    void setCondition(Expr* condition) { p_condition = condition; }
    void setBlock(Block* block) {p_block = block; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    Expr* p_condition;
    Block* p_block;
};

class IfStmt : public Stmt {
public:
    IfStmt(StmtType type): Stmt(type), p_condition(0), p_else(0), p_block(0) {}
    virtual ~IfStmt() {
        if(p_condition)
            delete p_condition;
        delete p_else;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << " type " << p_type << " substatements " <<p_subStatements.size() <<" ";
        if(p_condition)
            os <<" condition " <<*p_condition <<" ";
    }
    const Expr* getCondition() const { return p_condition; }
    Expr* toSimplifyCondition() { return p_condition; }
    const IfStmt* getElse() const { return p_else; }
    Block* getBlock() const { return p_block; }
    void setCondition(Expr* condition) { p_condition = condition; }
    void setElse(IfStmt* elseStmt) { p_else = elseStmt; }
    void setBlock(Block* block) {p_block = block; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    Expr* p_condition;
    IfStmt* p_else;
    Block* p_block;
};

class FunctionDeclStmt : public Stmt {
public:
    FunctionDeclStmt(StmtType type): Stmt(type), p_name(0), p_block(0) {}
    virtual ~FunctionDeclStmt() {
        delete p_block;
        delete p_name;
    }
    virtual void print(ostream& os) {
        os << " Function name " << *p_name;
    }
    Block* getBlock() const { return p_block; }
    IdentifierName* getName() const { return p_name; }
    void setBlock(Block* block) {p_block = block; }
    void setName(IdentifierName* name) { p_name = name; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    IdentifierName* p_name;
    Block* p_block;
};

class FunctionCallStmt : public Stmt {
public:
    FunctionCallStmt(StmtType type, const Expr* name, const vector<Expr*> actualArguments):
        Stmt(type), p_name(name), p_actualArguments(actualArguments) {}
    virtual ~FunctionCallStmt() {
    }
    virtual void print(ostream& os) {
        os << " Function name " << *p_name << " arguments ";
        for(Expr* v : p_actualArguments) {
            os << *v << " ";
        }
    }
    const Expr* getName() const { return p_name; }
    const vector<Expr*>& getActualArguments() const { return p_actualArguments; }
    void setName(Expr* name) { p_name = name; }
    void addActualArgument(Expr* argument) { p_actualArguments.push_back(argument); }
private:
    const Expr* p_name;
    vector<Expr*> p_actualArguments;
};

#endif /* SRC_STMT_H_ */
