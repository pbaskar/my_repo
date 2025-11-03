#include "jsonutils.h"
#include <QJsonDocument>

const QVariantList JsonUtils::fromResultsJson(const QJsonArray resultsJson)
{
    QVariantList results;
    for(const QJsonValue& value : resultsJson) {
        QJsonObject obj = value.toObject();
        qDebug() << Q_FUNC_INFO <<obj["errorMessage"].toString();
        results.append(obj.toVariantMap());
    }
    return results;
}

BasicBlock* JsonUtils::fromCFGJson(const QJsonArray cfgJson) {
    const QList<BasicBlock*> cfg = fromJson(cfgJson);
    qDebug() <<Q_FUNC_INFO << cfg.size();
    return cfg.at(1);
}

const QList<BasicBlock*> JsonUtils::fromJson(const QJsonArray jsonArray) {
    QList<BasicBlock*> blocks;
    for(const QJsonValue& value : jsonArray) {
        if(value.isArray()) {
            blocks.append(fromJson(value.toArray()));
        }
        else if(value.isObject()) {
            BasicBlock* blockPtr = fromJson(value.toObject());
            if(blockPtr != nullptr)
                blocks.append(blockPtr);
        }
        else {
            qDebug() << "fromJson " <<value.toString();
        }
    }
    return blocks;
}

BasicBlock* JsonUtils::fromJson(const QJsonObject jsonObject) {
    if(jsonObject.empty())
        return nullptr;
    QJsonObject::const_iterator it = jsonObject.constBegin();
    QString key = it.key();
    const QJsonValue value = it.value();
    if(key == "else") {
       qDebug() <<Q_FUNC_INFO <<"if";
       QList<BasicBlock*> ifBlocks;
       QList<BasicBlock*> elseBlocks;
       if(value.isArray()) {
           elseBlocks = fromJson(value.toArray());
       }
       it++;
       if(it != jsonObject.end() && it.key() == "if") {
           const QJsonValue elseValue = it.value();
           if(elseValue.isArray()) {
               ifBlocks = fromJson(elseValue.toArray());
           }
       }
       IfElseBlock* ifElseBlock  = new IfElseBlock(ifBlocks, elseBlocks);
       return ifElseBlock;
    }
    if(key == "while") {
         qDebug() <<Q_FUNC_INFO <<"while";
        QList<BasicBlock*> blocks;
        if(value.isArray()) {
            blocks = fromJson(value.toArray());
        }
        WhileBlock* whileBlock  = new WhileBlock(blocks);
        return whileBlock;
    }
    if (key == "for") {
        qDebug() << Q_FUNC_INFO << "for";
        QList<BasicBlock*> blocks;
        if (value.isArray()) {
            blocks = fromJson(value.toArray());
        }
        ForBlock* forBlock = new ForBlock(blocks);
        return forBlock;
    }
    if(key == "fnCall") {
        qDebug() <<Q_FUNC_INFO <<"fnCall";
        QString fname;
        BasicBlock* args(0);
        FunctionDeclBlock* functionDeclBlock(0);
        if(value.isArray()) {
            const QJsonArray fnCall = value.toArray();
            if(fnCall.count() < 3)
                return nullptr;
            fname = fnCall[0].toObject()["name"].toString();
            args = fromJson(fnCall[1].toObject());
            QJsonObject jsonObject = fnCall[2].toObject();
            auto it = jsonObject.constBegin();
            QString key = it.key();
            const QJsonValue fnDecl = it.value();
            if(key == "fnDecl") {
                qDebug() <<Q_FUNC_INFO <<"fnDecl";
                QString fnDeclName;
                QList<BasicBlock*> blocks;
                if(fnDecl.isArray()) {
                    const QJsonArray fnDeclArray = fnDecl.toArray();
                    if (fnDeclArray.count() < 2) {
                        return nullptr;
                    }
                    fnDeclName = fnDeclArray[0].toObject()["name"].toString();
                    blocks = fromJson(fnDeclArray[1].toArray());
                }
                functionDeclBlock  = new FunctionDeclBlock(fnDeclName, blocks);
            }
        }
        if(functionDeclBlock == nullptr) {
            delete args;
            delete functionDeclBlock;
            return nullptr;
        }
        FunctionCallBlock* functionCallBlock  = new FunctionCallBlock(fname, args, functionDeclBlock);
        return functionCallBlock;
    }
    if(key == "basic") {
        qDebug() <<Q_FUNC_INFO <<"basic";
        QStringList stmts;
        if(value.isArray()) {
            const QJsonArray jsonArray = value.toArray();
            for(const QJsonValue& stmt : jsonArray) {
                stmts.append(stmt.toString());
            }
        }
        BasicBlock* basicBlock = new BasicBlock(stmts);
        return basicBlock;
    }
    return nullptr;
}
