/*
 * InstrParser.cpp
 *
 *  Created on: Sep 18, 2023
 *      Author: prbas_000
 */
#include<iostream>
#include<cstring>
#include<cassert>
#include<cstdarg>
#include "InstrParser.h"
#include "Logger.h"

InstrParser::InstrParser(): p_mainBlock(0), p_pos(0) {

}

InstrParser::~InstrParser() {
    for(int i=0; i<p_types.size(); i++) {
        delete p_types[i];
    }
}

Status InstrParser::parseFile(const char* fileName) {
    Status status = p_tokenizer.openFile(fileName);
    if(status == FAILURE) { Logger::logMessage(ErrorCode::FILE_NOT_OPEN,  2, "InstrParser::parseFile:", fileName); p_tokenizer.closeFile(); return status; }
    status = p_tokenizer.nextLine();
    if(status == FAILURE) { Logger::logMessage(ErrorCode::FILE_EMPTY,  2, "InstrParser::parseFile:", fileName); p_tokenizer.closeFile(); return status; }
    p_mainBlock = new Block;
    char nextChar = p_tokenizer.nextChar(true);
    while(nextChar != '\0') {
        int pos = p_tokenizer.getPos();
        status = parseStructOrUnionSpecifier(p_mainBlock);
        if(status == Status::FIRST_MISMATCH) {
            p_tokenizer.setPos(pos);
            status = parseFunctionDecl(p_mainBlock);
        }
        if(status == FAILURE) { p_tokenizer.closeFile(); return status; }
        nextChar = p_tokenizer.nextChar(true);
    }
    //Todo: release identifier memory
    FunctionCallStmt* stmt = new FunctionCallStmt(FUNC_CALL, new Identifier("main"), vector<Expr*>());
    p_mainBlock->addStatement(stmt);

    p_tokenizer.closeFile();
    return status;
}

//make pointervariable and pointedTo variable
Variable* InstrParser::makeVariable(Block* block, const IdentifierName* identifierName, Type* dataType) {
    Variable* variable = nullptr;
    const PointerIdentifierName* pointerIdentifierName = dynamic_cast<const PointerIdentifierName*>(identifierName);
    if(pointerIdentifierName != nullptr) {
        Variable* pointsTo = makeVariable(block, pointerIdentifierName->getPointsTo(), dataType);
        variable = new PointerVariable(pointsTo->getName(), VarType::VALUE, pointsTo);
        pointsTo->setAddress(variable);
        cout <<"Pointer Variable created " <<pointsTo->getName();
    } else {
        assert(identifierName);
        variable = new Variable(identifierName->getName(), VarType::VALUE);
        cout << "Variable created " <<identifierName->getName();
    }
    return variable;
}

Variable* InstrParser::makeFunctionVariable(Block* block, const IdentifierName* identifierName, Type* dataType) {
    Variable* variable = nullptr;
    FunctionVariable* functionVariable = new FunctionVariable(identifierName->getName(), VarType::FUNCTION);
    PointerVariable* pointerVariable = new PointerVariable(identifierName->getName(), VarType::FUNCTION, functionVariable);
    functionVariable->setAddress(pointerVariable);
    return pointerVariable;
}

Variable* InstrParser::makeStructVariable(Block* block, const IdentifierName* identifierName, Type* dataType) {
    Variable* variable = nullptr;
    const StructType* structType = nullptr;
    NamedType* namedType = static_cast<NamedType*>(dataType);
    for(const StructType* typeIt : p_types) {
        if(strcmp(typeIt->getName(), namedType->getName())==0) {
            structType = typeIt;
            break;
        }
    }
    if(!structType) {
        cout <<"Struct type not found " <<namedType->getName() <<" " <<p_types.size() <<endl;
        return nullptr;
    }
    const vector<pair<const IdentifierName*, Type*>>& memIdentifierNames = structType->getMemIdentifierNames();
    StructVariable* structVariable = new StructVariable(identifierName->getName(), VarType::STRUCTTYPE);
    for(const pair<const IdentifierName*, Type*>& memIdentifierName : memIdentifierNames) {
        Variable* memVar = makeVariableFromIdentifierName(block, memIdentifierName.first, memIdentifierName.second);
        structVariable->addMemVars(memVar);
        cout <<"mem " <<memIdentifierName.first <<" " <<memIdentifierName.second <<endl;
    }
    //struct and pointer to struct
    const PointerIdentifierName* pointerIdentifierName = dynamic_cast<const PointerIdentifierName*>(identifierName);
    if(pointerIdentifierName != nullptr) {
        PointerVariable* pointerVariable = new PointerVariable(identifierName->getName(), VarType::STRUCTTYPE, structVariable);
        cout <<"struct Pointer variable added " <<*pointerVariable <<" " <<memIdentifierNames.size() <<endl;
        variable = pointerVariable;
    }
    else {
        cout <<"struct variable added " <<*structVariable <<" " <<memIdentifierNames.size() <<endl;
        variable = structVariable;
    }
    return variable;
}

