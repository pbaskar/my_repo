#include "jsonutils.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "jsonvisitor.h"

JsonUtils::JsonUtils()
{

}

QJsonArray JsonUtils::toJson(const std::vector<Result>& results) {
    QJsonArray resultArray;
    for(Result r : results) {
        QJsonObject resultObject;
        resultObject["errorMessage"] = r.errorMessage;
        resultArray.append(resultObject);
    }
    return resultArray;
}

QJsonArray JsonUtils::toJson(BasicBlock* cfgHead) {
    QJsonArray cfgArray;
    /*for(Result r : results) {
        QJsonObject resultObject;
        resultObject["errorMessage"] = r.errorMessage;
        resultArray.append(resultObject);
    }*/
    JsonVisitor jsonVisitor;
    jsonVisitor.visitCFG(cfgHead);
    cfgArray = jsonVisitor.getBlocks();
    return cfgArray;
}

void JsonUtils::fromJson(QByteArray resultsJson) {
    QJsonDocument resultsDoc = QJsonDocument::fromJson(resultsJson);
    QJsonArray resultsArray = resultsDoc.array();
    qDebug() <<Q_FUNC_INFO;
    for(const QJsonValue& value : resultsArray) {
        QJsonObject obj = value.toObject();
        qDebug() << "fromJson " <<obj["errorMessage"].toString();
    }
}

void JsonUtils::fromCFGJson(QByteArray cfgJson) {
    QJsonDocument cfgDoc = QJsonDocument::fromJson(cfgJson);
    QJsonArray cfgArray = cfgDoc.array();
    qDebug() <<Q_FUNC_INFO;
    fromJson(cfgArray);
}

void JsonUtils::fromJson(QJsonArray jsonArray) {
    for(const QJsonValue& value : jsonArray) {
        if(value.isArray()) {
            fromJson(value.toArray());
        }
        else if(value.isObject()) {
            fromJson(value.toObject());
        }
        else {
            qDebug() << "fromJson " <<value.toString();
        }
    }
}

void JsonUtils::fromJson(QJsonObject jsonObject) {
    for(const QJsonValue& value : jsonObject) {
        if(value.isArray()) {
            fromJson(value.toArray());
        }
        else {
            qDebug() << "fromJson " <<value.toString();
        }
    }
}
