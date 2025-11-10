#include "cemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "jsonutils.h"
#include "visitor.h"
#include "positionblock.h"
#include "edgevisitor.h"

CEModel::CEModel() : p_head(0)
{
    //Start server process
    p_server.start("CodeAnalyzer.exe");
    //QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    //file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    //QTextStream text(&file);
    if (p_server.waitForStarted(90000)) {
        qDebug() << "process started " << Qt::endl;
        //text << "process started " << Qt::endl;
    }
    else {
        qDebug() << "process did not start " << Qt::endl;
        //text << "process did not start " << Qt::endl;
    }
    //file.close();

    //connect client to server
    p_socketClient.connectToServer();
    connect(&p_socketClient, &SocketClient::resultsAvailable, this, &CEModel::onResultsAvailable);
}

CEModel::~CEModel()
{
    qDebug() <<Q_FUNC_INFO;
}

void CEModel::disconnect() {
    p_socketClient.close();
    p_server.close();
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
    /*QFile file("C:\\workspace\\sca\\localStorage\\instructions.json");
    if(!file.open(QIODevice::Text | QIODevice::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << resultsDoc.toJson();
    file.close();*/

    QJsonObject jsonObject = resultsDoc.object();

    //QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    //file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    //QTextStream text(&file);
    if (jsonObject.empty()) {
        //text << "jsonobject empty " << Qt::endl;
        delete p_head;
        p_head = nullptr;
        emit CFGAvailable(QList<PositionBlock>());
        emit edgesAvailable(QList<Edge>());
        return;
    }
    QString command = jsonObject.keys().at(0);
    if(command == "run")
    {
        QJsonValue jsonValue = jsonObject[command];
        if (jsonValue == QJsonValue::Undefined || !jsonValue.isArray()) {
            QVariantList results;
            QJsonObject invalidObject;
            invalidObject["errorMessage"] = QString("Invalid result");
            results.append(invalidObject);
            emit resultsAvailable(results);
            return;
        }
        const QJsonArray jsonArray = jsonValue.toArray();
        QVariantList results = JsonUtils::fromResultsJson(jsonArray);
        emit resultsAvailable(results);
    }
    else if(command == "getCFG")
    {
        //text << Q_FUNC_INFO <<"getCFG result " << Qt::endl;
        QJsonValue jsonValue = jsonObject[command];
        if (jsonValue == QJsonValue::Undefined || !jsonValue.isArray()) {
            delete p_head;
            p_head = nullptr;
            emit CFGAvailable(QList<PositionBlock>());
            emit edgesAvailable(QList<Edge>());

            //text << Q_FUNC_INFO << "invalid result " << Qt::endl;
            return;
        }
        const QJsonArray jsonArray = jsonValue.toArray();
        BasicBlock* newHead = JsonUtils::fromCFGJson(jsonArray);
        Q_ASSERT(newHead != nullptr);

        PrintVisitor printVisitor;
        printVisitor.visitCFG(newHead);

        PositionVisitor positionVisitor;
        positionVisitor.visitCFG(newHead);

        EdgeVisitor edgeVisitor;
        edgeVisitor.setPositionBlocks(positionVisitor.getPositionBlocks());
        edgeVisitor.visitCFG(newHead);

        delete p_head;
        p_head = newHead;

        emit CFGAvailable(positionVisitor.getPositionBlocks());
        emit edgesAvailable(edgeVisitor.getEdges());
    }
    //file.close();
}
