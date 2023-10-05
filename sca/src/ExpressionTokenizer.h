/*
 * ExpressionTokenizer.h
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#ifndef EXPRESSIONTOKENIZER_H_
#define EXPRESSIONTOKENIZER_H_
#include<iostream>
#include<cstring>

using namespace std;
enum ExprType {
	CONSTANT,
	VARIABLE,
	OPERATOR,
	INVALID
};

class Expr {
public:
	Expr() {}
	virtual ~Expr() {}
	virtual ExprType getExprType()=0;
	virtual void print(ostream& os) =0;

	friend ostream& operator<<(ostream& os, Expr& expr) {
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
	virtual void print(ostream& os) {
		os<< *p_left <<p_op << " " <<*p_right;
	}
private:
	Expr* p_left;
	char p_op;
	Expr* p_right;
};

class Variable : public Expr {
public:
	Variable(char* n): p_name(n) {}
	virtual ~Variable() { delete p_name; }
	virtual ExprType getExprType() { return VARIABLE; }
	void setName(char* name) { p_name = name; }
	virtual void print(ostream& os) {
		os <<p_name << " ";
	}
private:
	char* p_name;
};

class Constant : public Expr {
public:
	Constant(int n): p_number(n) {}
	virtual ~Constant() {}
	virtual ExprType getExprType() { return CONSTANT; }
	void setNumber(int num) { p_number = num; }
	virtual void print(ostream& os) {
		os <<p_number <<" ";
	}
private:
	int p_number;
};

class ExpressionTokenizer {
public:
	ExpressionTokenizer();
	virtual ~ExpressionTokenizer();

	char nextChar(bool peek=false);
	char* nextWord(ExprType& type,bool peek=false);
	void setExpressionStr(char* expressionStr);

private:
	char* p_expressionStr;
	int p_pos;
};

#endif /* EXPRESSIONTOKENIZER_H_ */
