/*
 * Expr.h
 *
 *  Created on: Nov 10, 2023
 *      Author: prbas_000
 */

#ifndef EXPR_H_
#define EXPR_H_
#include<cstring>
#include<iostream>
#include<vector>

class SymbolTable;
class Variable;
using namespace std;
enum ExprType {
    DEFINITION,
    CONSTANT,
    VARIABLE,
    POINTERVARIABLE,
    ADDRESSOFVARIABLE,
    IDENTIFIER,
    OPERATOR,
    UNARYOPERATOR,
    ADDRESSOFOPERATOR,
    DEREFERENCEOPERATOR,
    FUNCTIONCALL,
    MALLOCFNCALL,
    DELETEFNCALL,
    ASSIGNOPERATOR,
    INVALID
};

enum VarType {
    VALUE,
    POINTER
};

class FunctionCall;
class Expr {
public:
    Expr() {}
    virtual ~Expr() {}
    virtual ExprType getExprType() const=0;
    virtual void print(ostream& os) const=0;
    virtual void getRHSVariables(vector<const Expr*>& variables) const =0;
    virtual void getLHS(vector<const Expr*>& variables) const =0;
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) =0;
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const =0;
    virtual void populateVariable(SymbolTable* symbolTable)=0;
    virtual const Expr* populateDerefVariable(SymbolTable* symbolTable){ return nullptr; }
    friend ostream& operator<<(ostream& os, const Expr& expr) {
        //os <<expr.getExprType() <<" " ;
        expr.print(os);
        return os;
    }
};

class Operator : public Expr {
public:
    Operator(Expr* l, char o, Expr* r): p_left(l), p_op(o), p_right(r) {}
    virtual ~Operator() {
        delete p_left;
        delete p_right;
    }
    virtual ExprType getExprType() const { return OPERATOR; }
    void setLeftOp(Expr* left) { p_left = left; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< *p_left <<p_op << " " <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_left->getRHSVariables(variables);
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        p_left->getLHS(variables);
        p_right->getLHS(variables);
    }
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {
        p_left->getDerefIdentifiers(derefIdentifiers);
        p_right->getDerefIdentifiers(derefIdentifiers);
    }
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        p_left->getFunctionCalls(functionCalls);
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_left->populateVariable(symbolTable);
        p_right->populateVariable(symbolTable);
    }
    virtual const Expr* populateDerefVariable(SymbolTable* symbolTable) {
        return p_left->populateDerefVariable(symbolTable);
    }
private:
    Expr* p_left;
    char p_op;
    Expr* p_right;
};

class AssignOperator : public Expr {
public:
    AssignOperator(Expr* l, char o, Expr* r): p_left(l), p_op(o), p_right(r) {}
    virtual ~AssignOperator() {
        delete p_left;
        delete p_right;
    }
    virtual ExprType getExprType() const { return ASSIGNOPERATOR; }
    void setLeftOp(Expr* left) { p_left = left; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< *p_left <<p_op << " " <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        variables.push_back(p_left);
    }
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {
        p_left->getDerefIdentifiers(derefIdentifiers);
        p_right->getDerefIdentifiers(derefIdentifiers);
    }
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        p_left->getFunctionCalls(functionCalls);
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_left->populateVariable(symbolTable);
        p_right->populateVariable(symbolTable);
    }
private:
    Expr* p_left;
    char p_op;
    Expr* p_right;
};

class UnaryOperator : public Expr {
public:
    UnaryOperator(char o, Expr* r): p_op(o), p_right(r) {}
    virtual ~UnaryOperator() {
        delete p_right;
    }
    virtual ExprType getExprType() const { return UNARYOPERATOR; }
    void setRightOp(Expr* right) { p_right = right; }
    void setOp(char op) { p_op = op; }
    virtual void print(ostream& os) const{
        os<< p_op <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
        p_right->getLHS(variables);
    }
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {
        p_right->getDerefIdentifiers(derefIdentifiers);
    }
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_right->populateVariable(symbolTable);
    }
private:
    char p_op;
    Expr* p_right;
};

class Definition : public Expr {
public:
    Definition(bool valid) : p_valid(valid) {}
    virtual ~Definition() {}
    virtual ExprType getExprType() const { return DEFINITION; }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {}
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
    const bool isValid() const { return p_valid; }
    void setIsValid(bool valid) { p_valid = valid; }
    virtual void print(ostream& os) const {
        os << " " <<p_valid <<endl;
    }
private:
    bool p_valid;
};

