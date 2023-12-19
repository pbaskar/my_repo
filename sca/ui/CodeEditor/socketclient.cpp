#include "socketclient.h"

SocketClient::SocketClient()
{
    connect(&p_socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(&p_socket, &QTcpSocket::errorOccurred, this, &SocketClient::onConnectionError);
    connect(&p_socket, &QTcpSocket::readyRead, this, &SocketClient::readData);
    p_socket.connectToHost(QHostAddress::LocalHost, 1234);
}

void SocketClient::writeToSocket(QByteArray data)
{
    p_socket.write(data);
}

void SocketClient::onConnected()
{
    qDebug() <<Q_FUNC_INFO;
}

void SocketClient::onConnectionError()
{
    qDebug() << Q_FUNC_INFO <<p_socket.errorString();
    QByteArray resultsJson = "[{\"errorMessage\" : \"Not initialized b\"}]";
    QJsonDocument results = QJsonDocument::fromJson(resultsJson);
    emit resultsAvailable(results);
}

void SocketClient::readData()
{
    QByteArray resultsJson = p_socket.readAll();
    qDebug() <<Q_FUNC_INFO << "Reply from Server " <<resultsJson;
    QJsonDocument results = QJsonDocument::fromJson(resultsJson);
    emit resultsAvailable(results);
}

SocketClient::~SocketClient()
{
    qDebug() <<Q_FUNC_INFO;
}
