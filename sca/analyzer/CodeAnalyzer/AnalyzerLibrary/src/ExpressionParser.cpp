/*
 * ExpressionParser.cpp
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include "ExpressionParser.h"

using namespace std;

static const char operators[] = { '+', '-', '*', '/' };
ExpressionParser::ExpressionParser(): p_symbolTable(0) {
    // TODO Auto-generated constructor stub

}

ExpressionParser::~ExpressionParser() {
    // TODO Auto-generated destructor stub
}

Expr* ExpressionParser::parseExpressionStr(char* expressionStr) {
    p_exprTokenizer.setExpressionStr(expressionStr);
    Expr* expr = parseExpression();
    return expr;
}

Expr* ExpressionParser::makeNewLeaf(char* token, ExprType type) {
    Expr* expr = nullptr;
    switch(type) {
    case CONSTANT: {
        int number = atoi(token);
        delete token;
        expr = new Constant(number);
    }
    break;
    case VARIABLE: {
        expr = p_symbolTable->fetchVariable(token);
    }
    break;
    default: return nullptr; //failure
    }
    return expr;
}

Expr* ExpressionParser::parseFactor() {
    ExprType type = CONSTANT;
    char* token = p_exprTokenizer.nextWord(type);
    //cout <<"Factor: first token " <<token << type <<endl;
    Expr* leftOp = makeNewLeaf(token, type);

    char o = p_exprTokenizer.nextChar(true);
    //cout <<"Factor: op " <<o <<endl;

    if(o == '\0' || o == '+') return leftOp;

    char* op = p_exprTokenizer.nextWord(type);
    Expr* rightOp = parseFactor();

    Expr* oper = new Operator(leftOp,op[0],rightOp);
    delete op;
    return oper;
}

Expr* ExpressionParser::parseExpression() {
    ExprType type = CONSTANT;
    Expr* leftOp = parseFactor();

    if(!leftOp) return leftOp;

    char* op = p_exprTokenizer.nextWord(type);
    //if(op) cout <<"op " <<op <<endl;

    if(!op) return leftOp;

    Expr* rightOp = parseExpression();

    Expr* oper = new Operator(leftOp,op[0],rightOp);
    delete op;
    return oper;
}