class Variable : public Expr {
public:
    Variable(const char* n, VarType type): p_name(n), p_type(type) {}
    virtual ~Variable() { /*delete p_name;*/ } //Todo: release memory shared by ptr_ptr_p, ptr_p and p
    virtual ExprType getExprType() const { return VARIABLE; }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    void setAddress(const Variable* address) { p_address = address; }
    const Variable* getAddress() const { return p_address; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const { variables.push_back(this); }
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const Variable& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
    VarType p_type;
    const Variable* p_address;
};

class PointerVariable : public Variable {
public:
    PointerVariable(const char* n, VarType type, const Variable* pointsTo): Variable(n, type), p_pointsTo(pointsTo) {}
    virtual ~PointerVariable() { delete p_pointsTo; }
    virtual ExprType getExprType() const { return POINTERVARIABLE; }
    const Variable* getPointsTo() const { return p_pointsTo; }
    virtual void print(ostream& os) const{
        os <<"*"<<*p_pointsTo;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const { variables.push_back(this); }
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
protected:
    const Variable* p_pointsTo;
};

class AddressOfVariable : public PointerVariable {
public:
    AddressOfVariable(const char* n, VarType type, const Variable* pointsTo): PointerVariable(n, type, pointsTo) {
        p_definition = new Definition(true);
    }
    virtual void print(ostream& os) const{
        Variable::print(os);
    }
    virtual ~AddressOfVariable() {
        p_pointsTo = nullptr;
        delete p_definition;
    }
    virtual ExprType getExprType() const { return ADDRESSOFVARIABLE; }
    const Definition* getDefinition() const { return p_definition; }
private:
    const Definition* p_definition;
};

class Identifier : public Expr {
public:
    Identifier(const char* n): p_name(n) {}
    virtual ~Identifier() { /*delete p_name;*/ } //Todo: release memory shared by dereference operator **p, *p and p
    virtual ExprType getExprType() const { return IDENTIFIER; }
    void setName(const char* name) { p_name = name; }
    const char* getName() const { return p_name; }
    void setVariable(const Variable* variable) { p_variable = variable; }
    const Variable* getVariable() const { return p_variable; }
    virtual void print(ostream& os) const{
        os <<p_name << " ";
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const { variables.push_back(p_variable); }
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable);
    virtual const Expr* populateDerefVariable(SymbolTable* symbolTable);
    virtual void getLHSOnLeft(vector<const Expr*>& variables) const {
        variables.push_back(p_variable);
    }
    virtual void getRHSOnLeft(vector<const Expr*>& variables) const {
    }
    bool match(const char* name) {
        return strcmp(p_name, name) ==0;
    }
    bool operator==(const Identifier& other) {
        return strcmp(p_name, other.p_name) ==0;
    }
private:
    const char* p_name;
    const Variable* p_variable;
};

class DereferenceOperator : public Identifier {
public:
    DereferenceOperator(const char* name, Expr* r): Identifier(name), p_right(r) {}
    virtual ~DereferenceOperator() {
        delete p_right;
    }
    virtual ExprType getExprType() const { return DEREFERENCEOPERATOR; }
    void setRightOp(Expr* right) { p_right = right; }
    virtual void print(ostream& os) const{
        os<<"*"<<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        Identifier::getRHSVariables(variables);
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
    }
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {
        derefIdentifiers.push_back(this);
    }
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_right->populateVariable(symbolTable);
    }
    virtual const Expr* populateDerefVariable(SymbolTable* symbolTable) {
        const PointerVariable* pointerVariable = dynamic_cast<const PointerVariable*>(p_right->populateDerefVariable(symbolTable));
        if(pointerVariable) {
            const Variable* pointsTo = pointerVariable->getPointsTo();
            Identifier::setName(pointsTo->getName());
            Identifier::setVariable(pointsTo);
            cout <<"deref variable populated " << pointsTo <<endl;
            return pointsTo;
        }
        else {
            cout <<"PopulateDerefVariable error " <<p_right->getExprType() <<endl; //error
        }
        return nullptr;
    }
    virtual void getLHSOnLeft(vector<const Expr*>& variables) const {
        variables.push_back(Identifier::getVariable());
    }
    virtual void getRHSOnLeft(vector<const Expr*>& variables) const {
        p_right->getRHSVariables(variables);
    }
private:
    Expr* p_right;
};

class AddressOfOperator : public Identifier {
public:
    AddressOfOperator(const char* name, Expr* r): Identifier(name), p_right(r) {}
    virtual ~AddressOfOperator() {
        delete p_right;
    }
    virtual ExprType getExprType() const { return ADDRESSOFOPERATOR; }
    void setRightOp(Expr* right) { p_right = right; }
    virtual void print(ostream& os) const{
        os<< "&" <<*p_right;
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        Identifier::getRHSVariables(variables);
        p_right->getRHSVariables(variables);
    }
    virtual void getLHS(vector<const Expr*>& variables) const {
    }
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {
        derefIdentifiers.push_back(this);
    }
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        p_right->getFunctionCalls(functionCalls);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        p_right->populateVariable(symbolTable);
    }
    virtual const Expr* populateDerefVariable(SymbolTable* symbolTable);
private:
    Expr* p_right;
};

