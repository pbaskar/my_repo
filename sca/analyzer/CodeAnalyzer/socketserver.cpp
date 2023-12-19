#include "socketserver.h"
#include<QDebug>
#include "AnalyzerLibrary/analyzer.h"

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
    qDebug() << "Incoming Connection";
    if(hasPendingConnections())
    {
        p_clientSocket = nextPendingConnection();
    }
    connect(p_clientSocket, &QTcpSocket::disconnected, this, &SocketServer::clientDisconnected);
    connect(p_clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readData);
}

QByteArray toJson(const std::vector<Result>& results) {
    QJsonArray resultArray;
    for(Result r : results) {
        QJsonObject resultObject;
        resultObject["errorMessage"] = r.errorMessage;
        resultArray.append(resultObject);
    }
    QJsonDocument resultsDoc(resultArray);
    return resultsDoc.toJson();
}

void fromJson(QByteArray resultsJson) {
    QJsonDocument resultsDoc = QJsonDocument::fromJson(resultsJson);
    QJsonArray resultsArray = resultsDoc.array();
    for(const QJsonValue& value : resultsArray) {
        QJsonObject obj = value.toObject();
        qDebug() << "fromJson " <<obj["errorMessage"].toString();
    }
}

void SocketServer::readData()
{
    p_timer.start(30000);
    qDebug() <<p_clientSocket->readAll();

    Analyzer analyzer;
    std::vector<Result> results;
    Status s = analyzer.execute("C:\\workspace\\my_repo\\sca\\test\\instructions.c", results);
    QByteArray resultsJson;

    if(s == SUCCESS) {
        qDebug() <<"num of messages " <<results.size();
        for(Result r: results) {
            qDebug() <<"message " <<r.errorMessage;
        }
        resultsJson = toJson(results);
        fromJson(resultsJson);
    }
    else {
        qDebug() <<"Analysis failed";
    }
    p_clientSocket->write(resultsJson);
}

void SocketServer::clientDisconnected()
{
    qDebug() <<Q_FUNC_INFO;
    qDebug() << "Client Disconnected ";
}

void SocketServer::timerTriggered()
{
    qDebug() <<"Timer fired ";
    qApp->quit();
}
