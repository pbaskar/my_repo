/*
 * ControlFlowGraph.cpp
 *
 *  Created on: Sep 26, 2023
 *      Author: prbas_000
 */

#include "ControlFlowGraph.h"
#include "ComputeReachingDefsVisitor.h"
#include "ControlFlowGraphUpdater.h"

ControlFlowGraph::ControlFlowGraph(): p_head(0) {

}

ControlFlowGraph::~ControlFlowGraph() {
}

Status ControlFlowGraph::buildCFG(const Block* block) {
    Status status = SUCCESS;
    p_head = new BasicBlock(block->getSymbolTable());
    BasicBlock* currBlock = p_head;
    Logger::getDebugStreamInstance() <<"head " <<currBlock <<endl;
    status = buildBlock(currBlock, block);
    return status;
}

Status ControlFlowGraph::makeFunctionCallInstance(FunctionCallInstance*& fnCallInstance, FunctionCallStmt* functionCallStmt,
                                                       FunctionDeclBlock* fnDecl, const Block* block) {
    BasicBlock* first(0);
    BasicBlock* last(0);

    first = new BasicBlock(block->getSymbolTable());
    first->setNext(fnDecl);
    last = new BasicBlock(block->getSymbolTable());
    fnDecl->setNext(last);

    auto formalArguments = fnDecl->getFormalArguments();
    auto actualArguments = functionCallStmt->getActualArguments();
    int i=0;
    for(Expr* expr : actualArguments) {
        AssignmentNode* functionDeclNode = new AssignmentNode(formalArguments[i], expr);
        first->addNode(functionDeclNode);
        i++;
    }
    /*i=0;
    for(Expr* expr : actualArguments) {
        if(formalArguments[i]->getExprType() == ExprType::POINTERVARIABLE) {
            const Variable* var = dynamic_cast<Identifier*>(expr)->getVariable();
            if(var) {
                AssignmentNode* functionDeclNode = new AssignmentNode(var, formalArguments[i]);
                last->addNode(functionDeclNode);
            }
        }
        i++;
    }*/
    fnCallInstance = new FunctionCallInstance(fnDecl->getName(), first, fnDecl, last);
    i=0;
    for(Expr* expr : actualArguments) {
        fnCallInstance->addActualArgument(expr);
        i++;
    }
    return SUCCESS;
}

