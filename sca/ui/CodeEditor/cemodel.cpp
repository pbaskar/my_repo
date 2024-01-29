#include "cemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "jsonutils.h"
#include "visitor.h"
#include "positionblock.h"

CEModel::CEModel()
{
    connect(&p_socketClient, &SocketClient::resultsAvailable, this, &CEModel::onResultsAvailable);
}

CEModel::~CEModel()
{
    qDebug() <<Q_FUNC_INFO;
}

CEModel* CEModel::getInstance()
{
    static CEModel instance;
    return &instance;
}

void CEModel::sendCommand(QString command)
{
    //p_socketClient.writeToSocket(command.toLocal8Bit());
    qDebug() <<Q_FUNC_INFO <<command;
    QByteArray ba;
    QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    ba = file.readAll();
    file.close();
    qDebug() <<ba;
    const QJsonDocument resultsDoc = QJsonDocument::fromJson(ba);
    QJsonObject jsonObject = resultsDoc.object();
    BasicBlock* head = JsonUtils::fromCFGJson(jsonObject[command].toArray());
    PrintVisitor printVisitor;
    printVisitor.visitCFG(head);

    PositionVisitor positionVisitor;
    positionVisitor.visitCFG(head);

    //delete head;
    emit CFGAvailable(positionVisitor.getPositionBlocks());
}

void CEModel::onResultsAvailable(QJsonDocument resultsDoc)
{
    QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << resultsDoc.toJson();
    file.close();

    QJsonObject jsonObject = resultsDoc.object();
    QString command = jsonObject.keys().at(0);
    if(command == "run")
    {
        QVariantList results = JsonUtils::fromResultsJson(jsonObject[command].toArray());
        emit resultsAvailable(results);
    }
    else if(command == "getCFG")
    {
        BasicBlock* head = JsonUtils::fromCFGJson(jsonObject[command].toArray());
        PrintVisitor printVisitor;
        printVisitor.visitCFG(head);

        PositionVisitor positionVisitor;
        positionVisitor.visitCFG(head);

        delete head;
        emit CFGAvailable(positionVisitor.getPositionBlocks());
    }
}