Variable* InstrParser::makeVariableFromIdentifierName(Block* block, const IdentifierName* identifierName, Type* dataType) {
    DeclType declType = identifierName->getType();
    Variable* var = nullptr;
    switch(declType) {
        case VARDECL: {
            // struct and pointer to struct variables
            if(dataType->getDataType() == DataType::STRUCTSPECIFIER) {
                var = makeStructVariable(block, identifierName, dataType);
                if(!var) {
                    cout <<"Struct Declaration not found " <<identifierName->getName()<<endl;
                    return nullptr;
                }
            }
            else {
                //variables and pointer variables
                var = makeVariable(block, identifierName, dataType);
            }
        break;
        }
        case FUNCTIONDECL: {
            //pointer to function
            var = makeFunctionVariable(block, identifierName, dataType);
        break;
        }
        default:;
    }
    return var;
}

Status InstrParser::parseBlock(Block* block) {
    Status status = SUCCESS;
    char c = p_tokenizer.lookAhead(1);
    if( c != '{') return FIRST_MISMATCH;
    p_tokenizer.nextChar(); //consume '{'

    p_exprParser.setSymbolTable(block->getSymbolTable());
    p_tokenizer.nextLine();
    vector<AssignStmt*> declList;
    status = parseDeclarationList(block, declList);

    //make variables from Identifer names for declaration list
    for(AssignStmt* assignStmt : declList) {
        const IdentifierName* identifierName = assignStmt->getVar();
        assert(identifierName != nullptr);
        Type* type = assignStmt->getDataType();
        Variable* var = makeVariableFromIdentifierName(block, identifierName, type);
        block->getSymbolTable()->addSymbol(var);
        if(!var) return Status::FAILURE;
        block->addStatement(assignStmt);
    }

    vector<AssignStmt*> stmtList;
    status = parseStmtList(block);

    char closeParen =  p_tokenizer.nextChar(true);
    p_tokenizer.nextChar(); //consume '}'
    cout <<"consuming } "<<closeParen <<endl;
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
    }
    else if(strcmp(next, "for")==0) {
        p_tokenizer.consumeWord();
        parseFor(block);
    }else {
        status = FIRST_MISMATCH;
    }
    return status;
}

Status InstrParser::parseJumpStmt(Block* block) {
    Status status = FIRST_MISMATCH;
    return status;
}

