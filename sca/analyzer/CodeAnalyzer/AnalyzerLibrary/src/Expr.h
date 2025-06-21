/*
 * Expr.h
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */

#ifndef EXPR_H_
#define EXPR_H_
#include<cstring>
#include<iostream>
#include<vector>

class SymbolTable;
using namespace std;
enum ExprType {
    CONSTANT,
    VARIABLE,
    IDENTIFIER,
    OPERATOR,
    UNARYOPERATOR,
    FUNCTIONCALL,
    ASSIGNOPERATOR,
    INVALID
};

enum VarType {
    VALUE,
    POINTER
};

class FunctionCall;
class Expr {
public:
    Expr() {}
    virtual ~Expr() {}
    virtual ExprType getExprType()=0;
    virtual void print(ostream& os) const=0;
    virtual void getRHSVariables(vector<const Expr*>& variables) const =0;
    virtual void getLHS(vector<const Expr*>& variables) const =0;
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const =0;
    virtual void populateVariable(SymbolTable* symbolTable)=0;

    friend ostream& operator<<(ostream& os, const Expr& expr) {
        //os <<expr.getExprType() <<" " ;
        expr.print(os);
        return os;
    }
};

class Operator : public Expr {
public:
    Operator(Expr* l, char o, Expr* r): p_left(l), p_op(o), p_right(r) {}
    virtual ~Operator() {
        delete p_left;
        delete p_right;
    }
    virtual ExprType getExprType() { return OPERATOR; }
    void setLeftOp(Expr* left) { p_left = left; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< *p_left <<p_op << " " <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_left->getRHSVariables(variables);
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        p_left->getLHS(variables);
        p_right->getLHS(variables);
    }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {
        p_left->getFunctionCalls(functionCalls);
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_left->populateVariable(symbolTable);
        p_right->populateVariable(symbolTable);
    }
private:
    Expr* p_left;
    char p_op;
    Expr* p_right;
};

class AssignOperator : public Expr {
public:
    AssignOperator(Expr* l, char o, Expr* r): p_left(l), p_op(o), p_right(r) {}
    virtual ~AssignOperator() {
        delete p_left;
        delete p_right;
    }
    virtual ExprType getExprType() { return ASSIGNOPERATOR; }
    void setLeftOp(Expr* left) { p_left = left; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< *p_left <<p_op << " " <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        p_left->getLHS(variables);
    }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {
        p_left->getFunctionCalls(functionCalls);
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_left->populateVariable(symbolTable);
        p_right->populateVariable(symbolTable);
    }
private:
    Expr* p_left;
    char p_op;
    Expr* p_right;
};

class UnaryOperator : public Expr {
public:
    UnaryOperator(char o, Expr* r): p_op(o), p_right(r) {}
    virtual ~UnaryOperator() {
        delete p_right;
    }
    virtual ExprType getExprType() { return UNARYOPERATOR; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< p_op <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        p_right->getLHS(variables);
    }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_right->populateVariable(symbolTable);
    }
private:
    char p_op;
    Expr* p_right;
};

class Variable : public Expr {
public:
    Variable(const char* n, VarType type): p_name(n), p_type(type) {}
    virtual ~Variable() { /*delete p_name;*/ }
    virtual ExprType getExprType() { return VARIABLE; }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const { variables.push_back(this); }
    virtual void getLHS(vector<const Expr*>& variables) const { variables.push_back(this); }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const Variable& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
    VarType p_type;
};

class Identifier : public Expr {
public:
    Identifier(const char* n): p_name(n) {}
    virtual ~Identifier() { /*delete p_name;*/ }
    virtual ExprType getExprType() { return IDENTIFIER; }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    void setVariable(Variable* variable) { p_variable = variable; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const { variables.push_back(p_variable); }
    virtual void getLHS(vector<const Expr*>& variables) const { variables.push_back( p_variable); }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable);
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const Identifier& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
    Variable* p_variable;
};

class Constant : public Expr {
public:
    Constant(int n): p_number(n) {}
    virtual ~Constant() {}
    virtual ExprType getExprType() { return CONSTANT; }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {}
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
    void setNumber(int num) { p_number = num; }
    virtual void print(ostream& os) const {
        os <<p_number <<" ";
    }
private:
    int p_number;
};

class FunctionCall : public Expr {
public:
    FunctionCall(Expr* identifier, vector<Expr*> arguments): p_functionName(identifier), p_arguments(arguments) {}
    virtual ~FunctionCall() {
        delete p_functionName;
        for(Expr* expr : p_arguments) {
            delete expr;
        }
    }
    virtual ExprType getExprType() { return FUNCTIONCALL; }
    virtual void print(ostream& os) const{
        os<<*p_functionName << " ";
        for(Expr* expr : p_arguments) {
            os <<*expr << " ";
        }
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        for(Expr* expr : p_arguments) {
            expr->getRHSVariables(variables);
        }
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
    }
    virtual void getFunctionCalls(vector<const FunctionCall*>& functionCalls) const {
        functionCalls.push_back(this);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        for(Expr* expr : p_arguments) {
            expr->populateVariable(symbolTable);
        }
    }
    const Expr* getName() const { return p_functionName; }
    const vector<Expr*> getArguments() const { return p_arguments; }
private:
    Expr* p_functionName;
    vector<Expr*> p_arguments;
};

#endif /* EXPR_H_ */
