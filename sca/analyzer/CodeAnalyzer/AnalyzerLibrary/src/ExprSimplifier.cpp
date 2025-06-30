#include "ExprSimplifier.h"
#include "Stmt.h"

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
            cout << "Assign Stmt: " <<*stmt << " " <<block->getSubStatements().size() <<endl;
            Expr* value = assignStmt->toSimplify();
            if(value) {
                value->populateVariable(block->getSymbolTable());
                vector<Expr*> DerefIdentifiers;
                value->getDerefIdentifiers(DerefIdentifiers);
                for(int i=0; i<DerefIdentifiers.size(); i++) {
                    DerefIdentifiers[i]->populateDerefVariable(block->getSymbolTable());
                }
                vector<const Expr*> functionCalls;
                value->getFunctionCalls(functionCalls);
                for(int i=0; i<functionCalls.size(); i++) {
                    const FunctionCall* functionCallExpr = static_cast<const FunctionCall*>(functionCalls[i]);
                    FunctionCallStmt* functionCallStmt = new FunctionCallStmt(FUNC_CALL, functionCallExpr->getName(),
                                                                              functionCallExpr->getArguments());
                    it = stmtList.insert(it, functionCallStmt);
                    it++;
                }
            }
        }
        break;
        case IF:
        case ELSE: {
            IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
            cout << "If Stmt: " <<block->getSubStatements().size() <<endl;
            Expr* condition = ifStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(block->getSymbolTable());
            }
            status = simplifyBlock(ifStmt->getBlock());

            const IfStmt* elseStmt = ifStmt->getElse();
            if(elseStmt) {
                cout << "Else Stmt: " << " " <<block->getSubStatements().size() <<endl;
                status = simplifyBlock(elseStmt->getBlock());
            }
        }
        break;
        case WHILE: {
            WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
            cout << "While Stmt: " <<block->getSubStatements().size() <<endl;
            Expr* condition = whileStmt->toSimplifyCondition();
            if(condition) {
                condition->populateVariable(block->getSymbolTable());
            }
            status = simplifyBlock(whileStmt->getBlock());
        }
        break;
        case FUNC_DECL: {
            FunctionDeclStmt* functionDeclStmt = static_cast<FunctionDeclStmt*>(stmt);
            cout << "Func Decl Stmt: " <<block->getSubStatements().size() <<endl;
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
