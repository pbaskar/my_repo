#ifndef BASICBLOCK_H
#define BASICBLOCK_H
#include <QStringList>
#include <QList>
#include <QDebug>

class Visitor;
class BasicBlock
{
public:
    BasicBlock();
    BasicBlock(QStringList stmts);
    virtual void print() const {
        for(const QString& stmt : p_stmts)
            qDebug() <<stmt;
    }
    virtual QString getTitle() const { return QString(); }
    virtual void acceptVisitor(Visitor& visitor) const;
    virtual ~BasicBlock() { qDebug() <<Q_FUNC_INFO;}
    const QStringList& getStmts() const { return p_stmts; }
private:
    QStringList p_stmts;
};

class IfElseBlock : public BasicBlock
{
public:
    IfElseBlock(QList<BasicBlock*> ifBlocks, QList<BasicBlock*> elseBlocks);
    virtual QString getTitle() const { return QString("IfElse :"); }
    virtual void acceptVisitor(Visitor& visitor) const;
    const QList<BasicBlock*>& getIfBlocks() const { return p_ifBlocks; }
    const QList<BasicBlock*>& getElseBlocks() const { return p_elseBlocks; }
    virtual ~IfElseBlock() {
        qDeleteAll(p_ifBlocks.begin(), p_ifBlocks.end());
        qDeleteAll(p_elseBlocks.begin(), p_elseBlocks.end());
        qDebug() <<Q_FUNC_INFO;
    }
private:
    QList<BasicBlock*> p_ifBlocks;
    QList<BasicBlock*> p_elseBlocks;
};

class WhileBlock : public BasicBlock
{
public:
    WhileBlock(QList<BasicBlock*> blocks);
    virtual QString getTitle() const { return QString("While :"); }
    virtual void acceptVisitor(Visitor& visitor) const;
    const QList<BasicBlock*>& getBlocks() const { return p_blocks; }
    virtual ~WhileBlock() { qDeleteAll(p_blocks.begin(), p_blocks.end()); qDebug() <<Q_FUNC_INFO;}
private:
    QList<BasicBlock*> p_blocks;
};

class ForBlock : public BasicBlock
{
public:
    ForBlock(QList<BasicBlock*> blocks);
    virtual QString getTitle() const { return QString("For :"); }
    virtual void acceptVisitor(Visitor& visitor) const;
    const QList<BasicBlock*>& getBlocks() const { return p_blocks; }
    virtual ~ForBlock() { qDeleteAll(p_blocks.begin(), p_blocks.end()); qDebug() << Q_FUNC_INFO; }
private:
    QList<BasicBlock*> p_blocks;
};

class FunctionDeclBlock : public BasicBlock
{
public:
    FunctionDeclBlock(QString name, QList<BasicBlock*> blocks);
    virtual QString getTitle() const {
        QString title("FunctionDecl : ");
        title.append(p_name);
        return title;
    }
    virtual void acceptVisitor(Visitor& visitor) const;
    const QList<BasicBlock*>& getBlocks() const { return p_blocks; }
    virtual ~FunctionDeclBlock() { qDeleteAll(p_blocks.begin(), p_blocks.end()); qDebug() <<Q_FUNC_INFO;}
private:
    QString p_name;
    QList<BasicBlock*> p_blocks;
};

class FunctionCallBlock : public BasicBlock
{
public:
    FunctionCallBlock(QString name, BasicBlock* args, FunctionDeclBlock* fnDecl);
    virtual QString getTitle() const {
        QString title("FunctionCall : ");
        title.append(p_name);
        return title;
    }
    virtual void acceptVisitor(Visitor& visitor) const;
    const BasicBlock* getArgs() const { return p_args; }
    const FunctionDeclBlock* getFnDecl() const { return p_fnDecl; }
    virtual ~FunctionCallBlock() { delete p_args; delete p_fnDecl; qDebug() <<Q_FUNC_INFO;}
private:
    QString p_name;
    BasicBlock* p_args;
    FunctionDeclBlock* p_fnDecl;
};

#endif // BASICBLOCK_H
