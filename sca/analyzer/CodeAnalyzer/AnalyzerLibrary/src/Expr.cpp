#include "Expr.h"
#include "SymbolTable.h"

void Identifier::populateVariable(SymbolTable* symbolTable) {
    p_variable = symbolTable->fetchVariable(p_name);
    cout << "p_variable populated " << p_name <<" " <<p_variable <<endl;
}

const Expr* Identifier::populateDerefVariable(SymbolTable* symbolTable) {
    return symbolTable->fetchVariable(p_name);
    cout << "Deref populated " << p_name <<" " << p_variable <<endl;
}
