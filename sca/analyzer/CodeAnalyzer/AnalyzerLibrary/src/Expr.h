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

using namespace std;
enum ExprType {
    CONSTANT,
    VARIABLE,
    OPERATOR,
    UNARYOPERATOR,
    INVALID
};

enum VarType {
    VALUE,
    POINTER
};

class Expr {
public:
    Expr() {}
    virtual ~Expr() {}
    virtual ExprType getExprType()=0;
    virtual void print(ostream& os) const=0;
    virtual void getVariables(vector<const Expr*>& variables) const =0;

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
        if(p_left->getExprType() != VARIABLE)
            delete p_left;
        if(p_right->getExprType() != VARIABLE)
            delete p_right;
    }
    virtual ExprType getExprType() { return OPERATOR; }
    void setLeftOp(Expr* left) { p_left = left; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< *p_left <<p_op << " " <<*p_right;
    }
    virtual void getVariables(vector<const Expr*>& variables) const {
        p_left->getVariables(variables);
        p_right->getVariables(variables);
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
        if(p_right->getExprType() != VARIABLE)
            delete p_right;
    }
    virtual ExprType getExprType() { return UNARYOPERATOR; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< p_op <<*p_right;
    }
    virtual void getVariables(vector<const Expr*>& variables) const {
        p_right->getVariables(variables);
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
    virtual void getVariables(vector<const Expr*>& variables) const { variables.push_back(this); }
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

class Constant : public Expr {
public:
    Constant(int n): p_number(n) {}
    virtual ~Constant() {}
    virtual ExprType getExprType() { return CONSTANT; }
    virtual void getVariables(vector<const Expr*>& variables) const {}
    void setNumber(int num) { p_number = num; }
    virtual void print(ostream& os) const {
        os <<p_number <<" ";
    }
private:
    int p_number;
};

#endif /* EXPR_H_ */
