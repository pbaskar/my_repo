#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <QByteArray>
#include "analyzer.h"
#include "qjsonarray.h"

class JsonUtils
{
public:
    JsonUtils();

    static QJsonArray toJson(const std::vector<Result>& results);
    static QJsonArray toJson(BasicBlock* cfgHead);
    static void fromJson(QByteArray resultsJson);
    static void fromCFGJson(QByteArray cfgJson);
    static void fromJson(QJsonArray jsonArray);
    static void fromJson(QJsonObject jsonObject);
};

#endif // JSONUTILS_H
