#include "socketserver.h"
#include<QDebug>
#include "AnalyzerLibrary/analyzer.h"

SocketServer::SocketServer()
{
    connect(&p_timer, &QTimer::timeout, this, &SocketServer::timerTriggered);
    connect(this, &SocketServer::newConnection, this, &SocketServer::onNewConnection);
    listen(QHostAddress::LocalHost, 1234);
    p_timer.start(5);
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

void SocketServer::readData()
{
    p_timer.start(30000);
    qDebug() <<p_clientSocket->readAll();

    //Convert vector of Result to QJsonDocument
    p_clientSocket->write("toclient");
}

void SocketServer::clientDisconnected()
{
    qDebug() <<Q_FUNC_INFO;
    qDebug() << "Client Disconnected ";
}

void SocketServer::timerTriggered()
{
    qDebug() <<"Timer fired ";
    Analyzer analyzer;
    std::vector<Result> results;
    Status s = analyzer.execute("C:\\workspace\\my_repo\\sca\\test\\instructions.c", results);
    if(s == SUCCESS) {
        qDebug() <<"num of messages " <<results.size();
        for(Result r: results) {
            qDebug() <<"message " <<r.errorMessage;
        }
    }
    else {
        qDebug() <<"Analysis failed";
    }
    qApp->quit();
}