Expr* InstrParser::parseExpressionStmt(Block* block) {
    char endDelim[] = {';'};
    char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
    if(next == nullptr) {
        Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseExpressionStmt:", "value");
        return nullptr;
    }
    cout <<" next word " << next <<endl;
    Expr* value = p_exprParser.parseExpressionStr(next);
    delete next;
    if(value == nullptr) {
        Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseExpressionStmt:", "value");
        return nullptr;
    }
    p_tokenizer.nextChar(); // consume ';'
    return value;
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

Status InstrParser::parseAssign(Block* block) {
    Status status = SUCCESS;
    AssignStmt* stmt = new AssignStmt(ASSIGN);
    block->addStatement(stmt);

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

Status InstrParser::parseFor(Block* block) {
    Status status = SUCCESS;
    ForStmt* forStmt = new ForStmt(FOR);
    Block* forBlock = new Block(block);
    forStmt->setBlock(forBlock);
    block->addStatement(forStmt);

    char openBrace = p_tokenizer.nextChar();
    if ( openBrace != '(') { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseFor:", "open brace"); return FAILURE; }

    vector<AssignStmt*> initStmts;
    parseDeclaration(forBlock, initStmts);
    if(!initStmts.empty()) {
        AssignStmt* initAssignStmt = initStmts[0];

        const IdentifierName* identifierName = initAssignStmt->getVar();
        assert(identifierName != nullptr);
        Type* type = initAssignStmt->getDataType();
        Variable* var = makeVariableFromIdentifierName(forBlock, identifierName, type);
        forBlock->getSymbolTable()->addSymbol(var);
        if(!var) return Status::FAILURE;

        forStmt->setInitStmt(initAssignStmt);
    }

    Expr* condition = parseExpressionStmt(forBlock);
    if(!condition)
        forStmt->setCondition(condition);

    char endDelim[] = {')'};
    char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
    if(next != nullptr) {
        Expr* postExpr = p_exprParser.parseExpressionStr(next);
        delete next;
        if(postExpr == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseFor:", "postExpr"); return FAILURE; }
        forStmt->setPostExpr(postExpr);
    }

    p_tokenizer.nextChar();  //consume )
    status = parseBlock(forBlock);
    if(status == FAILURE) { return FAILURE; }

    p_tokenizer.nextLine();
    cout <<"For stmt: size = " <<block->getSubStatements().size() << " " <<*forStmt <<endl;
    return status;
}

TypeQualifier InstrParser::parseTypeQualifier(Block* block) {
    TypeQualifier typeQualifier = TypeQualifier::CONST;
    const char* next = p_tokenizer.nextWord(true);
    if(next == nullptr) return typeQualifier;
    if(strcmp(next,"const")==0) {
        p_tokenizer.consumeWord();
        typeQualifier = TypeQualifier::CONST;
    }
    else if(strcmp(next,"volatile")==0) {
        p_tokenizer.consumeWord();
        typeQualifier = TypeQualifier::VOLATILE;
    }
    delete(next);
    return typeQualifier;
}

Status InstrParser::parseStructOrUnionDef(Block* block, Type*& type) {
    Status status = SUCCESS;
    StructOrUnion structOrUnion = parseStructOrUnion(block);
    if(structOrUnion == StructOrUnion::NOT_STRUCTORUNION) {
        return Status::FIRST_MISMATCH;
    }
    char* nextWord = p_tokenizer.nextWord();
    if(nextWord == nullptr) return FAILURE;
    type = new NamedType(nextWord, DataType::STRUCTSPECIFIER);
    return status;
}

Status InstrParser::parseTypeSpecifier(Block* block, Type*& type) {
    Status status = SUCCESS;

    int pos = p_tokenizer.getPos();
    status = parseStructOrUnionDef(block, type);
    if(status == Status::FIRST_MISMATCH) {
        p_tokenizer.setPos(pos);
        char* next = p_tokenizer.nextWord(true);
        cout <<"next value " <<next <<endl;
        for(int i=0; i<dataTypesSize; i++) {
            if(next == nullptr) return FAILURE;
            if(strcmp(next,dataTypes[i])==0) {
                type = new Type(DataType(i));
                cout <<"type created " << i <<endl;
                p_tokenizer.consumeWord();
                return SUCCESS;
            }
        }
        delete(next);
    }
    return status;
}

StorageClassSpecifier InstrParser::parseStorageClassSpecifier(Block* block) {
    StorageClassSpecifier storageClassSpecifier = StorageClassSpecifier::AUTO;
    const char* next = p_tokenizer.nextWord(true);
    if(next == nullptr) return storageClassSpecifier;

    if(strcmp(next,"typedef")==0) {
        p_tokenizer.consumeWord();
        storageClassSpecifier = StorageClassSpecifier::TYPEDEF;
    }
    else if(strcmp(next,"extern")==0) {
        p_tokenizer.consumeWord();
        storageClassSpecifier = StorageClassSpecifier::EXTERN;
    }
    if(strcmp(next,"static")==0) {
        p_tokenizer.consumeWord();
        storageClassSpecifier = StorageClassSpecifier::STATIC;
    }
    else if(strcmp(next,"auto")==0) {
        p_tokenizer.consumeWord();
        storageClassSpecifier = StorageClassSpecifier::AUTO;
    }
    else if(strcmp(next,"register")==0) {
        p_tokenizer.consumeWord();
        storageClassSpecifier = StorageClassSpecifier::REGISTER;
    }
    delete(next);
    return storageClassSpecifier;
}

vector<Type*> InstrParser::parseSpecifierQualifierList(Block* block) {
    vector<Type*> typeSpecifiers;
    Type* type = nullptr;
    Status status = parseTypeSpecifier(block, type);
    if(type)
        typeSpecifiers.push_back(type);
    parseTypeQualifier(block);
    //parseSpecifierQualifierList(block);
    return typeSpecifiers;
}

AssignStmt* InstrParser::parseStructDeclarator(Block* block) {
    AssignStmt* assignStmt = nullptr;
    Expr* value = nullptr;
    DeclType declType = VARDECL;
    IdentifierName* var = parseDeclarator(block, declType);
    if(var == nullptr) return nullptr;
    char c = p_tokenizer.lookAhead(1);
    if(c == ':') {
        p_tokenizer.nextChar(); //consume :
        char endDelim[] = {',', ';'};
        char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
        if(next == nullptr) {
            Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseStructDeclarator:", "value");
            return nullptr;
        }
        Expr* value = p_exprParser.parseExpressionStr(next);
        delete next;
        if(value == nullptr) {
            Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseStructDeclarator:", "value");
            return nullptr;
        }
    }
    assignStmt = new AssignStmt(DECL, var, value);
    return assignStmt;
}

Status InstrParser::parseStructDeclaratorList(Block* block, vector<AssignStmt*>& structDeclaratorList) {
    Status status = SUCCESS;
    AssignStmt* assignStmt = parseStructDeclarator(block);
    if(assignStmt == nullptr) return FAILURE;
    structDeclaratorList.push_back(assignStmt);
    char c = p_tokenizer.lookAhead(1);
    if(c == ',') {
        p_tokenizer.nextChar(); //consume ','
        status = parseStructDeclaratorList(block, structDeclaratorList);
    }
    return status;
}

Status InstrParser::parseStructDeclaration(Block* block, vector<AssignStmt*>& structDeclaration) {
    Status status = SUCCESS;
    const vector<Type*>& dataTypes = parseSpecifierQualifierList(block);
    if(dataTypes.empty())
        return Status::FIRST_MISMATCH;
    vector<AssignStmt*> structDeclaratorList;
    status = parseStructDeclaratorList(block, structDeclaratorList);
    for(AssignStmt* assignStmt : structDeclaratorList) {
        assignStmt->setDataType(dataTypes[0]);
        structDeclaration.push_back(assignStmt);
        cout <<"declaration specifier " <<dataTypes[0] << " " <<structDeclaration.size()<<endl;
    }
    p_tokenizer.nextChar(); //consume ;
    p_tokenizer.nextLine();
    return status;
}

Status InstrParser::parseStructDeclarationList(Block* block, vector<AssignStmt*>& structDeclarationList) {
    int pos = p_tokenizer.getPos();
    Status status = parseStructDeclaration(block, structDeclarationList);
    if(status == SUCCESS) {
        status = parseStructDeclarationList(block, structDeclarationList);
    } else if (status == FIRST_MISMATCH) {
        p_tokenizer.setPos(pos);
        status = SUCCESS;
    }
    return status;
}

StructOrUnion InstrParser::parseStructOrUnion(Block* block) {
    StructOrUnion structOrUnion = StructOrUnion::NOT_STRUCTORUNION;
    const char* next = p_tokenizer.nextWord(true);
    if(next == nullptr) return structOrUnion;
    if(strcmp(next,"struct")==0) {
        p_tokenizer.consumeWord();
        structOrUnion = StructOrUnion::STRUCT;
    }
    else if(strcmp(next,"union")==0) {
        p_tokenizer.consumeWord();
        structOrUnion = StructOrUnion::UNION;
    }
    delete(next);
    return structOrUnion;
}

Status InstrParser::parseStructOrUnionSpecifier(Block* block) {
    Status status = SUCCESS;
    StructOrUnion structOrUnion = parseStructOrUnion(block);
    if(structOrUnion == StructOrUnion::NOT_STRUCTORUNION) {
        return Status::FIRST_MISMATCH;
    }
    StructDeclStmt* stmt = new StructDeclStmt(STRUCT_DECL);
    Block* declBlock = new Block(block);
    stmt->setBlock(declBlock);
    block->addStatement(stmt);

    char* nextWord = p_tokenizer.nextWord();
    if(nextWord == nullptr) return FAILURE;
    IdentifierName* identifier = new IdentifierName(nextWord);
    stmt->setName(identifier);
    cout <<"create structdeclStmt " <<nextWord <<endl;

    p_tokenizer.nextChar(); //consume {
    p_tokenizer.nextLine();
    vector<AssignStmt*> structDeclarationList;
    status = parseStructDeclarationList(block, structDeclarationList);
    p_tokenizer.nextChar(); //consume }

    if(status == FAILURE) { return FAILURE; }
    p_tokenizer.nextLine();
    StructType* structType = new StructType(identifier->getName(), DataType::STRUCTSPECIFIER);
    cout <<"created new struct type " <<identifier->getName()<<endl;

    for(AssignStmt* assignStmt : structDeclarationList) {
        const IdentifierName* identifierName = assignStmt->getVar();
        assert(identifierName != nullptr);
        structType->addIdentifierNames(identifierName,assignStmt->getDataType());
    }
    p_types.push_back(structType);
    cout <<"Struct Decl stmt: size = " <<stmt->getBlock()->getSubStatements().size() << " parent size "
            <<block->getSubStatements().size() << " " <<*stmt <<" status " <<status <<endl;
    return status;
}

vector<Type*> InstrParser::parseDeclarationSpecifiers(Block* block) {
    cout <<"parsedeclaration specifiers " <<endl;
    StorageClassSpecifier s = parseStorageClassSpecifier(block);
    //cout <<"StorageClass specifier "<<s <<endl;
    Type* typeSpecifier = nullptr;
    parseTypeSpecifier(block, typeSpecifier);
    cout <<"Type specifier " <<typeSpecifier <<endl;
    TypeQualifier t = parseTypeQualifier(block);
    //cout <<"Type Qualiffier " <<t <<endl;
    vector<Type*> declarationSpecifiers;
    if(typeSpecifier)
        declarationSpecifiers.push_back(typeSpecifier);
    cout <<"parseDeclarationSpecifiers " <<declarationSpecifiers.size() <<endl;
    return declarationSpecifiers;
}

IdentifierName* InstrParser::parseParameterDecl(Block* block) {
    IdentifierName* declarator = nullptr;
    const vector<Type*>& declarationSpecifiers = parseDeclarationSpecifiers(block);
    if(declarationSpecifiers.empty()) {
        return declarator;
    }
    DeclType declType;
    declarator = parseDeclarator(block, declType);
    cout <<"parseparameterdecl returns "<< declType <<" " <<declarator<<endl;
    return declarator;
}

Status InstrParser::parseParameterList(Block* block, vector<IdentifierName*>& identifierList) {
    Status status = SUCCESS;
    IdentifierName* parameterDecl = parseParameterDecl(block);
    if(parameterDecl == nullptr) {
        cout <<"parameter decl null " <<endl;
        return status;
    }
    identifierList.push_back(parameterDecl);
    char next = p_tokenizer.lookAhead(1);
    if(next == ',') {
        p_tokenizer.nextChar(); //consume ,
        status = parseParameterList(block, identifierList);
    }
    return status;
}

Status InstrParser::parsePointer(Block* block, vector<PointerIdentifierName*>& identifierList) {
    Status status = SUCCESS;
    char c = p_tokenizer.lookAhead(1);
    if(c=='*') {
        p_tokenizer.nextChar(); //consume *
        status = parsePointer(block, identifierList);
        PointerIdentifierName* pointerIdentifier = new PointerIdentifierName("Pointer", nullptr);
        if(!identifierList.empty()) {
            pointerIdentifier->setPointsTo(identifierList.back());
        }
        identifierList.push_back(pointerIdentifier);
    }
    return status;
}

Status InstrParser::parseDirectDeclaratorPrime(Block* block, vector<IdentifierName*>& directDeclaratorPrime, DeclType& declType) {
    char next = p_tokenizer.lookAhead(1);
    switch(next) {
        case '(':
            p_tokenizer.nextChar(); // consume (
            parseParameterList(block, directDeclaratorPrime);
            p_tokenizer.nextChar(); // consume )
            declType = FUNCTIONDECL;
        break;
        case '[':
            p_tokenizer.nextChar(); //consume [
            char endDelim[] = {']'};
            char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
            if(next == nullptr) {
                Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseDirectDeclaratorPrime:", "value");
                break;
            }
            Expr* value = p_exprParser.parseExpressionStr(next);
            delete value;
            delete next;
            if(value == nullptr) {
                Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseDirectDeclaratorPrime:", "value");
                break;
            }
            p_tokenizer.nextChar(); //consume ]
            parseDirectDeclaratorPrime(block, directDeclaratorPrime, declType);
            PointerIdentifierName* pointerIdentifierName = new PointerIdentifierName("Pointer",nullptr);
            if(!directDeclaratorPrime.empty()) {
                pointerIdentifierName->setPointsTo(directDeclaratorPrime.back());
            }
            directDeclaratorPrime.push_back(pointerIdentifierName);
            declType = ARRAYDECL;

        break;
    }
    return SUCCESS;
}

IdentifierName* InstrParser::parseDirectDeclarator(Block* block, DeclType& declType) {
    IdentifierName* directDeclarator = nullptr;
    IdentifierName* next = nullptr;
    char nextChar = p_tokenizer.lookAhead(1);
    if(nextChar == '(') {
        p_tokenizer.nextChar(); //consume '('
        next = parseDeclarator(block, declType);
        p_tokenizer.nextChar(); //consume ')'
    } else {
        char* nextWord = p_tokenizer.nextWord();
        if(nextWord == nullptr) return directDeclarator;
        next = new IdentifierName(nextWord);
    }
    declType = VARDECL;
    vector<IdentifierName*> directDeclaratorPrime;
    parseDirectDeclaratorPrime(block, directDeclaratorPrime, declType);
    switch(declType) {
        case VARDECL:
            directDeclarator = next;
        break;
            //function decl and pointer to function
        case FUNCTIONDECL: {
                FunctionIdentifierName* functionIdentifierName = new FunctionIdentifierName(next->getName(), directDeclaratorPrime);
                PointerIdentifierName* pointerIdentifierName = dynamic_cast<PointerIdentifierName*>(next);
                if(pointerIdentifierName != nullptr) {
                    delete(pointerIdentifierName->getPointsTo());
                    pointerIdentifierName->setPointsTo(functionIdentifierName);
                    directDeclarator = pointerIdentifierName;
                }
                else {
                    delete next;
                    directDeclarator = functionIdentifierName;
                }
                cout <<"FunctionIdentfierName created " <<functionIdentifierName->getName() <<endl;
            }
        break;
        //array and pointer to array represented as PointerIdentifierName. Assigned ARRAY_DEFINITION.
        case ARRAYDECL:
                IdentifierName* arrayIdentifierName = directDeclaratorPrime.back();
                arrayIdentifierName->setName(next->getName());
                PointerIdentifierName* pointerToData = static_cast<PointerIdentifierName*>(directDeclaratorPrime.front());
                PointerIdentifierName* pointerIdentifierName = dynamic_cast<PointerIdentifierName*>(next);
                if(pointerIdentifierName != nullptr) {
                    const IdentifierName* pointsTo = pointerIdentifierName->getPointsTo();
                    pointerToData->setPointsTo(pointsTo);
                    pointerIdentifierName->setPointsTo(arrayIdentifierName);
                    directDeclarator = pointerIdentifierName;
                }
                else {
                    pointerToData->setPointsTo(next);
                    directDeclarator = arrayIdentifierName;
                }
        break;
    }
    return directDeclarator;
}

IdentifierName* InstrParser::parseDeclarator(Block* block, DeclType& declType) {
    IdentifierName* declarator = nullptr;
    char c = p_tokenizer.lookAhead(1);
    if(c == '*') {
        vector<PointerIdentifierName*> pointerNames;
        parsePointer(block, pointerNames);
        IdentifierName* directDeclarator = parseDirectDeclarator(block, declType);
        pointerNames.front()->setPointsTo(directDeclarator);
        declarator = pointerNames.back();
        declarator->setName(directDeclarator->getName());
     } else {
        declarator = parseDirectDeclarator(block, declType);
    }
    cout <<"parseDeclarator " <<declarator <<endl;
    return declarator;
}

Expr* InstrParser::parseInitializer(Block* block) {
    char endDelim[] = {';',','};
    char* next = p_tokenizer.nextWordUntil(endDelim, sizeof(endDelim));
    if(next == nullptr) { Logger::logMessage(ErrorCode::NOT_FOUND,  2, "InstrParser::parseAssign:", "value"); return nullptr; }
    Expr* value = p_exprParser.parseExpressionStr(next);
    delete next;
    if(value == nullptr) { Logger::logMessage(ErrorCode::NOT_PARSE,  2, "InstrParser::parseAssign:", "value"); return nullptr; }
    return value;
}

vector<Expr*> InstrParser::parseInitializerList(Block* block) {
    vector<Expr*> initializerList;
    Expr* initializer = parseInitializer(block);
    initializerList.push_back(initializer);
    char c = p_tokenizer.lookAhead(1);
    if(c == ',') {
        p_tokenizer.nextChar();
        parseInitializerList(block);
    }
    return initializerList;
}

AssignStmt* InstrParser::parseInitDeclarator(Block* block) {
    AssignStmt* assignStmt = nullptr;
    Expr* value = nullptr;
    DeclType declType = VARDECL;
    IdentifierName* var = parseDeclarator(block, declType);
    if(var == nullptr) return nullptr;
    char c = p_tokenizer.lookAhead(1);
    if(c == '=') {
        p_tokenizer.nextChar();
        value = parseInitializer(block);
    }
    else {
        if(declType == DeclType::ARRAYDECL) {
            value = new ArrayDefinition(false);
        }
        else {
            value = new Definition(false);
        }
    }
    assignStmt = new AssignStmt(DECL, var, value);
    return assignStmt;
}

Status InstrParser::parseInitDeclaratorList(Block* block, vector<AssignStmt*>& initDeclaratorList) {
    Status status = SUCCESS;
    AssignStmt* assignStmt = parseInitDeclarator(block);
    if(assignStmt == nullptr) return FAILURE;
    initDeclaratorList.push_back(assignStmt);
    char c = p_tokenizer.lookAhead(1);
    if(c == ',') {
        p_tokenizer.nextChar();
        status = parseInitDeclaratorList(block, initDeclaratorList);
    }
    return status;
}

Status InstrParser::parseDeclaration(Block* block, vector<AssignStmt*>& declaration) {
    cout <<"parsedeclaration begin " <<endl;
    Status status = SUCCESS;
    const vector<Type*>& declarationSpecifiers = parseDeclarationSpecifiers(block);
    if(declarationSpecifiers.empty()) {
        return FIRST_MISMATCH;
    }
    vector<AssignStmt*> initDeclaratorList;
    status = parseInitDeclaratorList(block, initDeclaratorList);
    if(declarationSpecifiers.empty()) return FAILURE;
    // set datatype, structtype and other datatypes
    for(AssignStmt* assignStmt : initDeclaratorList) {
        assignStmt->setDataType(declarationSpecifiers[0]);
        declaration.push_back(assignStmt);
        cout <<"declaration specifier " <<declarationSpecifiers[0] << " " <<declaration.size()<<endl;
    }
    p_tokenizer.nextChar(); //consume ';'

    cout <<"parseDeclaration " <<status <<endl;
    return status;
}

Status InstrParser::parseDeclarationList(Block* block, vector<AssignStmt*>& declarationList) {
    Status status = SUCCESS;
    int pos = p_tokenizer.getPos();
    status = parseDeclaration(block, declarationList);
    if(status == SUCCESS) {
        p_tokenizer.nextLine();
        status = parseDeclarationList(block, declarationList);
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

    const vector<Type*>& dataTypes = parseDeclarationSpecifiers(block);
    assert(!dataTypes.empty());
    DeclType declType;
    IdentifierName* identifier = parseDeclarator(block, declType);
    FunctionIdentifierName* functionIdentifierName = dynamic_cast<FunctionIdentifierName*>(identifier);
    if(functionIdentifierName == nullptr) return FAILURE;
    vector<IdentifierName*> parameterList = functionIdentifierName->getParameterList();
    for(IdentifierName* identifierName : parameterList) {
        Variable* var = makeVariable(block, identifierName, dataTypes[0]);
        block->getSymbolTable()->addSymbol(var);
    }
    stmt->setName(identifier);

    status = parseBlock(stmt->getBlock());
    if(status == FAILURE) { return FAILURE; }
    p_tokenizer.nextLine();
    block->getSymbolTable()->addFnSymbol(identifier->getName());
    block->getSymbolTable()->addSymbol(new Variable(identifier->getName(), VarType::FUNCTION));
    cout <<"Function Decl stmt: size = " <<stmt->getBlock()->getSubStatements().size() << " parent size "
            <<block->getSubStatements().size() << " " <<*stmt <<" status " <<status <<endl;
    return status;
}

Block* InstrParser::getBlock() const {
    return p_mainBlock;
}

void InstrParser::clear() {
    delete p_mainBlock;
}
