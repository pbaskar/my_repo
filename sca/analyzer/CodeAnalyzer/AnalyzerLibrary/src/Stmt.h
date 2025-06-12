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

using namespace std;
enum StmtType { DECL, ASSIGN, IF, ELSE, WHILE, FUNC_DECL, FUNC_CALL };

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
    AssignStmt(StmtType type, Expr* var, Expr* value) : Stmt(type), p_var(var), p_value(value), p_dataType(INT){
    }
    virtual ~AssignStmt() {
        if(p_var && p_var->getExprType() != VARIABLE)
            delete p_var;
        if(p_value && p_value->getExprType() != VARIABLE)
            delete p_value;
    }
    virtual void print(ostream& os) {
        //os << "Assign statement: name " <<*p_var << " type " << p_type;
        if(p_value) os << " value " << *p_value << " ";
    }
    void setVar(Expr* var) { p_var = var; }
    void setValue(Expr* value) { p_value = value; }
    const Expr* getVar() const { return p_var; }
    const Expr* getValue() const { return p_value; }
private:
    Expr* p_var;
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
    Variable* addSymbol(char* name, VarType varType) {
        return p_symbolTable->addSymbol(name, varType);
    }
    Variable* fetchVariable(char* name) {
        return p_symbolTable->fetchVariable(name);
    }
    const vector<Stmt*>& getSubStatements() const { return p_subStatements; }
    SymbolTable* getSymbolTable() const { return p_symbolTable; }
private:
    vector<Stmt*> p_subStatements;
    SymbolTable* p_symbolTable;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(StmtType type): Stmt(type), p_condition(0), p_block(0) {}
    virtual ~WhileStmt() {
        if(p_condition && p_condition->getExprType() != VARIABLE)
            delete p_condition;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << "type " << p_type <<
        if(p_condition)
            os << " condition " <<*p_condition <<" ";
    }
    const Expr* getCondition() const { return p_condition; }
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
        if(p_condition && p_condition->getExprType() != VARIABLE)
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
    }
    virtual void print(ostream& os) {
        os << " Function name " << p_name << " arguments ";
        for(Variable* v : p_formalArguments) {
            os << *v << " ";
        }
    }
    Block* getBlock() const { return p_block; }
    const char* getName() const { return p_name; }
    const vector<Variable*>& getFormalArguments() const { return p_formalArguments; }
    void setBlock(Block* block) {p_block = block; }
    void setName(const char* name) { p_name = name; }
    void addFormalArgument(Variable* argument) { p_formalArguments.push_back(argument); }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    const char* p_name;
    vector<Variable*> p_formalArguments;
    Block* p_block;
};

class FunctionCallStmt : public Stmt {
public:
    FunctionCallStmt(StmtType type): Stmt(type), p_name(0) {}
    virtual ~FunctionCallStmt() {

    }
    virtual void print(ostream& os) {
        os << " Function name " << p_name << " arguments ";
        for(Expr* v : p_actualArguments) {
            os << *v << " ";
        }
    }
    const char* getName() const { return p_name; }
    const vector<Expr*>& getActualArguments() const { return p_actualArguments; }
    void setName(const char* name) { p_name = name; }
    void addActualArgument(Expr* argument) { p_actualArguments.push_back(argument); }
private:
    const char* p_name;
    vector<Expr*> p_actualArguments;
};

#endif /* SRC_STMT_H_ */
