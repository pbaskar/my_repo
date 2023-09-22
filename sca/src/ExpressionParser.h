/*
 * ExpressionParser.h
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#ifndef EXPRESSIONPARSER_H_
#define EXPRESSIONPARSER_H_

#include "ExpressionTokenizer.h"

class ExpressionParser {
public:
	ExpressionParser();
	virtual ~ExpressionParser();

	Expr* parseExpressionStr(char* expressionStr);
	Expr* parseExpression();
	Expr* makeNewLeaf(char* token, ExprType type);
private:
	ExpressionTokenizer p_exprTokenizer;
};

#endif /* EXPRESSIONPARSER_H_ */
