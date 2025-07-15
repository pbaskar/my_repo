#include "Expr.h"
#include "SymbolTable.h"

void FunctionVariable::print(ostream& os) const {
    Variable::print(os);
    for(const Expr* identifier : p_functionIdentifiers) {
        os <<", holds function ";
        identifier->print(os);
    }
}

void Identifier::populateVariable(SymbolTable* symbolTable) {
    p_variable = symbolTable->fetchVariable(p_name);
    cout << "Identifier::populateVariable populated " << p_name <<" " <<p_variable <<endl;
}

const Expr* Identifier::populateDerefVariable(SymbolTable* symbolTable) {
    cout << "Identifier::populateDerefVariable populated " << p_name <<" " << p_variable <<endl;
    return symbolTable->fetchVariable(p_name);
}

const Expr* AddressOfOperator::populateDerefVariable(SymbolTable* symbolTable) {
    const Variable* pointsTo = dynamic_cast<const Variable*>(p_right->populateDerefVariable(symbolTable));
    if(pointsTo) {
        if(pointsTo->getExprType() == ExprType::ADDRESSOFVARIABLE) {
            cout <<"Invalid address of operator usage " <<pointsTo->getName() <<endl;
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
            Identifier::setName(addressOfVariable->getName());
            Identifier::setVariable(addressOfVariable);
            return addressOfVariable;
        }
        const PointerVariable* pointerVariable = dynamic_cast<const PointerVariable*>(pointsTo->getAddress());
        if(pointerVariable) {
            Identifier::setName(pointerVariable->getName());
            Identifier::setVariable(pointerVariable);
        }
        else {
            Identifier::setName(newName);
            //add to symbol table
            Variable* addressOf = new AddressOfVariable(newName, VarType::POINTER, pointsTo);
            symbolTable->addSymbol(addressOf);
            Identifier::setVariable(addressOf);
            cout <<"addressOf variable populated " << addressOf <<endl;
            return addressOf;
        }
    }
    else {
        cout <<"AddressOfOperator::populateDerefVariable pointsTo downcast to var failure " <<endl;
    }
    return nullptr;
}
