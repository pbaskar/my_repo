/*
 * ExpressionParser.cpp
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include "ExpressionParser.h"

using namespace std;

static const char operators[] = { '+', '-', '*', '/', '=' };
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

vector<Expr*> ExpressionParser::parseExpressionList(char* expressionStr) {
    vector<Expr*> expressionList;
    p_exprTokenizer.setExpressionStr(expressionStr);
    while(true) {
        Expr* expr = parseExpression();
        if(expr != nullptr) {
            expressionList.push_back(expr);
        } else break;
        char next = p_exprTokenizer.nextChar(true);
        if(next == ',') {
            p_exprTokenizer.nextChar();
        }
        else break;
    }
    return expressionList;
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
    case IDENTIFIER: {
        expr = new Identifier(token);
    }
    break;
    default: return nullptr; //failure
    }
    return expr;
}

Expr* ExpressionParser::parseExpression() {
    Expr* expr = parseAssignmentExpression();
    return expr;
}

Expr* ExpressionParser::parseAdditiveExpression() {
    Expr* oper = nullptr;
    Expr* leftOp = parseMultiplicativeExpression();
    char o = p_exprTokenizer.nextChar(true);

    //cout <<"AdditiveExpr: op " <<o <<endl;
    //if(o == '\0' || o == '=') { cout <<"return leftop " <<leftOp <<endl; return leftOp; }
    if( o == '+' || o == '-') {
        p_exprTokenizer.nextChar();
        Expr* rightOp = parseAdditiveExpression();
        if(leftOp == nullptr || rightOp == nullptr) {
            delete leftOp;
            delete rightOp;
        }
        else {
            oper = new Operator(leftOp,o,rightOp);
        }
    } else {
        cout << "AdditiveExpression " <<leftOp <<endl;
        return leftOp;
    }
    cout << "AdditiveExpression " <<oper <<endl;
    return oper;
}

Expr* ExpressionParser::parseMultiplicativeExpression() {
    Expr* oper = nullptr;
    Expr* leftOp = parseUnaryExpression();
    char o = p_exprTokenizer.nextChar(true);
    cout <<"MultiplicativeExpr: op " <<o <<endl;

    //if(o == '\0' || o == '=') { cout <<"return leftop " <<leftOp <<endl; return leftOp; }
    if(o == '*' || o == '/') {
        p_exprTokenizer.nextChar();

        Expr* rightOp = parseMultiplicativeExpression();
        if(leftOp == nullptr || rightOp == nullptr) {
            delete leftOp;
            delete rightOp;
        }
        else {
            oper = new Operator(leftOp,o,rightOp);
        }
    } else {
        return leftOp;
    }
    cout << "MultiplicativeExpression " <<oper <<endl;
    return oper;
}

void ExpressionParser::parseArgumentExpressionList(vector<Expr*>& argumentExprList) {
    Expr* argumentExpr = parseAdditiveExpression();

    if(argumentExpr != nullptr) {
        argumentExprList.push_back(argumentExpr);
        char next = p_exprTokenizer.nextChar(true);
        cout <<"next char " <<next <<endl;
        if(next == ',') {
            p_exprTokenizer.nextChar();
            parseArgumentExpressionList(argumentExprList);
        }
    }
    return;
}

Expr* ExpressionParser::parsePrimaryExpression() {
    Expr* primaryExpr = nullptr;
    char o = p_exprTokenizer.nextChar(true);
    if(o == '(') {
        p_exprTokenizer.nextChar(); //consume '('
        primaryExpr = parseExpression();
        p_exprTokenizer.nextChar(); //consume ')'
    } else {
        ExprType type = CONSTANT;
        char* token = p_exprTokenizer.nextWord(type);
        if(token != nullptr) {
            primaryExpr = makeNewLeaf(token, type);
        }
    }
    return primaryExpr;
}

void ExpressionParser::parsePostFixExpressionPrime(vector<Expr*>& postFixExprPrime, ExprType& type) {
    char o = p_exprTokenizer.nextChar(true);
    if(o == '(') {
        type = FUNCTIONCALL;
        p_exprTokenizer.nextChar(); //consume '('
        parseArgumentExpressionList(postFixExprPrime);
        p_exprTokenizer.nextChar(); //consume ')'
    }
    else if (o == '[') {
        type = DEREFERENCEOPERATOR;
        p_exprTokenizer.nextChar(); //consume '['
        Expr* expr = parseExpression();
        delete expr;
        p_exprTokenizer.nextChar(); //consume ']'

        Expr* operand = !postFixExprPrime.empty() ?  postFixExprPrime.back() : nullptr;
        DereferenceOperator* dereferenceOperator = new DereferenceOperator("Deref", operand);
        postFixExprPrime.push_back(dereferenceOperator);

    }
    else if (o == '.') {
        type = DOTOPERATOR;
        p_exprTokenizer.nextChar(); //consume '.'
        Expr* expr = parsePrimaryExpression();
        Identifier* identifier = dynamic_cast<Identifier*>(expr);
        if(!identifier) {
            cout << "Non identifier in dot operator " <<endl; return;
        }
        Expr* postFixExpr = nullptr;

        parsePostFixExpressionPrime(postFixExprPrime, type);
        if(!postFixExprPrime.empty()) {
            DotOperator* operand = static_cast<DotOperator*>(postFixExprPrime.back());
            operand->setLeftOp(static_cast<Identifier*>(expr));
            postFixExpr = new DotOperator(identifier, operand);
        } else {
            postFixExpr = new DotOperator(nullptr, identifier);
            cout <<"Dot operator created " <<*identifier <<endl;
        }
        postFixExprPrime.push_back(postFixExpr);
    }
    else if (o == '-') {
        type = POINTEROPERATOR;
        p_exprTokenizer.nextChar(); //consume '-'
        p_exprTokenizer.nextChar(); //consume '>'
        Expr* expr = parsePrimaryExpression();
        Identifier* identifier = dynamic_cast<Identifier*>(expr);
        if(!identifier) {
            cout << "Non identifier in dot operator " <<endl; return;
        }
        Expr* postFixExpr = nullptr;

        parsePostFixExpressionPrime(postFixExprPrime, type);
        if(!postFixExprPrime.empty()) {
            DotOperator* operand = static_cast<DotOperator*>(postFixExprPrime.back());
            operand->setLeftOp(new DereferenceOperator("Deref", expr));
            postFixExpr = new DotOperator(identifier, operand);
        } else {
            postFixExpr = new DotOperator(nullptr, identifier);
        }
        postFixExprPrime.push_back(postFixExpr);
    }
}

Expr* ExpressionParser::parsePostFixExpression() {
    Expr* primaryExpr = parsePrimaryExpression();
    Expr* postFixExpr = nullptr;

    if(!primaryExpr)
        return nullptr;
    ExprType type=ExprType::INVALID;
    vector<Expr*> postFixExprPrime;
    parsePostFixExpressionPrime(postFixExprPrime, type);
    switch(type) {
        case FUNCTIONCALL: {
            if(primaryExpr->getExprType() == IDENTIFIER) {
                Identifier* identifier = static_cast<Identifier*>(primaryExpr);
                if(strcmp(identifier->getName(),"malloc")==0) {
                    postFixExpr = new MallocFnCall(primaryExpr, postFixExprPrime); break;
                } else if(strcmp(identifier->getName(), "delete") == 0) {
                    postFixExpr = new DeleteFnCall(primaryExpr, postFixExprPrime); break;
                }
            }
            postFixExpr = new FunctionCall(primaryExpr, postFixExprPrime);
        break;
        }
        case DEREFERENCEOPERATOR: {
            Expr* arrayDereferenceOperator = postFixExprPrime.back();
            DereferenceOperator* pointerToData = static_cast<DereferenceOperator*>(postFixExprPrime.front());
            pointerToData->setRightOp(primaryExpr);
            postFixExpr = arrayDereferenceOperator;
            break;
        }
        case DOTOPERATOR: {
            DotOperator* dotOperator = static_cast<DotOperator*>(postFixExprPrime.front());
            Identifier* identifier = dynamic_cast<Identifier*>(primaryExpr);
            if(!identifier) {
                cout << "Non identifier in dot operator " <<endl; break;
            }
            dotOperator->setLeftOp(identifier);
            postFixExpr = dotOperator;
            break;
        }
    case POINTEROPERATOR: {
            DotOperator* dotOperator = static_cast<DotOperator*>(postFixExprPrime.front());
            Identifier* identifier = dynamic_cast<Identifier*>(primaryExpr);
            if(!identifier) {
                cout << "Non identifier in dot operator " <<endl; break;
            }
            dotOperator->setLeftOp(new DereferenceOperator("Deref", identifier));
            postFixExpr = dotOperator;
            break;
        }
        default:
            postFixExpr = primaryExpr;
    }
    return postFixExpr;
}

Expr* ExpressionParser::parseUnaryExpression() {
    char o = p_exprTokenizer.nextChar(true);
    Expr* unaryExpr = nullptr;
    Expr* operand = nullptr;
    switch(o) {
        case '*':
            p_exprTokenizer.nextChar();
            operand = parsePostFixExpression();
            if(operand)
                unaryExpr = new DereferenceOperator("Deref",operand);
            break;
        case '&':
            p_exprTokenizer.nextChar();
            operand = parsePostFixExpression();
            if(operand)
                unaryExpr = new AddressOfOperator("AddressOf", operand);
        break;
        default:
            unaryExpr = parsePostFixExpression();
    }
    return unaryExpr;
}

Expr* ExpressionParser::parseAssignmentExpression() {
    Expr* assignExpr = nullptr;
    const int pos = p_exprTokenizer.getPos();

    Expr* leftOp = parseUnaryExpression();
    char op = p_exprTokenizer.nextChar(true);
    if(op == '\0') {
        cout <<"unary expresson " <<leftOp <<endl;
        return leftOp;
    }

    if(op == '=') {
        p_exprTokenizer.nextChar();
        Expr* rightOp = parseAssignmentExpression();

        if(leftOp == nullptr || rightOp == nullptr) {
            delete leftOp;
            delete rightOp;
        }
        else {
            assignExpr = new AssignOperator(leftOp,op,rightOp);
        }
        cout <<"parseAssignmentExpression assignment " << assignExpr <<" " <<leftOp <<" " <<rightOp <<endl;

    } else {
        //backtrack:reset state
        //delete leftOp; - TODO: deleting the variable
        p_exprTokenizer.setPos(pos);

        assignExpr = parseAdditiveExpression();
        cout <<"parseAssignmentExpression expression " << assignExpr <<endl;
    }
    return assignExpr;
}

