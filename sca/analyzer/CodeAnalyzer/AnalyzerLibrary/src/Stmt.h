/*
 * Stmt.h
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */

#ifndef SRC_STMT_H_
#define SRC_STMT_H_

#include "Expr.h"
#include "SymbolTable.h"
#include<list>

using namespace std;
enum StmtType { DECL, ASSIGN, IF, ELSE, WHILE, FOR, FUNC_DECL, FUNC_CALL, STRUCT_DECL };
enum DeclType {
    FUNCTIONDECL,
    VARDECL,
    ARRAYDECL
};
enum TypeQualifier {
    CONST,
    VOLATILE
};
enum StructOrUnion {
    STRUCT,
    UNION,
    NOT_STRUCTORUNION
};
enum StorageClassSpecifier {
    TYPEDEF,
    EXTERN,
    STATIC,
    AUTO,
    REGISTER
};

static const char* dataTypes[]= {"void", "char", "short", "int", "long", "float", "double", "signed", "unsigned", "struct",
                                 "union", "enum", "typename"};
static const size_t dataTypesSize = sizeof(dataTypes)/sizeof(dataTypes[0]);

class IdentifierName {
public:
    IdentifierName(const char* n): p_name(n) {}
    virtual ~IdentifierName() { /*delete p_name;*/ }
    virtual DeclType getType() const { return DeclType::VARDECL; }
    friend ostream& operator<<(ostream& os, IdentifierName& identifierName) {
        identifierName.print(os);
        return os;
    }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const IdentifierName& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
};

class FunctionIdentifierName : public IdentifierName {
public:
    FunctionIdentifierName(const char* name, vector<IdentifierName*> parameterList):
        IdentifierName(name), p_parameterList(parameterList) {}
    virtual ~FunctionIdentifierName() {
        for(int i=0; i<p_parameterList.size(); i++) {
            delete p_parameterList[i];
        }
    }
    virtual DeclType getType() const { return DeclType::FUNCTIONDECL; }
    friend ostream& operator<<(ostream& os, FunctionIdentifierName& functionIdentifierName) {
        functionIdentifierName.print(os);
        return os;
    }
    virtual void print(ostream& os) const{
        IdentifierName::print(os);
        os <<"arguments ";
        for(int i=0; i<p_parameterList.size(); i++) {
            os <<*p_parameterList[i] << " ";
        }
    }
    const vector<IdentifierName*>& getParameterList() { return p_parameterList; }
private:
    vector<IdentifierName*> p_parameterList;
};

class PointerIdentifierName : public IdentifierName {
public:
    PointerIdentifierName(const char* name, IdentifierName* pointsTo):
        IdentifierName(name), p_pointsTo(pointsTo) {}
    virtual ~PointerIdentifierName() {
        delete p_pointsTo;
    }
    virtual DeclType getType() const { return p_pointsTo->getType(); }
    virtual void print(ostream& os) const{
        IdentifierName::print(os);
        p_pointsTo->print(os);
    }
    void setPointsTo(const IdentifierName* pointsTo) {
        p_pointsTo = pointsTo;
    }
    const IdentifierName* getPointsTo() const {
        return p_pointsTo;
    }
private:
    const IdentifierName* p_pointsTo;
};

class Type {
public:
    Type(DataType dataType) : p_dataType(dataType) {}
    virtual ~Type() {}
    DataType getDataType() { return p_dataType; }
    friend ostream& operator<<(ostream& os, Type& type) {
        os << type.p_dataType <<" ";
        return os;
    }
private:
    DataType p_dataType;
};

class NamedType : public Type {
public:
    NamedType(const char* name, DataType dataType) : Type(dataType), p_name(name) {}
    virtual ~NamedType() {
        delete p_name;
    }
    const char* getName() const { return p_name; }
private:
    const char* p_name;
};

