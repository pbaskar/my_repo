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
#include<vector>
#include "Expr.h"

using namespace std;

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
