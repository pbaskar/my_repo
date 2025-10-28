#include "ExprSimplifier.h"
#include "Stmt.h"
#include "Utils.h"
#include <cassert>

ExprSimplifier::ExprSimplifier()
{

}

Status ExprSimplifier::simplify(Block* block) {
    Status status = SUCCESS;
    status = simplifyBlock(block);
    return status;
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
                    Utils::populateDefinitions(lhs, value);
                }

                // append rhs to function pointer var possible pointsTo function Identifiers
                const Variable* rhs = nullptr;
                if(identifierName) {
                    if(identifierName->getType() != DeclType::FUNCTIONDECL) continue;
                }
                vector<const Expr*> fnIdentifiers;
                if(!lhs || lhs->getVarType() != VarType::FUNCTION) continue;
                value->getRHSVariables(fnIdentifiers);
                if(fnIdentifiers.empty()) {  Logger::getDebugStreamInstance() <<"Bad function name assigned to function pointer " <<lhs->getName() <<endl; continue; }
                assert(fnIdentifiers[0]);
                rhs = static_cast<const Variable*>(fnIdentifiers[0]);

                assert(lhs->getExprType() == ExprType::POINTERVARIABLE);
                const PointerVariable* pointerVariable = static_cast<const PointerVariable*>(lhs);
                const Variable* pointsToLHS = pointerVariable->getPointsTo();
                const PointerVariable* rhsPointer=dynamic_cast<const PointerVariable*>(rhs);
                //&function name on rhs or directly function name
                if(rhsPointer != nullptr) {
                    rhs = rhsPointer->getPointsTo();
                }
                const FunctionVariable* functionVariable = dynamic_cast<const FunctionVariable*>(pointsToLHS);
                if(functionVariable == nullptr) {
                    Logger::getDebugStreamInstance() <<"Function pointer has no valid function Variable " << lhs->getName() <<endl;
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
                condition->populateDerefVariable(block->getSymbolTable());
                vector<const Expr*> functionCalls;
                condition->getFunctionCalls(functionCalls);
                for (int i = 0; i < functionCalls.size(); i++) {
                    const FunctionCall* functionCallExpr = static_cast<const FunctionCall*>(functionCalls[i]);
                    FunctionCallStmt* functionCallStmt = new FunctionCallStmt(FUNC_CALL, functionCallExpr->getName(),
                        functionCallExpr->getArguments());
                    Logger::getDebugStreamInstance() << "Function call stmt inserted for Name " << functionCallExpr->getName() << " Name ExprType " <<
                        functionCallExpr->getName()->getExprType() << endl;
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }
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
                condition->populateDerefVariable(whileSymbolTable);
                vector<const Expr*> functionCalls;
                condition->getFunctionCalls(functionCalls);
                for (int i = 0; i < functionCalls.size(); i++) {
                    const FunctionCall* functionCallExpr = static_cast<const FunctionCall*>(functionCalls[i]);
                    FunctionCallStmt* functionCallStmt = new FunctionCallStmt(FUNC_CALL, functionCallExpr->getName(),
                        functionCallExpr->getArguments());
                    Logger::getDebugStreamInstance() << "Function call stmt inserted for Name " << functionCallExpr->getName() << " Name ExprType " <<
                        functionCallExpr->getName()->getExprType() << endl;
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }
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
                initExpr->populateDerefVariable(forSymbolTable);
            }
            Expr* condition = forStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(forSymbolTable);
                condition->populateDerefVariable(forSymbolTable);
                vector<const Expr*> functionCalls;
                condition->getFunctionCalls(functionCalls);
                for (int i = 0; i < functionCalls.size(); i++) {
                    const FunctionCall* functionCallExpr = static_cast<const FunctionCall*>(functionCalls[i]);
                    FunctionCallStmt* functionCallStmt = new FunctionCallStmt(FUNC_CALL, functionCallExpr->getName(),
                        functionCallExpr->getArguments());
                    Logger::getDebugStreamInstance() << "Function call stmt inserted for Name " << functionCallExpr->getName() << " Name ExprType " <<
                        functionCallExpr->getName()->getExprType() << endl;
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }
            }
            Expr* postExpr = forStmt->toSimplifyPostExpr();
            if(postExpr) {
                postExpr->populateVariable(forSymbolTable);
                postExpr->populateDerefVariable(forSymbolTable);
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
