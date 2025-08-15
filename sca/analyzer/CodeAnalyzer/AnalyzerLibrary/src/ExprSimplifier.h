#ifndef EXPRSIMPLIFIER_H
#define EXPRSIMPLIFIER_H

#include "Utils.h"
#include "Stmt.h"
class ExprSimplifier
{
public:
    ExprSimplifier();

    Status simplify(Block* block);
    Status populateMallocFnCall(const Variable* lhs, Expr* value);
    Status populateDefinitions(const Variable* lhs, Expr* value);
    Status simplifyBlock(Block* block);
};

#endif // EXPRSIMPLIFIER_H
