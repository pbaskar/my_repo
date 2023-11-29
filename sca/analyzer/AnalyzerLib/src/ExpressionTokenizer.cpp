/*
 * ExpressionTokenizer.cpp
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */
#include<cstring>
#include<cstdlib>
#include "ExpressionTokenizer.h"

static const char delimiters[] = { '+', '-', '*', '/' };

ExpressionTokenizer::ExpressionTokenizer(): p_expressionStr(nullptr), p_pos(0) {
	// TODO Auto-generated constructor stub

}

ExpressionTokenizer::~ExpressionTokenizer() {
	// TODO Auto-generated destructor stub
}

static bool isDelimiter(char c) {
	for(size_t i=0; i<sizeof(delimiters); i++) {
		if(c == delimiters[i])
			return true;
	}
	return false;
}

char ExpressionTokenizer::nextChar(bool peek) {
	char next = p_expressionStr[p_pos];
	if(!peek && next != '\0')
		p_pos++;
	return next;
}

char* ExpressionTokenizer::nextWord(ExprType& type, bool peek) {
	int p = p_pos;

	if(p_expressionStr[p] == '\0') {
		return 0;
	}
	while(p_expressionStr[p] == ' ') {
		p++;
		p_pos++;
	}
	if(isDelimiter(p_expressionStr[p])) {
		type = OPERATOR;
		p++;
	}
	else {
		if((p_expressionStr[p] >= 'A' && p_expressionStr[p] <= 'Z') || (p_expressionStr[p] >= 'a' && p_expressionStr[p] <= 'z') )
			type = VARIABLE;
		else
			type = CONSTANT;
		while( p_expressionStr[p] != ' ' && p_expressionStr[p] != '\0' && !isDelimiter(p_expressionStr[p])) {
			p++;
		}
	}

	char* token = new char[p-p_pos+1];
	strncpy(token, p_expressionStr + p_pos, p-p_pos);
	token[p-p_pos] = '\0';

	while(p_expressionStr[p] == ' ') {
		p++;
	}
	if(!peek)
		p_pos = p ;
	return token;
}

void ExpressionTokenizer::setExpressionStr(char* expressionStr) {
	p_expressionStr = expressionStr;
	p_pos=0;
}

