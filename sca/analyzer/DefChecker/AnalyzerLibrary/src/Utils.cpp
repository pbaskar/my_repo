/*
 * Utils.cpp
 *
 *  Created on: Sep 21, 2023
 *      Author: prbas_000
 */

#include "Utils.h"
#include <cstring>
#include <cassert>

Utils::Utils() {
    // TODO Auto-generated constructor stub

}

Utils::~Utils() {
    // TODO Auto-generated destructor stub
}

const char* Utils::makeWord(const char* word) {
    int len = strlen(word);
    char* p = new char[len+1];
    p[len] = '\0';
    strncpy(p, word,len);
    return p;
}

const char* Utils::makeWord(const char* word1, const char* word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    char* newName = new char[len1+len2+1];
    newName[len1] = '\0';
    strncpy(newName, word1, len1);
    newName[len1 + len2] = '\0';
    strcat(newName, word2);
    return newName;
}

bool Utils::isAlpha(char c) {
    if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
        return true;
    return false;
}

Status Utils::populateMallocFnCall(const Variable* lhs, Expr* value) {
    MallocFnCall* mallocFnCall = static_cast<MallocFnCall*>(value);
    PointerDefinition* previous = static_cast<PointerDefinition*>(mallocFnCall->toSimplifyDefinition());
    const PointerVariable* lhsVar = nullptr;
    while (true) {
        lhsVar = dynamic_cast<const PointerVariable*>(lhs);
        if (lhsVar == nullptr) break;
        lhs = lhsVar->getPointsTo();
        assert(lhs);
        if (lhs->getExprType() != ExprType::POINTERVARIABLE) break;
        PointerDefinition* pointsToDefinition = new PointerDefinition(false);
        previous->setPointsTo(pointsToDefinition);
        previous = pointsToDefinition;
    }
    Definition* pointsToDefinition = new Definition(false);
    previous->setPointsTo(pointsToDefinition);
    return SUCCESS;
}

Status Utils::populateDefinitions(const Variable* lhs, Expr* value) {
    if (lhs->getExprType() == ExprType::POINTERVARIABLE) {
        switch (value->getExprType()) {
        case ExprType::MALLOCFNCALL: {
            populateMallocFnCall(lhs, value);
        }
        break;
        //pointervar
        case ExprType::POINTERDEFINITION: {
            PointerDefinition* previous = static_cast<PointerDefinition*>(value);
            const PointerVariable* lhsVar = nullptr;
            while (true) {
                lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                if (lhsVar == nullptr) {
                    break;
                }
                lhs = lhsVar->getPointsTo();
                assert(lhs);
                if (lhs->getExprType() != ExprType::POINTERVARIABLE) break;
                PointerDefinition* pointsToDefinition = new PointerDefinition(false);
                previous->setPointsTo(pointsToDefinition);
                previous = pointsToDefinition;
            }
            assert(lhs);
            Definition* pointsToDefinition = new Definition(false);
            previous->setPointsTo(pointsToDefinition);
            if (lhs->getExprType() == ExprType::STRUCTVARIABLE) {
                populateDefinitions(lhs, pointsToDefinition);
            }
        }
        break;
        //array
        case ExprType::ARRAYDEFINITION: {
            PointerDefinition* previous = static_cast<PointerDefinition*>(value);
            const PointerVariable* lhsVar = nullptr;
            while (true) {
                lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                if (lhsVar == nullptr) break;
                lhs = lhsVar->getPointsTo();
                assert(lhs);
                if (lhs->getExprType() != ExprType::POINTERVARIABLE) break;
                PointerDefinition* pointsToDefinition = new PointerDefinition(true);
                previous->setPointsTo(pointsToDefinition);
                previous = pointsToDefinition;
            }
            Definition* pointsToDefinition = new Definition(false);
            previous->setPointsTo(pointsToDefinition);
        }
        break;
        case ExprType::ASSIGNOPERATOR: {
            AssignOperator* assignOperator = static_cast<AssignOperator*>(value);
            const Identifier* lhs = static_cast<const Identifier*>(assignOperator->getLeftOp());
            assert(lhs);
            const Variable* lhsVar = static_cast<const Variable*>(lhs->getVariable());
            Expr* rhs = assignOperator->toSimplifyRightOp();
            if (rhs->getExprType() == ExprType::MALLOCFNCALL) {
                populateMallocFnCall(lhsVar, rhs);
            }
        }
        break;
        default:;
        }
    }
    else if (lhs->getExprType() == ExprType::STRUCTVARIABLE) {
        assert(value);
        //Logger::getDebugStreamInstance() <<"Expression simplification struct variable  " <<lhs->getName() <<endl;
        switch (value->getExprType()) {
        case ExprType::DEFINITION: {
            Definition* definition = static_cast<Definition*>(value);
            const StructVariable* structVariable = static_cast<const StructVariable*>(lhs);
            vector<const Variable*> memVars = structVariable->getMemVars();
            for (const Variable* memVar : memVars) {
                Definition* memDefinition = nullptr;
                if (memVar->getExprType() == ExprType::STRUCTVARIABLE) {
                    memDefinition = new Definition(false);
                    populateDefinitions(memVar, memDefinition);
                }
                else if (memVar->getExprType() == ExprType::POINTERVARIABLE) {
                    memDefinition = new PointerDefinition(false);
                    populateDefinitions(memVar, memDefinition);
                }
                else {
                    memDefinition = new Definition(false);
                }
                definition->addMemDefinition(memDefinition);
            }
        }
        break;
        default:;
        }
    }
    return SUCCESS;
}