/*
 * InstrParser.h
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */

#ifndef INSTRPARSER_H_
#define INSTRPARSER_H_
#include<vector>
#include "Expr.h"
#include "Stmt.h"
#include "Tokenizer.h"
#include "SymbolTable.h"
#include "ExpressionParser.h"

using namespace std;

class InstrParser {
public:
    InstrParser();
    virtual ~InstrParser();

    vector<DataType> parseTypeSpecifiers();
    vector<DataType> parseDeclarationSpecifiers();
    IdentifierName* parseParameterDecl();
    Status parseParameterList(vector<IdentifierName*>& identifierList);
    Status parsePointer(vector<PointerIdentifierName*>& identifierList);
    vector<IdentifierName*> parseDirectDeclaratorPrime(DeclType& declType);
    IdentifierName* parseDirectDeclarator();
    IdentifierName* parseDeclarator();
    Expr* parseInitializer();
    vector<Expr*> parseInitializerList();
    AssignStmt* parseInitDeclarator();
    Status parseInitDeclaratorList(vector<AssignStmt*>& initDeclaratorList);
    Status parseDeclaration(vector<AssignStmt*>& declaration);
    Status parseDeclarationList(vector<AssignStmt*>& declarationList);

    Status parseLabeledStmt(Block* block);
    Status parseSelectionStmt(Block* block);
    Status parseIterationStmt(Block* block);
    Status parseJumpStmt(Block* block);
    Status parseStmt(Block* block);
    Status parseStmtList(Block* block);
    //Status parseDecl(Block* block);
    Status parseAssign(Block* block);
    Status parseIf(IfStmt* stmt);
    Status parseElse(IfStmt* stmt);
    Status parseIfElse(Block* block);
    Status parseWhile(Block* block);
    Status parseBlock(Block* block);
    Status parseFunctionDecl(Block* block);
    //Status parseFunctionCall(Block* block);
    Status parseFile(const char* fileName);
    Block* getBlock() const;
    void clear();

private:
    Block* p_mainBlock;
    Tokenizer p_tokenizer;
    ExpressionParser p_exprParser;

    int p_pos;
};

#endif /* INSTRPARSER_H_ */