class StructType : public NamedType {
public:
    StructType(const char* name, DataType dataType) : NamedType(name, dataType) {
    }
    virtual ~StructType() {
        for(int i=0; i<p_memIdentifierNames.size(); i++) {
            delete p_memIdentifierNames[i].first;
        }
    }
    void addIdentifierNames(const IdentifierName* identifierName, Type* dataType) {
        p_memIdentifierNames.push_back(pair<const IdentifierName*,Type*>(identifierName, dataType));
    }
    const vector<pair<const IdentifierName*,Type*>>& getMemIdentifierNames() const { return p_memIdentifierNames; }
private:
    vector<pair<const IdentifierName*, Type*>> p_memIdentifierNames;
};

class EnumType : public Type {
    EnumType(DataType dataType) : Type(dataType) {}
};

class Stmt {
public:
    Stmt(StmtType type): p_type(type) {}
    virtual ~Stmt(){}
    friend ostream& operator<<(ostream& os, Stmt& stmt) {
        stmt.print(os);
        return os;
    }
    virtual void print(ostream& os)=0;
    StmtType getType() { return p_type; }
    StmtType p_type;
};

class AssignStmt : public Stmt {
public:
    AssignStmt(StmtType type): Stmt(type), p_var(0), p_value(0), p_dataType(0) {}
    AssignStmt(StmtType type, IdentifierName* var, Expr* value) :
        Stmt(type), p_var(var), p_value(value), p_dataType(0){
    }
    virtual ~AssignStmt() {
        if(p_var)
            delete p_var;
        if(p_value)
            delete p_value;
        if(p_dataType)
           delete p_dataType;
    }
    virtual void print(ostream& os) {
        if(p_var) os << "Assign statement: name " <<*p_var << " type " << p_type;
        if(p_value) os << " value " << *p_value << " value exprType " << p_value->getExprType();
        if(p_dataType) os << " data type " <<p_dataType <<endl;
    }
    void setVar(IdentifierName* var) { p_var = var; }
    void setValue(Expr* value) { p_value = value; }
    void setDataType(Type* type) { p_dataType = type; }
    Type* getDataType() const { return p_dataType; }
    const IdentifierName* getVar() const { return p_var; }
    const Expr* getValue() const { return p_value; }
    Expr* toSimplify() { return p_value; }
private:
    IdentifierName* p_var;
    Expr* p_value;
    Type* p_dataType;
};