Status ControlFlowGraph::buildBlock(BasicBlock*& currBlock, const Block* block) {
    Status status = SUCCESS;
    bool beginNewBlock = false;
    auto& stmtList = block->getSubStatements();
    for(Stmt* stmt : stmtList ) {
        StmtType type = stmt->getType();
        switch(type) {
        case JUMP: {
            JumpStmt* jumpStmt = static_cast<JumpStmt*>(stmt);
            JumpNode* jumpNode = new JumpNode(jumpStmt->getJumpType());
            Logger::getDebugStreamInstance() <<" Jump Node: "<< jumpNode <<endl;
            if(beginNewBlock) {
                beginNewBlock = false;
                BasicBlock* newBlock = new BasicBlock(block->getSymbolTable());
                currBlock->setNext(newBlock);
                currBlock = newBlock;
            }
            currBlock->addNode(jumpNode);
            currBlock->setType(BlockType::JUMPBLOCK);
        }
        break;
        case DECL:
        case ASSIGN: {
            AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
            Variable* var = nullptr;
            const IdentifierName* identifierName = assignStmt->getVar();
            if(identifierName)
                var = block->getSymbolTable()->fetchVariable(identifierName->getName());
            const Expr* value = assignStmt->getValue();
            const int lineNum = assignStmt->getLineNum();
            AssignmentNode* newNode = new AssignmentNode(var, value, lineNum);
            Logger::getDebugStreamInstance() << "Assignment Node: " << *newNode <<endl;
            //Logger::getDebugStreamInstance() <<block->getSubStatements().size() <<endl;

            if(beginNewBlock) {
                beginNewBlock = false;
                BasicBlock* newBlock = new BasicBlock(block->getSymbolTable());
                currBlock->setNext(newBlock);
                currBlock = newBlock;
            }
            currBlock->addNode(newNode);
        }
        break;
        case IF:
        case ELSE: {
            BasicBlock* first(0);
            BasicBlock* ifLast(0);
            BasicBlock* elseFirst(0);
            BasicBlock* elseLast(0);

            IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
            ConditionNode* ifNode = new ConditionNode(ifStmt->getCondition());
            Logger::getDebugStreamInstance() << "If Node: " << *ifNode << " " <<block->getSubStatements().size() <<endl;

            first = new BasicBlock(ifStmt->getBlock()->getSymbolTable());
            first->addNode(ifNode);

            ifLast = first;
            status = buildBlock(ifLast, ifStmt->getBlock());

            const IfStmt* elseStmt = ifStmt->getElse();
            if(elseStmt) {
                ConditionNode* elseNode = new ConditionNode(elseStmt->getCondition());
                Logger::getDebugStreamInstance() << "Else Node: " << *elseNode << " " <<block->getSubStatements().size() <<endl;

                elseFirst = new BasicBlock(elseStmt->getBlock()->getSymbolTable());
                elseFirst->addNode(elseNode);

                elseLast = elseFirst;
                status = buildBlock(elseLast, elseStmt->getBlock());
            }

            IfElseBlock* ifElseBlock = new IfElseBlock(0, first, ifLast, elseFirst, elseLast);
            if(ifLast->getType() != BlockType::JUMPBLOCK)
                ifLast->setNext(ifElseBlock->getLast());
            if(elseLast && elseLast->getType() != BlockType::JUMPBLOCK) {
                elseLast->setNext(ifElseBlock->getLast());
            }
            currBlock->setNext(ifElseBlock);
            currBlock = ifElseBlock;
            beginNewBlock = true;
        }
        break;
        case WHILE: {
            BasicBlock* first(0);
            BasicBlock* last(0);

            WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
            ConditionNode* whileNode = new ConditionNode(whileStmt->getCondition());
            Logger::getDebugStreamInstance() << "While Node: " << *whileNode << " " <<block->getSubStatements().size() <<endl;

            first = new BasicBlock(whileStmt->getBlock()->getSymbolTable());
            first->addNode(whileNode);

            last = new BasicBlock(whileStmt->getBlock()->getSymbolTable());
            first->setNext(last);
            status = buildBlock(last, whileStmt->getBlock());

            WhileBlock* whileBlock = new WhileBlock(0,first,last);
            whileBlock->setSelf();
            currBlock->setNext(whileBlock);
            currBlock = whileBlock;
            beginNewBlock = true;
        }
        break;
        case FOR: {
            BasicBlock* first(0);
            BasicBlock* last(0);

            ForStmt* forStmt = static_cast<ForStmt*>(stmt);
            const AssignStmt* initStmt = forStmt->getInitStmt();
            AssignmentNode* initNode = nullptr;
            first = new BasicBlock(forStmt->getBlock()->getSymbolTable());
            if(initStmt) {
                Variable* var = nullptr;
                const IdentifierName* identifierName = initStmt->getVar();
                if(identifierName)
                    var = forStmt->getBlock()->getSymbolTable()->fetchVariable(identifierName->getName());
                const Expr* value = initStmt->getValue();
                AssignmentNode* initNode = new AssignmentNode(var, value);
                first->addNode(initNode);
                Logger::getDebugStreamInstance() << "For:init " << initNode << " " <<block->getSubStatements().size() <<endl;
            }

            const Expr* forCondition = forStmt->getCondition();
            BasicBlock* condition = new BasicBlock(forStmt->getBlock()->getSymbolTable());
            first->setNext(condition);
            last = condition;
            if(forCondition) {
                AssignmentNode* conditionNode = new AssignmentNode(nullptr, forCondition);
                Logger::getDebugStreamInstance() << "For:condition " << conditionNode << " " <<block->getSubStatements().size() <<endl;
                condition->addNode(conditionNode);
            }

            status = buildBlock(last, forStmt->getBlock());

            BasicBlock* forPostBlock = new BasicBlock(forStmt->getBlock()->getSymbolTable());
            const Expr* forPostExpr = forStmt->getPostExpr();
            last->setNext(forPostBlock);
            last = forPostBlock;
            if(forPostExpr) {
                AssignmentNode* postExprNode = new AssignmentNode(nullptr, forPostExpr);
                Logger::getDebugStreamInstance() << "For:postExpr " << postExprNode << " " <<block->getSubStatements().size() <<endl;
                forPostBlock->addNode(postExprNode);
            }

            ForBlock* forBlock = new ForBlock(0,first,last);
            forBlock->setSelf();
            currBlock->setNext(forBlock);
            currBlock = forBlock;
            beginNewBlock = true;
        }
        break;
        case FUNC_DECL: {
            BasicBlock* first(0);
            BasicBlock* last(0);

            FunctionDeclStmt* functionDeclStmt = static_cast<FunctionDeclStmt*>(stmt);

            first = new BasicBlock(functionDeclStmt->getBlock()->getSymbolTable());

            last = first;
            status = buildBlock(last, functionDeclStmt->getBlock());

            FunctionIdentifierName* functionIdentifierName = static_cast<FunctionIdentifierName*>(functionDeclStmt->getName());
            FunctionDeclBlock* functionDeclBlock = new FunctionDeclBlock(0,functionIdentifierName->getName(),first,last);

            vector<IdentifierName*> formalArguments = functionIdentifierName->getParameterList();

            for(IdentifierName* identifierName : formalArguments) {
                Variable* var = functionDeclStmt->getBlock()->getSymbolTable()->fetchVariable(identifierName->getName());
                functionDeclBlock->addFormalArgument(var);
            }
            Logger::getDebugStreamInstance() << "Function Decl Block: " << functionDeclBlock->getName() << " " <<block->getSubStatements().size() <<endl;
            block->getSymbolTable()->setFunctionDeclBlock(functionDeclBlock);
            beginNewBlock = true;
        }
        break;
        case FUNC_CALL: {
            FunctionDeclBlock* fnDecl(0);
            FunctionCallInstance* functionCallInstance(0);
            FunctionCallBlock* functionCallBlock(0);
            FunctionCallStmt* functionCallStmt = static_cast<FunctionCallStmt*>(stmt);
            const Expr* expr = functionCallStmt->getName();
            const Identifier* identifier = static_cast<const Identifier*>(expr);

            if(identifier->getExprType() == ExprType::DEREFERENCEOPERATOR){
                const Variable* var = identifier->getVariable();
                const FunctionVariable* fnVar = dynamic_cast<const FunctionVariable*>(var);
                if(fnVar) {
                    const vector<const Expr*>& fnIdentifiers = fnVar->getFunctionIdentifiers();
                    if(fnIdentifiers.empty()) {
                        Logger::getDebugStreamInstance() << "Function pointer has no function assigned " <<identifier->getName() <<endl;
                        continue;
                    }
                    functionCallBlock = new FunctionCallBlock(0,identifier);
                    for(int i=0; i<fnIdentifiers.size(); i++) {
                        const Variable* identifier = static_cast<const Variable*>(fnIdentifiers[i]);
                        fnDecl = p_head->fetchFunctionDeclBlock(identifier->getName());
                        if(fnDecl == nullptr) { Logger::getDebugStreamInstance() <<"function declaration block not found " <<identifier->getName() << endl; return FAILURE; }
                        makeFunctionCallInstance(functionCallInstance, functionCallStmt, fnDecl, block);
                        functionCallBlock->addInstance(functionCallInstance);
                    }
                }
                else {
                    Logger::getDebugStreamInstance() <<"CFG: function pointer dereference does not contain function variable " <<*expr <<endl;
                }
            }
            else {
                fnDecl = p_head->fetchFunctionDeclBlock(identifier->getName());
                if(fnDecl == nullptr) { Logger::getDebugStreamInstance() <<"function declaration block not found " <<endl; return FAILURE; }
                makeFunctionCallInstance(functionCallInstance, functionCallStmt, fnDecl, block);
                functionCallBlock = new FunctionCallBlock(0,identifier);
                functionCallBlock->addInstance(functionCallInstance);
            }
            currBlock->setNext(functionCallBlock);
            currBlock = functionCallBlock;
            beginNewBlock = true;
            Logger::getDebugStreamInstance() <<"Function Call Block: "  <<identifier->getName() << " " <<block->getSubStatements().size()<<endl;
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

void ControlFlowGraph::print(ostream& os) {
    PrintVisitor printVisitor;
    TraverserOne tOne(&printVisitor);
    tOne.traverseCFG(this->p_head);
}

void ControlFlowGraph::variableInitCheck(vector<Result>& results) {
    ControlFlowGraphUpdater controlFlowGraphUpdater;
    controlFlowGraphUpdater.visitCFG(p_head);

    ComputeReachingDefsVisitor computeReachingDefsVisitor;
    computeReachingDefsVisitor.visitCFG(p_head);

    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> inVariableNodes = computeReachingDefsVisitor.getInVariableNodes();
    map<BasicBlock*, map<const Variable*, vector<pair<const Definition*, bool>>>> inDefinitions = computeReachingDefsVisitor.getInDefinitions();
    map<BasicBlock*, map<const Definition*, vector<vector<const Variable*>>>> inVariableGroups = computeReachingDefsVisitor.getInVariableGroups();
    Logger::getDebugStreamInstance() << "********************************** Compute Reaching Defs done ****************************************" <<endl;
    VariableInitCheckVisitor variableInitCheckVisitor(inVariableNodes, inDefinitions, inVariableGroups);
    variableInitCheckVisitor.visitCFG(p_head);
    results = variableInitCheckVisitor.getResults();
}

void ControlFlowGraph::clear() {
    p_head->clearFnSymbolEntries();
    DeleteVisitor deleteVisitor;
    TraverserOne tOne(&deleteVisitor, true);
    tOne.traverseCFG(p_head);
}
