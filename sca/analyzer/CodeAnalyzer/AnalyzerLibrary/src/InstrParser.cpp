/*
 * InstrParser.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include<cstring>
#include<cstdarg>
#include "InstrParser.h"
#include "Logger.h"

InstrParser::InstrParser(): p_mainBlock(0), p_pos(0) {

}

InstrParser::~InstrParser() {
}

Status InstrParser::parseFile(const char* fileName) {
    Status status = p_tokenizer.openFile(fileName);
    if(status == FAILURE) { Logger::logMessage(ErrorCode::FILE_NOT_OPEN,  2, "InstrParser::parseFile:", fileName); p_tokenizer.closeFile(); return status; }
    status = p_tokenizer.nextLine();
    if(status == FAILURE) { Logger::logMessage(ErrorCode::FILE_EMPTY,  2, "InstrParser::parseFile:", fileName); p_tokenizer.closeFile(); return status; }
    p_mainBlock = new Block;
    char nextChar = p_tokenizer.nextChar(true);
    while(nextChar != '\0') {
        status = parseFunctionDecl(p_mainBlock);
        if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
        nextChar = p_tokenizer.nextChar(true);
    }
    //Todo: release identifier memory
    FunctionCallStmt* stmt = new FunctionCallStmt(FUNC_CALL, new Identifier("main"), vector<Expr*>());
    p_mainBlock->addStatement(stmt);

    p_tokenizer.closeFile();
    return status;
}

Variable* makeVariable(Block* block, const IdentifierName* identifierName) {
    Variable*  variable = nullptr;
    const PointerIdentifierName* pointerIdentifierName = dynamic_cast<const PointerIdentifierName*>(identifierName);
    if(pointerIdentifierName != nullptr) {
        Variable* pointsTo = makeVariable(block, pointerIdentifierName->getPointsTo());
        variable = new PointerVariable(pointsTo->getName(), VarType::POINTER, pointsTo);
    } else {
        variable = new Variable(identifierName->getName(), VarType::VALUE);
    }
    return variable;
}

Status InstrParser::parseBlock(Block* block) {
    Status status = SUCCESS;
    char c = p_tokenizer.lookAhead(1);
    if( c != '{') return FIRST_MISMATCH;
    p_tokenizer.nextChar(); //consume '{'

    p_exprParser.setSymbolTable(block->getSymbolTable());
    p_tokenizer.nextLine();
    vector<AssignStmt*> declList;
    status = parseDeclarationList(declList);

    for(AssignStmt* assignStmt : declList) {
        Variable* var = makeVariable(block, assignStmt->getVar());
        block->getSymbolTable()->addSymbol(var);
        block->addStatement(assignStmt);
    }

    vector<AssignStmt*> stmtList;
    status = parseStmtList(block);

    char closeParen =  p_tokenizer.nextChar(true);
    p_tokenizer.nextChar(); //consume '}'
    cout <<"consuming } "<<closeParen <<endl;
    /*char closeBrace = p_tokenizer.nextChar(true);
    while(closeBrace != '}' && closeBrace != '\0') {
        Status status = parseStmt(block);
        if(status == FAILURE) {
            return FAILURE;
        }
        closeBrace = p_tokenizer.nextChar(true);
    }*/
    cout <<"parseBlock " <<status <<endl;
    return status;
}

Status InstrParser::parseLabeledStmt(Block* block) {
    Status status = FIRST_MISMATCH;
    return status;
}

Status InstrParser::parseSelectionStmt(Block* block) {
    Status status = SUCCESS;
    char* next = p_tokenizer.nextWord(true);
    if(next == nullptr) return FAILURE;
    if(strcmp(next, "if")==0) {
        p_tokenizer.consumeWord();
        parseIfElse(block);
    } else {
        status = FIRST_MISMATCH;
    }
    return status;
}

Status InstrParser::parseIterationStmt(Block* block) {
    Status status = SUCCESS;
    char* next = p_tokenizer.nextWord(true);
    if(next == nullptr) return FAILURE;
    if(strcmp(next, "while")==0) {
        p_tokenizer.consumeWord();
        parseWhile(block);
    } else {
        status = FIRST_MISMATCH;
    }
    return status;
}

