#include "Expr.h"
#include "SymbolTable.h"
#include "Utils.h"
#include<cassert>

void FunctionVariable::print(ostream& os) const {
    Variable::print(os);
    for(const Expr* identifier : p_functionIdentifiers) {
        os <<", holds function ";
        identifier->print(os);
    }
}

void Identifier::populateVariable(SymbolTable* symbolTable) {
    p_variable = symbolTable->fetchVariable(p_name);
    Logger::getDebugStreamInstance() << "Identifier::populateVariable populated " << p_name <<" " <<p_variable <<endl;
}

const Expr* Identifier::populateDerefVariable(SymbolTable* symbolTable, const Expr* structVar) {
    if(structVar) {
        setVariable(static_cast<const Variable*>(structVar));
        Logger::getDebugStreamInstance() << "Identifier::populateDerefVariable populated " << p_name <<" " << p_variable <<endl;
        return structVar;
    }
    Logger::getDebugStreamInstance() << "Identifier::populateDerefVariable populated " << p_name <<" " << p_variable <<endl;
    return symbolTable->fetchVariable(p_name);
}

const Expr* Identifier::populateStructVariable(SymbolTable* symbolTable, const Expr* structVar) {
    // find var from structVar member that has this name
    const char* name = getPointedName();
    const StructVariable* structVariable = static_cast<const StructVariable*>(structVar);
    const vector<const Variable*>& memVariables = structVariable->getMemVars();
    const Variable* memStructVar = nullptr;
    for(int i=0; i<memVariables.size(); i++) {
        if(strcmp(memVariables[i]->getName(), name)==0) {
            memStructVar = memVariables[i];
        }
    }
    setVariable(memStructVar);
    assert(memStructVar);
    Logger::getDebugStreamInstance() <<"Identifier::populateStructVariable "<<*memStructVar <<endl;
    return memStructVar;
}

const Expr* DereferenceOperator::populateDerefVariable(SymbolTable* symbolTable, const Expr* structVar) {
    const PointerVariable* pointerVariable = dynamic_cast<const PointerVariable*>(p_right->populateDerefVariable
                                                                                  (symbolTable, structVar));
    if(pointerVariable) {
        const Variable* pointsTo = pointerVariable->getPointsTo();
        const char* p = Utils::makeWord(pointsTo->getName());
        Identifier::setName(p);
        Identifier::setVariable(pointsTo);
        Logger::getDebugStreamInstance() <<"DerefOperator::populatederefvariable populated " << pointsTo <<endl;
        return pointsTo;
    }
    else {
        Logger::getDebugStreamInstance() <<"PopulateDerefVariable error " <<p_right->getExprType() <<endl; //error
    }
    return nullptr;
}

const Expr* DereferenceOperator::populateStructVariable(SymbolTable* symbolTable, const Expr* structVar) {
    // find var from structVar member that has this name
    const char* name = getPointedName();
    const StructVariable* structVariable = static_cast<const StructVariable*>(structVar);
    const vector<const Variable*>& memVariables = structVariable->getMemVars();
    const Expr* memStructVar = nullptr;
    for(int i=0; i<memVariables.size(); i++) {
        if(strcmp(memVariables[i]->getName(), name)==0) {
            memStructVar = memVariables[i];
        }
    }
    memStructVar = populateDerefVariable(symbolTable, memStructVar);
    assert(memStructVar);
    Logger::getDebugStreamInstance() <<"DereferenceOperator::populateStructVariable "<<*memStructVar <<endl;
    return memStructVar;
}

const Expr* DotOperator::populateDerefVariable(SymbolTable* symbolTable, const Expr* structVar) {
    const Expr* leftVar = p_left->populateDerefVariable(symbolTable);
    if(leftVar == nullptr) return leftVar;
    const Expr* pointedToVar = p_right->populateStructVariable(symbolTable, leftVar);
    assert(pointedToVar);
    Logger::getDebugStreamInstance()<<"DotOperator::populateDerefVariable " <<*pointedToVar <<endl;
    return pointedToVar;
}

const Expr* DotOperator::populateStructVariable(SymbolTable* symbolTable, const Expr* structVar) {
    // find var from structVar member that has leftVar name
    const char* name = p_left->getPointedName();
    const StructVariable* structVariable = static_cast<const StructVariable*>(structVar);
    const vector<const Variable*>& memVariables = structVariable->getMemVars();
    const Expr* memStructVar = nullptr;
    for(int i=0; i<memVariables.size(); i++) {
        if(strcmp(memVariables[i]->getName(), name)==0) {
            memStructVar = memVariables[i];
        }
    }
    if(memStructVar == nullptr) return memStructVar;
    p_left->populateDerefVariable(symbolTable, memStructVar);
    memStructVar = p_right->populateStructVariable(symbolTable, memStructVar);
    assert(memStructVar);
    Logger::getDebugStreamInstance() <<"DotOperator::populateStructVariable "<<*memStructVar <<endl;
    return memStructVar;
}

const Expr* AddressOfOperator::populateDerefVariable(SymbolTable* symbolTable, const Expr* structVar) {
    const Variable* pointsTo = dynamic_cast<const Variable*>(p_right->populateDerefVariable(symbolTable));
    if(pointsTo) {
        if(pointsTo->getExprType() == ExprType::ADDRESSOFVARIABLE) {
            Logger::getDebugStreamInstance() <<"Invalid address of operator usage " <<pointsTo->getName() <<endl;
            return nullptr;
        }
        const char* name = pointsTo->getName();
        int len = strlen(name);
        char* newName = new char(len+2);
        strncpy(newName, name, len);
        newName[len] = '&';
        newName[len+1] = '\0';
        //already in symboltable
        const Variable* addressOfVariable = symbolTable->fetchVariable(newName);
        if(addressOfVariable) {
            const char* p = Utils::makeWord(addressOfVariable->getName());
            Identifier::setName(p);
            Identifier::setVariable(addressOfVariable);
            return addressOfVariable;
        }
        const PointerVariable* pointerVariable = dynamic_cast<const PointerVariable*>(pointsTo->getAddress());
        if(pointerVariable) {
            const char* p = Utils::makeWord(pointerVariable->getName());
            Identifier::setName(p);
            Identifier::setVariable(pointerVariable);
        }
        else {
            //add to symbol table
            Identifier::setName(Utils::makeWord(newName));
            Variable* addressOf = new AddressOfVariable(newName, VarType::POINTER, pointsTo);
            symbolTable->addSymbol(addressOf);
            Identifier::setVariable(addressOf);
            Logger::getDebugStreamInstance() <<"addressOf variable populated " << addressOf <<endl;
            return addressOf;
        }
    }
    else {
        Logger::getDebugStreamInstance() <<"AddressOfOperator::populateDerefVariable pointsTo downcast to var failure " <<endl;
    }
    return nullptr;
}
