#include "Expr.h"
#include "SymbolTable.h"

void Identifier::populateVariable(SymbolTable* symbolTable) {
    p_variable = symbolTable->fetchVariable(p_name);
    cout << "p_variable populated " << p_name <<endl;
}