Status InstrParser::parseJumpStmt(Block* block) {
    Status status = FIRST_MISMATCH;
    return status;
}

Status InstrParser::parseStmt(Block* block) {
    Status status = SUCCESS;
    int pos = p_tokenizer.getPos();

    status = parseBlock(block);
    if(status != FIRST_MISMATCH) return status;

    p_tokenizer.setPos(pos);
    status = parseLabeledStmt(block);
    if(status != FIRST_MISMATCH) return status;

    p_tokenizer.setPos(pos);
    status = parseSelectionStmt(block);
    if(status != FIRST_MISMATCH) return status;

    p_tokenizer.setPos(pos);
    status = parseIterationStmt(block);
    if(status != FIRST_MISMATCH) return status;

    p_tokenizer.setPos(pos);
    status = parseJumpStmt(block);
    if(status != FIRST_MISMATCH) return status;

    p_tokenizer.setPos(pos);
    status = parseAssign(block);

    /*char* next = p_tokenizer.nextWord(true);

    if(strcmp(next, "int")==0) {
        p_tokenizer.consumeWord();
        status = parseDecl(block);
    }
    else if(strcmp(next, "if")==0) {
        p_tokenizer.consumeWord();
        status = parseIfElse(block);
    }
    else if(strcmp(next, "while")==0) {
        p_tokenizer.consumeWord();
        status = parseWhile(block);
    }
    else {
        status = parseAssign(block);
        char c = p_tokenizer.lookAhead(2);
        if(c == '=')
            status = parseAssign(block);
        else if(c == '(')
            status = parseFunctionCall(block);
        else {
            Logger::logMessage(ErrorCode::STMT_INVALID,  1, "InstrParser::parseStmt:");
            status = FAILURE;
        }
    }
    delete next;*/
    cout <<"parsestmt " <<status <<endl;
    return status;
}

Status InstrParser::parseStmtList(Block* block) {
    Status status = SUCCESS;
    status = parseStmt(block);
    char c = p_tokenizer.lookAhead(1);
    cout <<"stmt list look for closing brace " << c <<endl;
    if(status == SUCCESS && c != '}') {
        status = parseStmtList(block);
    }
    return status;
}

/*Status InstrParser::parseDecl(Block* block) {
    Status status = SUCCESS;
    AssignStmt* stmt = new AssignStmt(DECL);
    block->addStatement(stmt);

    VarType varType = VarType::VALUE;
    char pointer = p_tokenizer.lookAhead(1);
    if(pointer == '*') { p_tokenizer.nextChar(); varType = VarType::POINTER; }
    char* name = p_tokenizer.nextWord();
    if(name == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseDecl:", "name"); return FAILURE; }
    stmt->setVar(block->addSymbol(name, varType));

    char equal = p_tokenizer.nextChar();
    if(equal == ';') { 	p_tokenizer.nextLine(); return status; }
    if(equal != '=') {  Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseDecl:", "="); return FAILURE; }

    char* next = p_tokenizer.nextWord();
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseDecl:", "value"); return FAILURE; }
    Expr* value = p_exprParser.parseExpressionStr(next);
    delete next;
    if(value == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseDecl:", "value"); return FAILURE; }
    stmt->setValue(value);
    p_tokenizer.nextLine();
    cout <<"Declarative stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
    return status;
}*/

Status InstrParser::parseAssign(Block* block) {
    Status status = SUCCESS;
    AssignStmt* stmt = new AssignStmt(ASSIGN);
    block->addStatement(stmt);

    /*char delim[] = {'='};
    char* name = p_tokenizer.nextWordUntil(delim, sizeof(delim));
    if(name == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "name"); return FAILURE; }
    cout <<" name " << name <<endl;
    Expr* var = p_exprParser.parseExpressionStr(name);
    delete name;
    if(var == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseAssign:", "value"); return FAILURE; }
    stmt->setVar(var);

    char equal = p_tokenizer.nextChar();
    if ( equal != '=' ) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "="); return FAILURE; }
*/
    char endDelim[] = {';',','};
    char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "value"); return FAILURE; }
    cout <<" next word " << next <<endl;
    Expr* value = p_exprParser.parseExpressionStr(next);
    delete next;
    if(value == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseAssign:", "value"); return FAILURE; }
    stmt->setValue(value);
    char comma = p_tokenizer.lookAhead(1);
    cout << "comma " <<comma <<endl;
    if(comma == ',') {
        p_tokenizer.nextChar();
        parseAssign(block);
    }
    else {
        p_tokenizer.nextLine();
    }
    cout <<"Assignment stmt: size = " << block->getSubStatements().size()  <<" " <<*stmt <<endl;
    return status;
}