class Constant : public Expr {
public:
    Constant(int n): p_number(n) {}
    virtual ~Constant() {}
    virtual ExprType getExprType() const { return CONSTANT; }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {}
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {}
    virtual void populateVariable(SymbolTable* symbolTable) {}
    void setNumber(int num) { p_number = num; }
    virtual void print(ostream& os) const {
        os <<p_number <<" ";
    }
private:
    int p_number;
};

class FunctionCall : public Expr {
public:
    FunctionCall(Expr* identifier, vector<Expr*> arguments): p_functionName(identifier), p_arguments(arguments) {}
    virtual ~FunctionCall() {
        delete p_functionName;
        for(Expr* expr : p_arguments) {
            delete expr;
        }
    }
    virtual ExprType getExprType() const { return FUNCTIONCALL; }
    virtual void print(ostream& os) const{
        os<<*p_functionName << " ";
        for(Expr* expr : p_arguments) {
            os <<*expr << " ";
        }
    }
    virtual void getRHSVariables(vector<const Expr*>& variables) const {
        for(Expr* expr : p_arguments) {
            expr->getRHSVariables(variables);
        }
    }
    virtual void getLHS(vector<const Expr*>& variables) const {}
    virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        functionCalls.push_back(this);
    }
    virtual void populateVariable(SymbolTable* symbolTable) {
        for(Expr* expr : p_arguments) {
            expr->populateVariable(symbolTable);
        }
    }
    const Expr* getName() const { return p_functionName; }
    const vector<Expr*> getArguments() const { return p_arguments; }
private:
    Expr* p_functionName;
    vector<Expr*> p_arguments;
};

class MallocFnCall : public FunctionCall {
public:
    MallocFnCall(Expr* identifier, vector<Expr*> arguments): FunctionCall(identifier, arguments) {
        p_definition = new Definition(true);
    }
    virtual ~MallocFnCall() {
        delete p_definition;
    }
    virtual ExprType getExprType() const { return MALLOCFNCALL; }
    virtual void print(ostream& os) const{
        FunctionCall::print(os);
    }
    //virtual void getRHSVariables(vector<const Expr*>& variables) const {}
    //virtual void getLHS(vector<const Expr*>& variables) const {}
    //virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        //functionCalls.push_back(this);
    }
    const Definition* getDefinition() const { return p_definition; }
    //virtual void populateVariable(SymbolTable* symbolTable) {}
private:
    Definition* p_definition;
};

class DeleteFnCall : public FunctionCall {
public:
    DeleteFnCall(Expr* identifier, vector<Expr*> arguments): FunctionCall(identifier, arguments) {}
    virtual ~DeleteFnCall() {
    }
    virtual ExprType getExprType() const { return DELETEFNCALL; }
    virtual void print(ostream& os) const{
        FunctionCall::print(os);
    }
    //virtual void getRHSVariables(vector<const Expr*>& variables) const {}
    virtual void getLHS(vector<const Expr*>& variables) const {
        const vector<Expr*> arguments = FunctionCall::getArguments();
        for(const Expr* expr : arguments) {
            variables.push_back(expr);
        }
    }
    //virtual void getDerefIdentifiers(vector<Expr*>& derefIdentifiers) {}
    virtual void getFunctionCalls(vector<const Expr*>& functionCalls) const {
        //functionCalls.push_back(this);
    }
    //virtual void populateVariable(SymbolTable* symbolTable) {}
private:
};

#endif /* EXPR_H_ */
