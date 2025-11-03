#include "socketserver.h"
#include<QDebug>
#include "analyzer.h"
#include "jsonutils.h"

extern char* gFileName;
SocketServer::SocketServer() : p_clientSocket(0)
{
    //connect(&p_timer, &QTimer::timeout, this, &SocketServer::timerTriggered);
    connect(this, &SocketServer::newConnection, this, &SocketServer::onNewConnection);
    listen(QHostAddress::LocalHost, 1234);
}

SocketServer::~SocketServer()
{
}

void SocketServer::onNewConnection()
{
    qDebug() << Q_FUNC_INFO <<"Incoming Connection";
    if(hasPendingConnections())
    {
        p_clientSocket = nextPendingConnection();
    }
    connect(p_clientSocket, &QTcpSocket::disconnected, this, &SocketServer::clientDisconnected);
    connect(p_clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readData);
}

void SocketServer::readData()
{
    QByteArray clientCommand = p_clientSocket->readAll();
    qDebug() <<Q_FUNC_INFO<< clientCommand;
    /*QFile file("C:\\workspace\\my_repo\\sca\\test\\server.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "SocketServer::readData" << clientCommand <<Qt::endl;
    file.close();*/

    QList<QByteArray> clientCommandList = clientCommand.split(' ');
    if(clientCommandList.empty()) qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand;
    QByteArray command = clientCommandList[0];

    if(command == "run") {
        if (clientCommandList.size() < 2) { qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand; }

        std::string file = clientCommandList[1].toStdString();
        const char* fileName = file.c_str();
        Analyzer analyzer;
        std::vector<Result> results;
        Status s = analyzer.execute(fileName, "C:\\workspace\\my_repo\\sca\\test", results);

        QJsonObject jsonObject;
        if(s == SUCCESS) {
            qDebug() <<Q_FUNC_INFO<<"num of messages " <<results.size();
            for(Result r: results) {
                qDebug() <<"message " <<r.errorMessage;
            }
            QJsonArray resultsJson = JsonUtils::toJson(results);
            jsonObject[command] = resultsJson;
            //JsonUtils::fromJson(resultsJson);
        }
        else {
            qDebug() <<Q_FUNC_INFO<<"Analysis failed";
            qDebug() << Q_FUNC_INFO << "num of messages " << results.size();
            for (Result r : results) {
                qDebug() << "message " << r.errorMessage;
            }
            QJsonArray resultsJson = JsonUtils::toJson(results);
            jsonObject[command] = resultsJson;
        }
        QJsonDocument resultsDoc(jsonObject);
        p_clientSocket->write(resultsDoc.toJson());

        for (Result r : results) {
            delete r.errorMessage;
        }
        results.clear();
    }
    else if(command=="getCFG") {
        if (clientCommandList.size() < 2) { qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand; }

        std::string file = clientCommandList[1].toStdString();
        const char* fileName = file.c_str();

        Analyzer analyzer;
        BasicBlock* cfgHead;
        std::vector<Result> results;
        Status s = analyzer.getCFG(fileName, cfgHead, results);
        QJsonObject jsonObject;

        if(s == SUCCESS) {
            qDebug() <<Q_FUNC_INFO<<"CFG Head " <<cfgHead;
            QJsonArray cfgJson = JsonUtils::toJson(cfgHead);
            jsonObject["getCFG"] = cfgJson;
            qDebug() << "CFG Json " << cfgJson;
            //JsonUtils::fromCFGJson(cfgJson);
        }
        else {
            qDebug() <<Q_FUNC_INFO<<"Analysis failed";
        }
        for (Result r : results) {
            delete r.errorMessage;
        }
        results.clear();
        QJsonDocument cfgDoc(jsonObject);
        p_clientSocket->write(cfgDoc.toJson());

        for (Result r : results) {
            delete r.errorMessage;
        }
        results.clear();
        analyzer.clear();
    }
    else {
        qDebug() <<Q_FUNC_INFO <<"Invalid command" <<command;
    }

}

void SocketServer::clientDisconnected()
{
    qDebug() << Q_FUNC_INFO <<"Client Disconnected ";
    /*QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "app quit" << Qt::endl;
    file.close();*/
    qApp->quit();
}