Status InstrParser::parseIfElse(Block* block) {
    Status status = SUCCESS;
    IfStmt* ifStmt = new IfStmt(IF);
    ifStmt->setBlock(new Block(block));
    block->addStatement(ifStmt);

    status = parseIf(ifStmt);
    if(status == FAILURE) return FAILURE;

    p_tokenizer.nextLine();
    char* next = p_tokenizer.nextWord(true);
    if(next == nullptr || strcmp(next, "else") != 0) { cout <<"no else " <<next <<endl; delete next; return status; }
    delete next;
    p_tokenizer.consumeWord();

    IfStmt* elseStmt = new IfStmt(ELSE);
    elseStmt->setBlock(new Block(block));
    status = parseElse(elseStmt);
    if(status == FAILURE) { delete elseStmt; return FAILURE; }

    ifStmt->setElse(elseStmt);
    p_tokenizer.nextLine();
    return status;
}

Status InstrParser::parseIf(IfStmt* stmt) {
    Status status = SUCCESS;

    char openBrace = p_tokenizer.nextChar();
    if ( openBrace != '(')	{ Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseIf:", "open brace"); return FAILURE; }
    char* next = p_tokenizer.nextWord();
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseIf:", "condition"); return FAILURE; }
    Expr* condition = p_exprParser.parseExpressionStr(next);
    delete next;
    if(condition == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseIf:", "condition"); return FAILURE; }
    stmt->setCondition(condition);
    p_tokenizer.nextChar(); // consume )

    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    cout <<"If stmt: " <<stmt->getBlock()->getSubStatements().size() << " "<<*stmt <<endl;
    return status;
}

Status InstrParser::parseElse(IfStmt* stmt) {
    Status status = SUCCESS;

    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    cout <<"Else stmt: " <<*stmt <<endl;
    return status;
}

Status InstrParser::parseWhile(Block* block) {
    Status status = SUCCESS;
    WhileStmt* stmt = new WhileStmt(WHILE);
    stmt->setBlock(new Block(block));
    block->addStatement(stmt);

    char openBrace = p_tokenizer.nextChar();
    if ( openBrace != '(') { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseWhile:", "open brace"); return FAILURE; }

    char* next = p_tokenizer.nextWord();
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseWhile:", "condition"); return FAILURE; }
    Expr* condition = p_exprParser.parseExpressionStr(next);
    delete next;
    if(condition == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseWhile:", "condition"); return FAILURE; }
    stmt->setCondition(condition);

    p_tokenizer.nextChar();  //consume )
    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    p_tokenizer.nextLine();
    cout <<"While stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
    return status;
}

vector<DataType> InstrParser::parseTypeSpecifiers() {
    vector<DataType> types;
    const char* next = p_tokenizer.nextWord();
    if(next == nullptr) return types;
    if(strcmp(next,"int")==0)
        types.push_back(INT);
    delete(next);
    return types;
}

vector<DataType> InstrParser::parseDeclarationSpecifiers() {
    vector<DataType> typeSpecifiers = parseTypeSpecifiers();
    return typeSpecifiers;
}

IdentifierName* InstrParser::parseParameterDecl() {
    IdentifierName* declarator = nullptr;
    vector<DataType> declarationSpecifiers = parseDeclarationSpecifiers();
    if(declarationSpecifiers.empty()) {
        return declarator;
    }
    declarator = parseDeclarator();
    return declarator;
}

Status InstrParser::parseParameterList(vector<IdentifierName*>& identifierList) {
    Status status = SUCCESS;
    IdentifierName* parameterDecl = parseParameterDecl();
    if(parameterDecl == nullptr) return status;
    identifierList.push_back(parameterDecl);
    char next = p_tokenizer.lookAhead(1);
    if(next == ',') {
        p_tokenizer.nextChar(); //consume ,
        status = parseParameterList(identifierList);
    }
    return status;
}

Status InstrParser::parsePointer(vector<PointerIdentifierName*>& identifierList) {
    Status status = SUCCESS;
    char c = p_tokenizer.lookAhead(1);
    if(c=='*') {
        p_tokenizer.nextChar(); //consume *
        status = parsePointer(identifierList);
        PointerIdentifierName* pointerIdentifier = new PointerIdentifierName("Pointer", nullptr);
        if(!identifierList.empty()) {
            pointerIdentifier->setPointsTo(identifierList.back());
        }
        identifierList.push_back(pointerIdentifier);
    }
    return status;
}

vector<IdentifierName*> InstrParser::parseDirectDeclaratorPrime(DeclType& declType) {
    vector<IdentifierName*> directDeclaratorPrime;
    char next = p_tokenizer.lookAhead(1);
    switch(next) {
        case '(':
            p_tokenizer.nextChar(); // consume (
            parseParameterList(directDeclaratorPrime);
            p_tokenizer.nextChar(); // consume )
            declType = FUNCTIONDECL;
        break;
    }
    return directDeclaratorPrime;
}

IdentifierName* InstrParser::parseDirectDeclarator() {
    IdentifierName* directDeclarator = nullptr;
    char* next = p_tokenizer.nextWord();
    if(next == nullptr) return directDeclarator;
    DeclType declType = VARDECL;
    vector<IdentifierName*> directDeclaratorPrime = parseDirectDeclaratorPrime(declType);
    switch(declType) {
        case VARDECL:
            directDeclarator = new IdentifierName(next);
        break;
        case FUNCTIONDECL:
            directDeclarator = new FunctionIdentifierName(next, directDeclaratorPrime);
        break;
    }
    return directDeclarator;
}

IdentifierName* InstrParser::parseDeclarator() {
    IdentifierName* declarator = nullptr;
    char c = p_tokenizer.lookAhead(1);
    if(c == '*') {
        vector<PointerIdentifierName*> pointerNames;
        parsePointer(pointerNames);
        IdentifierName* directDeclarator = parseDirectDeclarator();
        pointerNames.front()->setPointsTo(directDeclarator);
        declarator = pointerNames.back();
        declarator->setName(directDeclarator->getName());
     } else {
        declarator = parseDirectDeclarator();
    }
    cout <<"parseDeclarator " <<declarator <<endl;
    return declarator;
}

Expr* InstrParser::parseInitializer() {
    char endDelim[] = {';',','};
    char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "value"); return nullptr; }
    Expr* value = p_exprParser.parseExpressionStr(next);
    delete next;
    if(value == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseAssign:", "value"); return nullptr; }
    return value;
}

vector<Expr*> InstrParser::parseInitializerList() {
    vector<Expr*> initializerList;
    Expr* initializer = parseInitializer();
    initializerList.push_back(initializer);
    char c = p_tokenizer.lookAhead(1);
    if(c == ',') {
        p_tokenizer.nextChar();
        parseInitializerList();
    }
    return initializerList;
}

AssignStmt* InstrParser::parseInitDeclarator() {
    AssignStmt* assignStmt = nullptr;
    Expr* value = nullptr;
    IdentifierName* var = parseDeclarator();
    if(var == nullptr) return nullptr;
    char c = p_tokenizer.lookAhead(1);
    if(c == '=') {
        p_tokenizer.nextChar();
        value = parseInitializer();
    }
    assignStmt = new AssignStmt(DECL, var, value);
    return assignStmt;
}

Status InstrParser::parseInitDeclaratorList(vector<AssignStmt*>& initDeclaratorList) {
    Status status = SUCCESS;
    AssignStmt* assignStmt = parseInitDeclarator();
    if(assignStmt == nullptr) return FAILURE;
    initDeclaratorList.push_back(assignStmt);
    char c = p_tokenizer.lookAhead(1);
    if(c == ',') {
        p_tokenizer.nextChar();
        status = parseInitDeclaratorList(initDeclaratorList);
    }
    return status;
}

Status InstrParser::parseDeclaration(vector<AssignStmt*>& declaration) {
    Status status = SUCCESS;
    vector<DataType> declarationSpecifiers = parseDeclarationSpecifiers();
    if(declarationSpecifiers.empty())
        return FIRST_MISMATCH;
    status = parseInitDeclaratorList(declaration);
    p_tokenizer.nextChar(); //consume ';'
    p_tokenizer.nextLine();
    cout <<"parseDeclaration " <<status <<endl;
    return status;
}

Status InstrParser::parseDeclarationList(vector<AssignStmt*>& declarationList) {
    Status status = SUCCESS;
    int pos = p_tokenizer.getPos();
    status = parseDeclaration(declarationList);
    if(status == SUCCESS) {
        status = parseDeclarationList(declarationList);
    } else if (status == FIRST_MISMATCH) {
        p_tokenizer.setPos(pos);
        status = SUCCESS;
    }
    return status;
}

Status InstrParser::parseFunctionDecl(Block* block) {
    Status status = SUCCESS;
    FunctionDeclStmt* stmt = new FunctionDeclStmt(FUNC_DECL);
    Block* declBlock = new Block(block);
    stmt->setBlock(declBlock);
    block->addStatement(stmt);

    parseDeclarationSpecifiers();
    IdentifierName* identifier = parseDeclarator();
    FunctionIdentifierName* functionIdentifierName = dynamic_cast<FunctionIdentifierName*>(identifier);
    if(functionIdentifierName == nullptr) return FAILURE;
    vector<IdentifierName*> parameterList = functionIdentifierName->getParameterList();
    for(IdentifierName* identifierName : parameterList) {
        Variable* var = makeVariable(block, identifierName);
        block->getSymbolTable()->addSymbol(var);
    }
    stmt->setName(identifier);

    /*char* next = p_tokenizer.nextWord(); //return type
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionDecl:", "return type"); return FAILURE; }

    next = p_tokenizer.nextWord();	//function name - add to symbol table
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionDecl:", "function name"); return FAILURE; }
    stmt->setName(next);
    cout<<"function name " <<next <<endl;

    char openBrace = p_tokenizer.nextChar();
    if ( openBrace != '(') { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionDecl:", "open brace"); return FAILURE; }

    next = p_tokenizer.nextWord();
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionDecl:", "argument declaration"); return FAILURE; }
    delete next;

    VarType varType = VarType::VALUE;
    char pointer = p_tokenizer.lookAhead(1);
    if(pointer == '*') { p_tokenizer.nextChar(); varType = VarType::POINTER; }
    next = p_tokenizer.nextWord();
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionDecl:", "argument name"); return FAILURE; }

    Variable* argument = block->addSymbol(next, varType); // add to the current symbol table
    stmt->addFormalArgument(argument);*/

    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }
    p_tokenizer.nextLine();
    block->getSymbolTable()->addFnSymbol(identifier->getName());
    cout <<"Function Decl stmt: size = " <<stmt->getBlock()->getSubStatements().size() << " parent size "
            <<block->getSubStatements().size() << " " <<*stmt <<" status " <<status <<endl;
    return status;
}

/*Status InstrParser::parseFunctionCall(Block* block) {
    Status status = SUCCESS;
    FunctionCallStmt* stmt = new FunctionCallStmt(FUNC_CALL);
    block->addStatement(stmt);

    char* next = p_tokenizer.nextWord(); //function name
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionCall:", "function name"); return FAILURE; }
    stmt->setName(next);

    char openBrace = p_tokenizer.nextChar();
    if ( openBrace != '(') { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFunctionCall:", "open brace"); return FAILURE; }

    next = p_tokenizer.nextWord();
    if(next == nullptr) { return status; }
    Expr* argument = p_exprParser.parseExpressionStr(next);
    delete next;
    if(argument == nullptr) { cout<<"InstrParser::parseFunctionCall: could not parse argument "<<endl; return FAILURE; }
    stmt->addActualArgument(argument);

    p_tokenizer.nextLine();
    cout <<"Function Call stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
    return status;
}*/

Block* InstrParser::getBlock() const {
    return p_mainBlock;
}

void InstrParser::clear() {
    delete p_mainBlock;
}
