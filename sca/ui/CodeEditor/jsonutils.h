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

    static QVariantList fromResultsJson(QJsonArray resultsJson);
    static BasicBlock* fromCFGJson(QJsonArray cfgJson);
    static QList<BasicBlock*> fromJson(QJsonArray jsonArray);
    static BasicBlock* fromJson(QJsonObject jsonObject);
};

#endif // JSONUTILS_H
