#include "ExprSimplifier.h"
#include "Stmt.h"
#include <cassert>

ExprSimplifier::ExprSimplifier()
{

}

Status ExprSimplifier::simplify(Block* block) {
    Status status = SUCCESS;
    status = simplifyBlock(block);
    return status;
}

Status ExprSimplifier::populateMallocFnCall(const Variable* lhs, Expr* value) {
    MallocFnCall* mallocFnCall = static_cast<MallocFnCall*>(value);
    PointerDefinition* previous = mallocFnCall->toSimplifyDefinition();
    const PointerVariable* lhsVar = nullptr;
    while(true) {
        lhsVar = dynamic_cast<const PointerVariable*>(lhs);
        if(lhsVar == nullptr) break;
        lhs = lhsVar->getPointsTo();
        assert(lhs);
        if(lhs->getExprType() != ExprType::POINTERVARIABLE) break;
        PointerDefinition* pointsToDefinition = new PointerDefinition(false);
        previous->setPointsTo(pointsToDefinition);
        previous = pointsToDefinition;
    }
    Definition* pointsToDefinition = new Definition(false);
    previous->setPointsTo(pointsToDefinition);
    return SUCCESS;
}

Status ExprSimplifier::populateDefinitions(const Variable* lhs, Expr* value) {
    if(lhs->getExprType() == ExprType::POINTERVARIABLE) {
        switch(value->getExprType()) {
            case ExprType::MALLOCFNCALL: {
                populateMallocFnCall(lhs, value);
            }
            break;
            //pointervar
            case ExprType::POINTERDEFINITION: {
                PointerDefinition* previous = static_cast<PointerDefinition*>(value);
                const PointerVariable* lhsVar = nullptr;
                while(true) {
                    lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                    if(lhsVar == nullptr) {
                        break;
                    }
                    lhs = lhsVar->getPointsTo();
                    assert(lhs);
                    if(lhs->getExprType() != ExprType::POINTERVARIABLE) break;
                    PointerDefinition* pointsToDefinition = new PointerDefinition(false);
                    previous->setPointsTo(pointsToDefinition);
                    previous = pointsToDefinition;
                }
                assert(lhs);
                Definition* pointsToDefinition = new Definition(false);
                previous->setPointsTo(pointsToDefinition);
                if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
                    populateDefinitions(lhs, pointsToDefinition);
                }
            }
            break;
            //array
            case ExprType::ARRAYDEFINITION: {
                PointerDefinition* previous = static_cast<PointerDefinition*>(value);
                const PointerVariable* lhsVar = nullptr;
                while(true) {
                    lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                    if(lhsVar == nullptr) break;
                    lhs = lhsVar->getPointsTo();
                    assert(lhs);
                    if(lhs->getExprType() != ExprType::POINTERVARIABLE) break;
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
                if(rhs->getExprType() == ExprType::MALLOCFNCALL) {
                    populateMallocFnCall(lhsVar, rhs);
                }
            }
            break;
            default:;
        }
    }
    else if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
        assert(value);
        //Logger::getDebugStreamInstance() <<"Expression simplification struct variable  " <<lhs->getName() <<endl;
        switch(value->getExprType()) {
            case ExprType::DEFINITION: {
                Definition* definition = static_cast<Definition*>(value);
                const StructVariable* structVariable = static_cast<const StructVariable*>(lhs);
                vector<const Variable*> memVars = structVariable->getMemVars();
                for(const Variable* memVar : memVars) {
                    Definition* memDefinition = nullptr;
                    if(memVar->getExprType() == ExprType::STRUCTVARIABLE) {
                        memDefinition = new Definition(false);
                        populateDefinitions(memVar, memDefinition);
                    } else if(memVar->getExprType() == ExprType::POINTERVARIABLE) {
                        memDefinition = new PointerDefinition(false);
                        populateDefinitions(memVar, memDefinition);
                    } else {
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

Status ExprSimplifier::simplifyBlock(Block* block) {
    Status status = SUCCESS;
    list<Stmt*>& stmtList = block->toSimplify();
    for(list<Stmt*>::const_iterator it = stmtList.begin(); it != stmtList.end(); it++) {
        Stmt* stmt = *it;
        StmtType type = (*it)->getType();
        switch(type) {
        case DECL:
        case ASSIGN: {
            AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
            Logger::getDebugStreamInstance() << "Assign Stmt: " <<*stmt <<endl;
            //Logger::getDebugStreamInstance() << " no of substatements " <<block->getSubStatements().size() <<endl;
            Expr* value = assignStmt->toSimplify();

            if(value) {
                value->populateVariable(block->getSymbolTable());
                vector<Expr*> derefIdentifiers;
                value->getDerefIdentifiers(derefIdentifiers);
                for(int i=0; i<derefIdentifiers.size(); i++) {
                    derefIdentifiers[i]->populateDerefVariable(block->getSymbolTable());
                }
                vector<const Expr*> functionCalls;
                value->getFunctionCalls(functionCalls);
                for(int i=0; i<functionCalls.size(); i++) {
                    const FunctionCall* functionCallExpr = static_cast<const FunctionCall*>(functionCalls[i]);
                    FunctionCallStmt* functionCallStmt = new FunctionCallStmt(FUNC_CALL, functionCallExpr->getName(),
                                                                              functionCallExpr->getArguments());
                    Logger::getDebugStreamInstance() <<"Function call stmt inserted for Name "<<functionCallExpr->getName() << " Name ExprType " <<
                           functionCallExpr->getName()->getExprType() <<endl;
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }

                //fetch LHS from declaration and assignment
                const Variable* lhs = nullptr;
                const IdentifierName* identifierName = assignStmt->getVar();
                if(identifierName) {
                    lhs =  block->getSymbolTable()->fetchVariable(identifierName->getName());
                }
                if(!lhs) {
                    vector<const Expr*> lhsVars;
                    value->getLHS(lhsVars);
                    if(!lhsVars.empty()) {
                        const Identifier* identifier = static_cast<const Identifier*>(lhsVars[0]);
                        assert(identifier);
                        Logger::getDebugStreamInstance() <<"Identifier name, associated variable " <<identifier->getName() <<" " <<identifier->getVariable() <<endl;
                        //lhs can hold Dotoperator which has no associated variable
                        lhs = identifier->getVariable();
                    }
                }

                //Populate definitions for declarations and assignments(malloc)
                if(lhs) {
                    populateDefinitions(lhs, value);
                }

                // append rhs to function pointer var possible pointsTo function Identifiers
                const Variable* rhs = nullptr;
                if(identifierName) {
                    if(identifierName->getType() != DeclType::FUNCTIONDECL) continue;
                }
                vector<const Expr*> fnIdentifiers;
                if(!lhs || lhs->getVarType() != VarType::FUNCTION) continue;
                value->getRHSVariables(fnIdentifiers);
                if(fnIdentifiers.empty()) {  Logger::getDebugStreamInstance() <<"Bad function name assigned to function pointer " <<identifierName->getName() <<endl; continue; }
                assert(fnIdentifiers[0]);
                rhs = static_cast<const Variable*>(fnIdentifiers[0]);

                assert(lhs->getExprType() == ExprType::POINTERVARIABLE);
                const PointerVariable* pointerVariable = static_cast<const PointerVariable*>(lhs);
                lhs = pointerVariable->getPointsTo();
                const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(rhs);
                //&function name on rhs or directly function name
                if(rhsPointer != nullptr) {
                    rhs = rhsPointer->getPointsTo();
                }
                const FunctionVariable* functionVariable = dynamic_cast<const FunctionVariable*>(lhs);
                if(functionVariable == nullptr) {
                    Logger::getDebugStreamInstance() <<"Function pointer has no valid function Variable " <<identifierName->getName() <<endl;
                    continue;
                }
                assert(rhs);
                functionVariable->addFunction(rhs);
            }
        }
        break;
        case IF:
        case ELSE: {
            IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
            Logger::getDebugStreamInstance() << "If Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            Expr* condition = ifStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(block->getSymbolTable());
            }
            status = simplifyBlock(ifStmt->getBlock());

            const IfStmt* elseStmt = ifStmt->getElse();
            if(elseStmt) {
                Logger::getDebugStreamInstance() << "Else Stmt: no of substatements " << " " <<block->getSubStatements().size() <<endl;
                status = simplifyBlock(elseStmt->getBlock());
            }
        }
        break;
        case WHILE: {
            WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
            Logger::getDebugStreamInstance() << "While Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            SymbolTable* whileSymbolTable = whileStmt->getBlock()->getSymbolTable();
            Expr* condition = whileStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(whileSymbolTable);
            }
            status = simplifyBlock(whileStmt->getBlock());
        }
        break;
        case FOR: {
            ForStmt* forStmt = static_cast<ForStmt*>(stmt);
            Logger::getDebugStreamInstance() << "For Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            SymbolTable* forSymbolTable = forStmt->getBlock()->getSymbolTable();
            Expr* initExpr = forStmt->toSimplifyInitExpr();
            if(initExpr) {
                initExpr->populateVariable(forSymbolTable);
            }
            Expr* condition = forStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(forSymbolTable);
            }
            Expr* postExpr = forStmt->toSimplifyPostExpr();
            if(postExpr) {
                postExpr->populateVariable(forSymbolTable);
            }
            status = simplifyBlock(forStmt->getBlock());
        }
        break;
        case FUNC_DECL: {
            FunctionDeclStmt* functionDeclStmt = static_cast<FunctionDeclStmt*>(stmt);
            Logger::getDebugStreamInstance() << "Func Decl Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            status = simplifyBlock(functionDeclStmt->getBlock());
        }
        break;
        default: {
            Logger::getDebugStreamInstance() <<"error " <<endl;
        }
        break;
        }
    }
    return status;
}
