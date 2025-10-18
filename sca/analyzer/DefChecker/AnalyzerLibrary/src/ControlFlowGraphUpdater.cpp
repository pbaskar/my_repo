#include "ControlFlowGraphUpdater.h"
#include "BasicBlock.h"

#include <cassert>

ControlFlowGraphUpdater::ControlFlowGraphUpdater() : p_loopBreakTargetBlock(0),
    p_loopContinueTargetBlock(0), p_returnTargetBlock(0)
{

}

ControlFlowGraphUpdater::~ControlFlowGraphUpdater() {

}

void ControlFlowGraphUpdater::visitBasicBlock(BasicBlock* basicBlock) {
    vector<Node*>& nodeList = basicBlock->toUpdateNodeList();
    for(Node* node: nodeList) {
        if(node->type() == NodeType::JUMPNODE) {
            JumpNode* jumpNode = static_cast<JumpNode*>(node);
            switch(jumpNode->getJumpType()) {
                case JumpType::BREAK: {
                    assert(p_loopBreakTargetBlock);
                    basicBlock->setNext(p_loopBreakTargetBlock);
                    Logger::getDebugStreamInstance() <<"Break setNext " << p_loopBreakTargetBlock <<endl;
                }
                break;
                case JumpType::CONTINUE: {
                    assert(p_loopContinueTargetBlock);
                    basicBlock->setNext(p_loopContinueTargetBlock);
                    Logger::getDebugStreamInstance() <<"Continue setNext " << p_loopContinueTargetBlock <<endl;
                }
                break;
                case JumpType::RETURN: {
                    //assert(p_returnTargetBlock);
                    if (p_returnTargetBlock) {
                        basicBlock->setNext(p_returnTargetBlock);
                    }
                    Logger::getDebugStreamInstance() << "Return setNext " << p_returnTargetBlock << endl;
                }
                break;
                default:;
            }
        }
    }
}

void ControlFlowGraphUpdater::visitIfElseBlock(IfElseBlock* ifElseBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of IfElseBlock: " <<endl <<endl;
    BasicBlock* block = ifElseBlock->getIfFirst();
    BasicBlock* lastBlock = ifElseBlock->getIfLast();
    BasicBlock* next(0);
    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    lastBlock->acceptVisitor(*this);

    if(ifElseBlock->getElseFirst()) {
        block = ifElseBlock->getElseFirst();
        lastBlock = ifElseBlock->getElseLast();
        while(block != lastBlock) {
            if(block->getType() == JUMPBLOCK) {
                Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
                break;
            }
            next = block->getNext();
            block->acceptVisitor(*this);
            block = next;
        }
        if(block) {
            block->acceptVisitor(*this);
        }
    }
    ifElseBlock->getLast()->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of IfElseBlock: " <<endl <<endl;
}

void ControlFlowGraphUpdater::visitWhileBlock(WhileBlock* whileBlock) {
    BasicBlock* loopBreakTargetBlock = p_loopBreakTargetBlock;
    BasicBlock* loopContinueTargetBlock = p_loopContinueTargetBlock;
    p_loopBreakTargetBlock = whileBlock->getNext();
    p_loopContinueTargetBlock = whileBlock->getFirst();

    Logger::getDebugStreamInstance() <<"Beginning of WhileBlock: " <<endl <<endl;
    BasicBlock* block = whileBlock->getFirst();
    BasicBlock* lastBlock = whileBlock->getLast();
    BasicBlock* next(0);

    //exit after condition
    assert(block);
    block->setNext(whileBlock->getNext());

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of WhileBlock: " <<endl <<endl;

    p_loopBreakTargetBlock = loopBreakTargetBlock;
    p_loopContinueTargetBlock = loopContinueTargetBlock;
}

void ControlFlowGraphUpdater::visitForBlock(ForBlock* forBlock) {
    BasicBlock* loopBreakTargetBlock = p_loopBreakTargetBlock;
    BasicBlock* loopContinueTargetBlock = p_loopContinueTargetBlock;
    p_loopBreakTargetBlock = forBlock->getNext();
    p_loopContinueTargetBlock = forBlock->getLast();

    Logger::getDebugStreamInstance() <<"Beginning of ForBlock: " <<endl <<endl;
    BasicBlock* block = forBlock->getFirst();
    BasicBlock* lastBlock = forBlock->getLast();
    BasicBlock* next(0);

    //exit after condition
    assert(block->getNext());
    block->getNext()->setNext(forBlock->getNext());

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of ForBlock: " <<endl <<endl;

    p_loopBreakTargetBlock = loopBreakTargetBlock;
    p_loopContinueTargetBlock = loopContinueTargetBlock;
}

void ControlFlowGraphUpdater::visitFunctionDeclBlock(FunctionDeclBlock* functionDeclBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of FunctionDeclBlock: " <<endl <<endl;
    BasicBlock* block = functionDeclBlock->getFirst();
    BasicBlock* lastBlock = functionDeclBlock->getLast();
    BasicBlock* next(0);

    while(block != lastBlock) {
        if(block->getType() == JUMPBLOCK) {
            Logger::getDebugStreamInstance() <<"Unreachable code following jump " <<endl;
            break;
        }
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    block->acceptVisitor(*this);
    Logger::getDebugStreamInstance() <<"End of FunctionDeclBlock: " <<endl <<endl;
}

void ControlFlowGraphUpdater::visitFunctionCallBlock(FunctionCallBlock* functionCallBlock) {
    Logger::getDebugStreamInstance() <<"Beginning of FunctionCallBlock: " <<endl <<endl;

    BasicBlock* returnTargetBlock = p_returnTargetBlock;
    p_returnTargetBlock = functionCallBlock->getNext();

    vector<FunctionCallInstance*>& functionCallInstances = functionCallBlock->getFnCallInstances();
    for(int i=0; i<functionCallInstances.size(); i++) {
        FunctionCallInstance* fnCallInstance = functionCallInstances[i];
        BasicBlock* block = fnCallInstance->getFirst();
        block->acceptVisitor(*this);

        block = fnCallInstance->getFnDecl();
        block->acceptVisitor(*this);

        block = fnCallInstance->getLast();
        block->acceptVisitor(*this);
    }
    p_returnTargetBlock = returnTargetBlock;
    Logger::getDebugStreamInstance() <<"End of FunctionCallBlock: " <<endl <<endl;
}

void ControlFlowGraphUpdater::visitCFG(BasicBlock* block) {
    Logger::getDebugStreamInstance() <<"Beginning of CFG: " <<endl <<endl;
    BasicBlock* next(0);
    while(block) {
        next = block->getNext();
        block->acceptVisitor(*this);
        block = next;
    }
    Logger::getDebugStreamInstance() <<"End of CFG: variableNodes size " <<endl <<endl;
}
