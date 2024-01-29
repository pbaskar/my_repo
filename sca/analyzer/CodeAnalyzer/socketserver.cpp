#include "socketserver.h"
#include<QDebug>
#include "AnalyzerLibrary/analyzer.h"
#include "jsonutils.h"

SocketServer::SocketServer()
{
    connect(&p_timer, &QTimer::timeout, this, &SocketServer::timerTriggered);
    connect(this, &SocketServer::newConnection, this, &SocketServer::onNewConnection);
    listen(QHostAddress::LocalHost, 1234);
    p_timer.start(30000);
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
        Status s = analyzer.execute("C:\\workspace\\my_repo\\sca\\test\\instructions.c", results);

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
    else if(command == "getCFG") {
        Analyzer analyzer;
        BasicBlock* cfgHead;
        Status s = analyzer.getCFG("C:\\workspace\\my_repo\\sca\\test\\instructions.c", cfgHead);
        QJsonObject jsonObject;

        if(s == SUCCESS) {
            qDebug() <<Q_FUNC_INFO<<"CFG Head " <<cfgHead;
            QJsonArray cfgJson = JsonUtils::toJson(cfgHead);
            jsonObject[command] = cfgJson;
            qDebug() << "CFG Json " << cfgJson;
            //JsonUtils::fromJson(cfgJson);
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
    /*Analyzer analyzer;
    BasicBlock* cfgHead;
    Status s = analyzer.getCFG("C:\\workspace\\my_repo\\sca\\test\\instructions.c", cfgHead);
    QByteArray cfgJson;

    if(s == SUCCESS) {
        qDebug() <<Q_FUNC_INFO<<"CFG Head " <<cfgHead;
        cfgJson = JsonUtils::toJson(cfgHead);
        qDebug() << "CFG Json " << cfgJson;
        JsonUtils::fromCFGJson(cfgJson);
    }
    else {
        qDebug() <<Q_FUNC_INFO<<"Analysis failed";
    }*/
    qApp->quit();
}
