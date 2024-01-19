#include "cemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "jsonutils.h"
#include "visitor.h"

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
    //p_socketClient.writeToSocket("run");
    p_socketClient.writeToSocket("getCFG");

    /*QByteArray ba;
    QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    ba = file.readAll();
    file.close();
    qDebug() <<ba;

    const QJsonDocument results = QJsonDocument::fromJson(ba);
    BasicBlock* head = JsonUtils::fromCFGJson(results);
    PrintVisitor printVisitor;
    printVisitor.visitCFG(head);

    delete head;*/
}

void CEModel::onResultsAvailable(QJsonDocument resultsDoc)
{
    QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << resultsDoc.toJson();
    file.close();
    //QVariantList results = fromJson(resultsDoc);
    //emit resultsAvailable(results);
}
