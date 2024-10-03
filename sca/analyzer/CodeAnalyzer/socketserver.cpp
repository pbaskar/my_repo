#include "socketserver.h"
#include<QDebug>
#include "AnalyzerLibrary/analyzer.h"
#include "jsonutils.h"

extern char* gFileName;
SocketServer::SocketServer()
{
    connect(&p_timer, &QTimer::timeout, this, &SocketServer::timerTriggered);
    connect(this, &SocketServer::newConnection, this, &SocketServer::onNewConnection);
    listen(QHostAddress::LocalHost, 1234);
    p_timer.start(3000);
}

SocketServer::~SocketServer()
{

}

void SocketServer::onNewConnection()
{
    p_timer.start(30000);
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
    p_timer.start(30000);

    QByteArray command = p_clientSocket->readAll();
    qDebug() <<Q_FUNC_INFO<<command;
    if(command == "run") {
        Analyzer analyzer;
        std::vector<Result> results;
        Status s = analyzer.execute(gFileName, results);

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
        }
        QJsonDocument resultsDoc(jsonObject);
        p_clientSocket->write(resultsDoc.toJson());
    }
    else if(command.startsWith("getCFG")) {
        Analyzer analyzer;
        BasicBlock* cfgHead;
        Status s = analyzer.getCFG(gFileName, cfgHead);
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
        QJsonDocument cfgDoc(jsonObject);
        p_clientSocket->write(cfgDoc.toJson());
    }
    else {
        qDebug() <<Q_FUNC_INFO <<"Invalid command" <<command;
    }

}

void SocketServer::clientDisconnected()
{
    qDebug() << Q_FUNC_INFO <<"Client Disconnected ";
}

void SocketServer::timerTriggered()
{
    qDebug() <<Q_FUNC_INFO <<"Timer fired ";
    Analyzer analyzer;
    std::vector<Result> results;
    Status s = analyzer.execute(gFileName, results);

    QJsonObject jsonObject;
    if(s == SUCCESS) {
        qDebug() <<Q_FUNC_INFO<<"num of messages " <<results.size();
    }
    else {
        qDebug() <<Q_FUNC_INFO<<"Analysis failed";
    }
    qApp->quit();
}
