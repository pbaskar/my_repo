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

    Variable* makeVariable(Block* block, const IdentifierName* identifierName, Type* dataType);
    Variable* makeFunctionVariable(Block* block, const IdentifierName* identifierName, Type* dataType);
    Variable* makeStructVariable(Block* block, const IdentifierName* identifierName, Type* dataType);
    Variable* makeVariableFromIdentifierName(Block* block, const IdentifierName* identifierName, Type* dataType);

    TypeQualifier parseTypeQualifier(Block* block);
    Status parseStructOrUnionDef(Block* block, Type*& type);
    Status parseTypeSpecifier(Block* block, Type*& type);
    StorageClassSpecifier parseStorageClassSpecifier(Block* block);
    vector<Type*> parseSpecifierQualifierList(Block* block);
    AssignStmt* parseStructDeclarator(Block* block);
    Status parseStructDeclaratorList(Block* block, vector<AssignStmt*>& structDeclaratorList);
    Status parseStructDeclaration(Block* block, vector<AssignStmt*>& structDeclaration);
    Status parseStructDeclarationList(Block* block, vector<AssignStmt*>& structDeclarationList);
    StructOrUnion parseStructOrUnion(Block* block);
    vector<Type*> parseDeclarationSpecifiers(Block* block);
    IdentifierName* parseParameterDecl(Block* block);
    Status parseParameterList(Block* block, vector<IdentifierName*>& identifierList);
    Status parsePointer(Block* block, vector<PointerIdentifierName*>& identifierList);
    Status parseDirectDeclaratorPrime(Block* block, vector<IdentifierName*>& directDeclaratorPrime, DeclType& declType);
    IdentifierName* parseDirectDeclarator(Block* block, DeclType& declType);
    IdentifierName* parseDeclarator(Block* block, DeclType& declType);
    Expr* parseInitializer(Block* block);
    vector<Expr*> parseInitializerList(Block* block);
    AssignStmt* parseInitDeclarator(Block* block);
    Status parseInitDeclaratorList(Block* block, vector<AssignStmt*>& initDeclaratorList);
    Status parseDeclaration(Block* block, vector<AssignStmt*>& declaration);
    Status parseDeclarationList(Block* block, vector<AssignStmt*>& declarationList);

    Status parseStructOrUnionDef(Block* block);
    Status parseStructOrUnionSpecifier(Block* block);
    Status parseLabeledStmt(Block* block);
    Status parseSelectionStmt(Block* block);
    Status parseIterationStmt(Block* block);
    Status parseJumpStmt(Block* block);
    Status parseStmt(Block* block);
    Status parseStmtList(Block* block);
    Status parseAssign(Block* block);
    Status parseIf(IfStmt* stmt);
    Status parseElse(IfStmt* stmt);
    Status parseIfElse(Block* block);
    Status parseWhile(Block* block);
    Status parseBlock(Block* block);
    Status parseFunctionDecl(Block* block);
    Status parseFile(const char* fileName);
    Block* getBlock() const;
    void clear();

private:
    Block* p_mainBlock;
    Tokenizer p_tokenizer;
    ExpressionParser p_exprParser;

    vector<const StructType*> p_types;

    int p_pos;
};

#endif /* INSTRPARSER_H_ */
