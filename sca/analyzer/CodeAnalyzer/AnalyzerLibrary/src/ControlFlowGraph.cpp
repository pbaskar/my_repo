/*
 * ControlFlowGraph.cpp
 *
 *  Created on: Sep 26, 2023
 *      Author: prbas_000
 */

#include "ControlFlowGraph.h"
#include "ComputeReachingDefsVisitor.h"

ControlFlowGraph::ControlFlowGraph(): p_head(0) {

}

ControlFlowGraph::~ControlFlowGraph() {
}

Status ControlFlowGraph::buildCFG(const Block* block) {
    Status status = SUCCESS;
    p_head = new BasicBlock(block->getSymbolTable());
    BasicBlock* currBlock = p_head;
    cout <<"head " <<currBlock <<endl;
    status = buildBlock(currBlock, block);
    return status;
}

Status ControlFlowGraph::buildBlock(BasicBlock*& currBlock, const Block* block) {
    Status status = SUCCESS;
    bool beginNewBlock = false;
    auto& stmtList = block->getSubStatements();
    for(Stmt* stmt : stmtList ) {
        StmtType type = stmt->getType();
        switch(type) {
        case DECL:
        case ASSIGN: {
            AssignStmt* assignStmt = static_cast<AssignStmt*>(stmt);
            Variable* var = nullptr;
            const IdentifierName* identifierName = assignStmt->getVar();
            if(identifierName)
                var = block->getSymbolTable()->fetchVariable(identifierName->getName());
            const Expr* value = assignStmt->getValue();
            AssignmentNode* newNode = new AssignmentNode(var, value);
            cout << "Assignment Node: " << *newNode << " " <<block->getSubStatements().size() <<endl;

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
            ConditionNode* ifNode = new ConditionNode(*ifStmt);
            cout << "If Node: " << *ifNode << " " <<block->getSubStatements().size() <<endl;

            first = new BasicBlock(ifStmt->getBlock()->getSymbolTable());
            first->addNode(ifNode);

            ifLast = first;
            status = buildBlock(ifLast, ifStmt->getBlock());

            const IfStmt* elseStmt = ifStmt->getElse();
            if(elseStmt) {
                ConditionNode* elseNode = new ConditionNode(*elseStmt);
                cout << "Else Node: " << *elseNode << " " <<block->getSubStatements().size() <<endl;

                elseFirst = new BasicBlock(elseStmt->getBlock()->getSymbolTable());
                elseFirst->addNode(elseNode);

                elseLast = elseFirst;
                status = buildBlock(elseLast, elseStmt->getBlock());
            }

            IfElseBlock* ifElseBlock = new IfElseBlock(0, first, ifLast, elseFirst, elseLast);
            currBlock->setNext(ifElseBlock);
            currBlock = ifElseBlock;
            beginNewBlock = true;
        }
        break;
        case WHILE: {
            BasicBlock* first(0);
            BasicBlock* last(0);

            WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
            ConditionNode* whileNode = new ConditionNode(*whileStmt);
            cout << "While Node: " << *whileNode << " " <<block->getSubStatements().size() <<endl;

            first = new BasicBlock(whileStmt->getBlock()->getSymbolTable());
            first->addNode(whileNode);

            last = first;
            status = buildBlock(last, whileStmt->getBlock());

            WhileBlock* whileBlock = new WhileBlock(0,first,last);
            whileBlock->setSelf();
            currBlock->setNext(whileBlock);
            currBlock = whileBlock;
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
            cout << "Function Decl Block: " << functionDeclBlock->getName() << " " <<block->getSubStatements().size() <<endl;
            block->getSymbolTable()->setFunctionDeclBlock(functionDeclBlock);
            beginNewBlock = true;
        }
        break;
        case FUNC_CALL: {
            BasicBlock* first(0);
            FunctionDeclBlock* fnDecl(0);

            FunctionCallStmt* functionCallStmt = static_cast<FunctionCallStmt*>(stmt);
            first = new BasicBlock(block->getSymbolTable());

            const Identifier* identifier = static_cast<const Identifier*>(functionCallStmt->getName());
            fnDecl = p_head->fetchFunctionDeclBlock(identifier->getName());

            first->setNext(fnDecl);
            FunctionCallBlock* functionCallBlock = new FunctionCallBlock(0,identifier,first,fnDecl);
            auto formalArguments = fnDecl->getFormalArguments();
            auto actualArguments = functionCallStmt->getActualArguments();
            int i=0;
            for(Expr* expr : actualArguments) {
                AssignmentNode* functionDeclNode = new AssignmentNode(formalArguments[i], expr);
                first->addNode(functionDeclNode);
                functionCallBlock->addActualArgument(expr);
                i++;
            }
            cout <<"Function Call Block: "  <<functionCallBlock->getName() << " " <<block->getSubStatements().size()<<endl;
            currBlock->setNext(functionCallBlock);
            currBlock = functionCallBlock;
            beginNewBlock = true;
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

void ControlFlowGraph::print(ostream& os) {
    PrintVisitor printVisitor;
    TraverserOne tOne(&printVisitor);
    tOne.traverseCFG(this->p_head);
}

void ControlFlowGraph::variableInitCheck(vector<Result>& results) {
    ComputeReachingDefsVisitor computeReachingDefsVisitor;
    computeReachingDefsVisitor.visitCFG(p_head);

    map<BasicBlock*, map<const Variable*, vector<AssignmentNode*>>> inVariableNodes = computeReachingDefsVisitor.getInVariableNodes();

    VariableInitCheckVisitor variableInitCheckVisitor(inVariableNodes);
    variableInitCheckVisitor.visitCFG(p_head);
    results = variableInitCheckVisitor.getResults();
}

void ControlFlowGraph::clear() {
    p_head->clearFnSymbolEntries();
    DeleteVisitor deleteVisitor;
    TraverserOne tOne(&deleteVisitor, true);
    tOne.traverseCFG(p_head);
}
