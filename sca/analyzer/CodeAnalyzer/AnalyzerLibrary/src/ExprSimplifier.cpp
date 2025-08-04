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

Status ExprSimplifier::populateDefinitions(const Variable* lhs, Expr* value) {
    if(lhs->getExprType() == ExprType::POINTERVARIABLE) {
        switch(value->getExprType()) {
            case ExprType::MALLOCFNCALL: {
                MallocFnCall* mallocFnCall = static_cast<MallocFnCall*>(value);
                Definition* definition = mallocFnCall->toSimplifyDefinition();
                const PointerVariable* lhsVar = nullptr;
                definition->addPointsToDefinition(new Definition(true));
                while(true) {
                    lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                    if(lhsVar == nullptr) break;
                    lhs = lhsVar->getPointsTo();
                    assert(lhs);
                    definition->addPointsToDefinition(new Definition(false));
                }
            }
            break;
            //pointervar
            case ExprType::DEFINITION: {
                Definition* definition = static_cast<Definition*>(value);
                const PointerVariable* lhsVar = nullptr;
                while(true) {
                    Definition* pointsToDefinition = new Definition(false);
                    definition->addPointsToDefinition(pointsToDefinition);
                    lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                    if(lhsVar == nullptr) {
                        assert(lhs);
                        if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
                            populateDefinitions(lhs, pointsToDefinition);
                        }
                        break;
                    }
                    lhs = lhsVar->getPointsTo();
                    assert(lhs);
                }
            }
            break;
            //array
            case ExprType::ARRAYDEFINITION: {
                Definition* definition = static_cast<Definition*>(value);
                const PointerVariable* lhsVar = nullptr;
                while(true) {
                    definition->addPointsToDefinition(new Definition(true));
                    lhsVar = dynamic_cast<const PointerVariable*>(lhs);
                    if(lhsVar == nullptr) break;
                    lhs = lhsVar->getPointsTo();
                    assert(lhs);
                }
        }
        break;
            default:;
        }
    }
    else if(lhs->getExprType() == ExprType::STRUCTVARIABLE) {
        assert(value);
        //cout <<"Expression simplification struct variable  " <<lhs->getName() <<endl;
        switch(value->getExprType()) {
            case ExprType::DEFINITION: {
                Definition* definition = static_cast<Definition*>(value);

                const StructVariable* structVariable = static_cast<const StructVariable*>(lhs);
                vector<const Variable*> memVars = structVariable->getMemVars();
                for(const Variable* memVar : memVars) {
                    Definition* memDefinition = new Definition(false);
                    if(memVar->getExprType() == ExprType::STRUCTVARIABLE ||
                            memVar->getExprType() == ExprType::POINTERVARIABLE) {
                        populateDefinitions(memVar, memDefinition);
                    }
                    definition->addPointsToDefinition(memDefinition);
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
            cout << "Assign Stmt: " <<*stmt <<endl;
            //cout << " no of substatements " <<block->getSubStatements().size() <<endl;
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
                    cout <<"Function call stmt inserted for Name "<<functionCallExpr->getName() << " Name ExprType " <<
                           functionCallExpr->getName()->getExprType() <<endl;
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }

                //Populate definitions for declarations
                const IdentifierName* identifierName = assignStmt->getVar();
                if(identifierName) {
                    const Variable* lhs =  block->getSymbolTable()->fetchVariable(identifierName->getName());
                    assert(lhs);
                    populateDefinitions(lhs, value);
                }
            }
            // append rhs to function pointer var possible pointsTo function Identifiers
            const Variable* lhs = nullptr;
            const Variable* rhs = nullptr;
            const IdentifierName* identifierName = assignStmt->getVar();
            if(identifierName) {
                if(identifierName->getType() != DeclType::FUNCTIONDECL) continue;
                lhs = block->getSymbolTable()->fetchVariable(identifierName->getName());
            }
            vector<const Expr*> fnIdentifiers;
            if(value) {
                if(!lhs) {
                    vector<const Expr*> lhsVars;
                    value->getLHS(lhsVars);
                    if(lhsVars.empty()) continue;

                    const Identifier* identifier = static_cast<const Identifier*>(lhsVars[0]);
                    assert(identifier);
                    cout <<"Identifier name, associated variable " <<identifier->getName() <<" " <<identifier->getVariable() <<endl;
                    lhs = identifier->getVariable();
                    //lhs can hold Dotoperator which has no associated variable
                    if(!lhs || lhs->getVarType() != VarType::FUNCTION) continue;
                }
                value->getRHSVariables(fnIdentifiers);
                if(fnIdentifiers.empty()) {  cout <<"Bad function name assigned to function pointer " <<identifierName->getName() <<endl; continue; }
                assert(fnIdentifiers[0]);
                rhs = static_cast<const Variable*>(fnIdentifiers[0]);
            }
            else continue;
            assert(lhs->getExprType() == ExprType::POINTERVARIABLE);
            const PointerVariable* pointerVariable = static_cast<const PointerVariable*>(lhs);
            lhs = pointerVariable->getPointsTo();
            const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(rhs);
            if(rhsPointer != nullptr) {
                rhs = rhsPointer->getPointsTo();
            }
            const FunctionVariable* functionVariable = dynamic_cast<const FunctionVariable*>(lhs);
            if(functionVariable == nullptr) {
                cout <<"Function pointer has no valid function Variable " <<identifierName->getName() <<endl;
                continue;
            }
            assert(rhs);
            functionVariable->addFunction(rhs);
        }
        break;
        case IF:
        case ELSE: {
            IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
            cout << "If Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            Expr* condition = ifStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(block->getSymbolTable());
            }
            status = simplifyBlock(ifStmt->getBlock());

            const IfStmt* elseStmt = ifStmt->getElse();
            if(elseStmt) {
                cout << "Else Stmt: no of substatements " << " " <<block->getSubStatements().size() <<endl;
                status = simplifyBlock(elseStmt->getBlock());
            }
        }
        break;
        case WHILE: {
            WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
            cout << "While Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
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
            cout << "For Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
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
            cout << "Func Decl Stmt: no of substatements " <<block->getSubStatements().size() <<endl;
            status = simplifyBlock(functionDeclStmt->getBlock());
        }
        break;
        default: {
            cout <<"error " <<endl;
        }
        break;
        }
    }
    return status;
}
