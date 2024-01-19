#include "jsonutils.h"
#include <QJsonDocument>

JsonUtils::JsonUtils()
{

}

QVariantList JsonUtils::fromJson(const QJsonDocument resultsDoc)
{
    QVariantList results;
    if(resultsDoc.isArray())
    {
        const QJsonArray resultsArray = resultsDoc.array();
        for(const QJsonValue& value : resultsArray) {
            QJsonObject obj = value.toObject();
            qDebug() << Q_FUNC_INFO <<obj["errorMessage"].toString();
            results.append(obj.toVariantMap());
        }
    }
    else
    {
        qDebug() << "No message ";
    }
    return results;
}

BasicBlock* JsonUtils::fromCFGJson(const QJsonDocument cfgJson) {
    QJsonArray cfgArray = cfgJson.array();
    QList<BasicBlock*> cfg = fromJson(cfgArray);
    qDebug() <<Q_FUNC_INFO << cfg.size();
    return cfg.at(1);
}

QList<BasicBlock*> JsonUtils::fromJson(const QJsonArray jsonArray) {
    QList<BasicBlock*> blocks;
    for(const QJsonValue& value : jsonArray) {
        if(value.isArray()) {
            blocks.append(fromJson(value.toArray()));
        }
        else if(value.isObject()) {
            blocks.append(fromJson(value.toObject()));
        }
        else {
            qDebug() << "fromJson " <<value.toString();
        }
    }
    return blocks;
}

BasicBlock* JsonUtils::fromJson(const QJsonObject jsonObject) {
    QJsonObject::const_iterator it = jsonObject.constBegin();
    QString key = it.key();
    const QJsonValue value = it.value();
    if(key == "else") {
       qDebug() <<Q_FUNC_INFO <<"if";
       QList<BasicBlock*> ifBlocks;
       QList<BasicBlock*> elseBlocks;
       if(value.isArray()) {
           ifBlocks = fromJson(value.toArray());
       }
       it++;
       if(it.key() == "if") {
           const QJsonValue elseValue = it.value();
           if(elseValue.isArray()) {
               elseBlocks = fromJson(elseValue.toArray());
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
    if(key == "fnCall") {
        qDebug() <<Q_FUNC_INFO <<"fnCall";
        BasicBlock* args(0);
        FunctionDeclBlock* functionDeclBlock(0);
        if(value.isArray()) {
            args = fromJson(value[0].toObject());
            QJsonObject jsonObject = value[1].toObject();
            auto it = jsonObject.begin();
            QString key = it.key();
            const QJsonValue fnDecl = it.value();
            if(key == "fnDecl") {
                qDebug() <<Q_FUNC_INFO <<"fnDecl";
                QList<BasicBlock*> blocks;
                if(fnDecl.isArray()) {
                    blocks = fromJson(fnDecl.toArray());
                }
                functionDeclBlock  = new FunctionDeclBlock(blocks);
            }
        }
        FunctionCallBlock* functionCallBlock  = new FunctionCallBlock(args, functionDeclBlock);
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
    return 0;
}
