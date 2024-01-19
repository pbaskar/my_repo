#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <QByteArray>
#include "AnalyzerLibrary/analyzer.h"
#include "qjsonarray.h"

class JsonUtils
{
public:
    JsonUtils();

    static QByteArray toJson(const std::vector<Result>& results);
    static QByteArray toJson(BasicBlock* cfgHead);
    static void fromJson(QByteArray resultsJson);
    static void fromCFGJson(QByteArray cfgJson);
    static void fromJson(QJsonArray jsonArray);
    static void fromJson(QJsonObject jsonObject);
};

#endif // JSONUTILS_H
