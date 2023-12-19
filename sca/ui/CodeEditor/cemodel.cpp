#include "cemodel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

CEModel::CEModel()
{
    connect(&p_socketClient, &SocketClient::resultsAvailable, this, &CEModel::onResultsAvailable);
}

CEModel::~CEModel()
{
    qDebug() <<Q_FUNC_INFO;
}

void CEModel::sendCommand(QString command)
{
    p_socketClient.writeToSocket("run");
}

QVariantList fromJson(QJsonDocument resultsDoc)
{
    QVariantList results;
    if(resultsDoc.isArray())
    {
        QJsonArray resultsArray = resultsDoc.array();
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

void CEModel::onResultsAvailable(QJsonDocument resultsDoc)
{
    QVariantList results = fromJson(resultsDoc);
    emit resultsAvailable(results);
}
