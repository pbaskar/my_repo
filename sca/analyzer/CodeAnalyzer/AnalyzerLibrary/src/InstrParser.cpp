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
    FunctionCallStmt* stmt = new FunctionCallStmt(FUNC_CALL);
    stmt->setName("main");
    p_mainBlock->addStatement(stmt);

    p_tokenizer.closeFile();
    return status;
}

Status InstrParser::parseBlock(Block* block) {
    Status status = SUCCESS;
    p_exprParser.setSymbolTable(block->getSymbolTable());
    char closeBrace = p_tokenizer.nextChar(true);
    while(closeBrace != '}' && closeBrace != '\0') {
        Status status = parseStmt(block);
        if(status == FAILURE) {
            return FAILURE;
        }
        closeBrace = p_tokenizer.nextChar(true);
    }
    return status;
}

Status InstrParser::parseStmt(Block* block) {
    char* next = p_tokenizer.nextWord(true);
    Status status = SUCCESS;
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
        /*char c = p_tokenizer.lookAhead(2);
        if(c == '=')
            status = parseAssign(block);
        else if(c == '(')
            status = parseFunctionCall(block);
        else {
            Logger::logMessage(ErrorCode::STMT_INVALID,  1, "InstrParser::parseStmt:");
            status = FAILURE;
        }*/
    }
    delete next;
    return status;
}

Status InstrParser::parseDecl(Block* block) {
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
}

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
    cout <<" next word " << next <<endl;
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "value"); return FAILURE; }
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
    if(next == nullptr || strcmp(next, "else") != 0) { cout <<"no else " <<endl; delete next; return status; }
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
    p_tokenizer.nextLine();
    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    cout <<"If stmt: " <<stmt->getBlock()->getSubStatements().size() << " "<<*stmt <<endl;
    return status;
}

Status InstrParser::parseElse(IfStmt* stmt) {
    Status status = SUCCESS;

    p_tokenizer.nextLine();
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

    p_tokenizer.nextLine();
    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    p_tokenizer.nextLine();
    cout <<"While stmt: size = " <<block->getSubStatements().size() << " " <<*stmt <<endl;
    return status;
}

Status InstrParser::parseFunctionDecl(Block* block) {
    Status status = SUCCESS;
    FunctionDeclStmt* stmt = new FunctionDeclStmt(FUNC_DECL);
    stmt->setBlock(new Block(block));
    block->addStatement(stmt);

    char* next = p_tokenizer.nextWord(); //return type
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
    stmt->addFormalArgument(argument);

    p_tokenizer.nextLine();
    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }

    p_tokenizer.nextLine();
    cout <<"Function Decl stmt: size = " <<stmt->getBlock()->getSubStatements().size() << " parent size "
            <<block->getSubStatements().size() << " " <<*stmt <<" status " <<status <<endl;
    return status;
}

Status InstrParser::parseFunctionCall(Block* block) {
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
}

Block* InstrParser::getBlock() const {
    return p_mainBlock;
}

void InstrParser::clear() {
    delete p_mainBlock;
}
