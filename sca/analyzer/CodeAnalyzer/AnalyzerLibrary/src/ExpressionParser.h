/*
 * ExpressionParser.h
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#ifndef EXPRESSIONPARSER_H_
#define EXPRESSIONPARSER_H_

#include "ExpressionTokenizer.h"
#include "SymbolTable.h"

class ExpressionParser {
public:
    ExpressionParser();
    virtual ~ExpressionParser();

    Expr* parseExpressionStr(char* expressionStr);
    vector<Expr*> parseExpressionList(char* expressionStr);
    Expr* parseExpression();
    Expr* parseAssignmentExpression();
    Expr* parseUnaryExpression();
    Expr* parsePostFixExpression();
    void parsePostFixExpressionPrime(vector<Expr*>& postFixExprPrime, ExprType& type);
    Expr* parsePrimaryExpression();
    void parseArgumentExpressionList(vector<Expr*>& argumentExprList);
    Expr* parseMultiplicativeExpression();
    Expr* parseAdditiveExpression();
    Expr* makeNewLeaf(char* token, ExprType type);
    void setSymbolTable(const SymbolTable* symbolTable) { p_symbolTable = symbolTable; }
private:
    ExpressionTokenizer p_exprTokenizer;
    const SymbolTable* p_symbolTable;
};

#endif /* EXPRESSIONPARSER_H_ */
