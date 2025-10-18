#ifndef EXPRSIMPLIFIER_H
#define EXPRSIMPLIFIER_H

#include "Utils.h"
#include "Stmt.h"
class ExprSimplifier
{
public:
    ExprSimplifier();

    Status simplify(Block* block);
    Status simplifyBlock(Block* block);
};

#endif // EXPRSIMPLIFIER_H
