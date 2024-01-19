#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include "basicblock.h"

class JsonUtils
{
public:
    JsonUtils();

    static QVariantList fromJson(QJsonDocument resultsJson);
    static BasicBlock* fromCFGJson(QJsonDocument cfgJson);
    static QList<BasicBlock*> fromJson(QJsonArray jsonArray);
    static BasicBlock* fromJson(QJsonObject jsonObject);
};

#endif // JSONUTILS_H