class Block {
public:
    Block(){ p_symbolTable = new SymbolTable; }
    Block(const Block* parent) { p_symbolTable = new SymbolTable(parent->p_symbolTable); }
    ~Block() {
        for(Stmt* stmt : p_subStatements) {
            delete stmt;
        }
        delete p_symbolTable;
    }
    void addStatement(Stmt* stmt) {
        p_subStatements.push_back(stmt);
    }
    Variable* addSymbol(const char* name, VarType varType) {
        return p_symbolTable->addSymbol(name, varType);
    }
    Variable* fetchVariable(char* name) {
        return p_symbolTable->fetchVariable(name);
    }
    list<Stmt*>& toSimplify() { return p_subStatements; }
    const list<Stmt*>& getSubStatements() const { return p_subStatements; }
    SymbolTable* getSymbolTable() const { return p_symbolTable; }
private:
    list<Stmt*> p_subStatements;
    SymbolTable* p_symbolTable;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(StmtType type): Stmt(type), p_condition(0), p_block(0) {}
    virtual ~WhileStmt() {
        if(p_condition)
            delete p_condition;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << "type " << p_type <<
        if(p_condition)
            os << " condition " <<*p_condition <<" ";
    }
    const Expr* getCondition() const { return p_condition; }
    Expr* toSimplifyCondition() { return p_condition; }
    Block* getBlock() const { return p_block; }
    void setCondition(Expr* condition) { p_condition = condition; }
    void setBlock(Block* block) {p_block = block; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    Expr* p_condition;
    Block* p_block;
};

class ForStmt : public Stmt {
public:
    ForStmt(StmtType type): Stmt(type), p_initStmt(0), p_condition(0), p_postExpr(0), p_block(0) {}
    virtual ~ForStmt() {
        if(p_initStmt)
            delete p_initStmt;
        if(p_condition)
            delete p_condition;
        if(p_postExpr)
            delete p_postExpr;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << "type " << p_type <<
        if(p_initStmt)
            os << "init " <<*p_initStmt <<" ";
        if(p_condition)
            os << " condition " <<*p_condition <<" ";
        if(p_postExpr)
            os << " postExpr " <<*p_postExpr <<" ";
    }
    const AssignStmt* getInitStmt() const { return p_initStmt; }
    const Expr* getInitExpr() const {return p_initStmt->getValue(); }
    Expr* toSimplifyInitExpr() { return p_initStmt->toSimplify(); }
    const Expr* getCondition() const { return p_condition; }
    Expr* toSimplifyCondition() { return p_condition; }
    const Expr* getPostExpr() const { return p_postExpr; }
    Expr* toSimplifyPostExpr() { return p_postExpr; }
    Block* getBlock() const { return p_block; }
    void setInitStmt(AssignStmt* initStmt) { p_initStmt = initStmt; }
    void setCondition(Expr* condition) { p_condition = condition; }
    void setPostExpr(Expr* postExpr) { p_postExpr = postExpr; }
    void setBlock(Block* block) {p_block = block; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    AssignStmt* p_initStmt;
    Expr* p_condition;
    Expr* p_postExpr;
    Block* p_block;
};

class IfStmt : public Stmt {
public:
    IfStmt(StmtType type): Stmt(type), p_condition(0), p_else(0), p_block(0) {}
    virtual ~IfStmt() {
        if(p_condition)
            delete p_condition;
        delete p_else;
        delete p_block;
    }
    virtual void print(ostream& os) {
        //os << " type " << p_type << " substatements " <<p_subStatements.size() <<" ";
        if(p_condition)
            os <<" condition " <<*p_condition <<" ";
    }
    const Expr* getCondition() const { return p_condition; }
    Expr* toSimplifyCondition() { return p_condition; }
    const IfStmt* getElse() const { return p_else; }
    Block* getBlock() const { return p_block; }
    void setCondition(Expr* condition) { p_condition = condition; }
    void setElse(IfStmt* elseStmt) { p_else = elseStmt; }
    void setBlock(Block* block) {p_block = block; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    Expr* p_condition;
    IfStmt* p_else;
    Block* p_block;
};

class FunctionDeclStmt : public Stmt {
public:
    FunctionDeclStmt(StmtType type): Stmt(type), p_name(0), p_block(0) {}
    virtual ~FunctionDeclStmt() {
        delete p_block;
        delete p_name;
    }
    virtual void print(ostream& os) {
        os << " Function name " << *p_name;
    }
    Block* getBlock() const { return p_block; }
    IdentifierName* getName() const { return p_name; }
    void setBlock(Block* block) {p_block = block; }
    void setName(IdentifierName* name) { p_name = name; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    IdentifierName* p_name;
    Block* p_block;
};

class FunctionCallStmt : public Stmt {
public:
    FunctionCallStmt(StmtType type, const Expr* name, const vector<Expr*> actualArguments):
        Stmt(type), p_name(name), p_actualArguments(actualArguments) {}
    virtual ~FunctionCallStmt() {
    }
    virtual void print(ostream& os) {
        os << " Function name " << *p_name << " arguments ";
        for(Expr* v : p_actualArguments) {
            os << *v << " ";
        }
    }
    const Expr* getName() const { return p_name; }
    const vector<Expr*>& getActualArguments() const { return p_actualArguments; }
    void setName(Expr* name) { p_name = name; }
    void addActualArgument(Expr* argument) { p_actualArguments.push_back(argument); }
private:
    const Expr* p_name;
    vector<Expr*> p_actualArguments;
};

class StructDeclStmt : public Stmt {
public:
    StructDeclStmt(StmtType type): Stmt(type), p_name(0), p_block(0) {}
    virtual ~StructDeclStmt() {
        delete p_block;
        delete p_name;
    }
    virtual void print(ostream& os) {
        os << " Struct decl name " << *p_name;
    }
    Block* getBlock() const { return p_block; }
    IdentifierName* getName() const { return p_name; }
    void setBlock(Block* block) {p_block = block; }
    void setName(IdentifierName* name) { p_name = name; }
    void addStatement(Stmt* stmt) { p_block->addStatement(stmt); }
private:
    IdentifierName* p_name;
    Block* p_block;
};

#endif /* SRC_STMT_H_ */
