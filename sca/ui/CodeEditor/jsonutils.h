#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include "basicblock.h"

class JsonUtils
{
public:
    static const QVariantList fromResultsJson(const QJsonArray resultsJson);
    static BasicBlock* fromCFGJson(const QJsonArray cfgJson);
    static const QList<BasicBlock*> fromJson(const QJsonArray jsonArray);
    static BasicBlock* fromJson(const QJsonObject jsonObject);
};

#endif // JSONUTILS_H
