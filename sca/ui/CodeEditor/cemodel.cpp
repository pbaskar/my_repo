#include "cemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "jsonutils.h"
#include "visitor.h"
#include "positionblock.h"
#include "edgevisitor.h"

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
    p_socketClient.writeToSocket(command.toLocal8Bit());
    qDebug() <<Q_FUNC_INFO <<command;
    /*QByteArray ba;
    QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    ba = file.readAll();
    file.close();
    qDebug() <<ba;
    const QJsonDocument resultsDoc = QJsonDocument::fromJson(ba);
    QJsonObject jsonObject = resultsDoc.object();
    QJsonValue jsonValue = jsonObject[command];
    if(jsonValue == QJsonValue::Undefined || !jsonValue.isArray())
        return;
    const QJsonArray jsonArray = jsonValue.toArray();
    BasicBlock* head = JsonUtils::fromCFGJson(jsonArray);
    Q_ASSERT(head != nullptr);

    PrintVisitor printVisitor;
    printVisitor.visitCFG(head);

    PositionVisitor positionVisitor;
    positionVisitor.visitCFG(head);

    EdgeVisitor edgeVisitor;
    edgeVisitor.setPositionBlocks(positionVisitor.getPositionBlocks());
    edgeVisitor.visitCFG(head);

    emit CFGAvailable(positionVisitor.getPositionBlocks());
    emit edgesAvailable(edgeVisitor.getEdges());*/

    //delete head;
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
    if(jsonObject.empty())
        return;
    QString command = jsonObject.keys().at(0);
    if(command == "run")
    {
        QJsonValue jsonValue = jsonObject[command];
        if(jsonValue == QJsonValue::Undefined || !jsonValue.isArray())
            return;
        const QJsonArray jsonArray = jsonValue.toArray();
        QVariantList results = JsonUtils::fromResultsJson(jsonArray);
        emit resultsAvailable(results);
    }
    else if(command == "getCFG")
    {
        //delete head;
        QJsonValue jsonValue = jsonObject[command];
        if(jsonValue == QJsonValue::Undefined || !jsonValue.isArray())
            return;
        const QJsonArray jsonArray = jsonValue.toArray();
        BasicBlock* head = JsonUtils::fromCFGJson(jsonArray);
        Q_ASSERT(head != nullptr);

        PrintVisitor printVisitor;
        printVisitor.visitCFG(head);

        PositionVisitor positionVisitor;
        positionVisitor.visitCFG(head);

        EdgeVisitor edgeVisitor;
        edgeVisitor.setPositionBlocks(positionVisitor.getPositionBlocks());
        edgeVisitor.visitCFG(head);

        emit CFGAvailable(positionVisitor.getPositionBlocks());
        emit edgesAvailable(edgeVisitor.getEdges());
    }
}
