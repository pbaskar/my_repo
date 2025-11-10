#include "socketclient.h"
#include <QDebug>
#include <QFile>

SocketClient::SocketClient()
{
}

void SocketClient::connectToServer() {
    connect(&p_socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(&p_socket, &QTcpSocket::errorOccurred, this, &SocketClient::onConnectionError);
    connect(&p_socket, &QTcpSocket::readyRead, this, &SocketClient::readData);
    p_socket.connectToHost(QHostAddress::LocalHost, 1234);
}

void SocketClient::writeToSocket(QByteArray data)
{
    /*QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "SocketClient::writetosocket" << data<<Qt::endl;
    file.close();*/
    qDebug() << Q_FUNC_INFO << data << Qt::endl;
    p_socket.write(data);
}

void SocketClient::close() {
    p_socket.close();
}

void SocketClient::onConnected()
{
    qDebug() <<Q_FUNC_INFO;
}

void SocketClient::onConnectionError()
{
    qDebug() << Q_FUNC_INFO <<p_socket.errorString();
    QByteArray resultsJson = "{\"run\":[{\"errorMessage\" : \"Connection Error\"}]}";
    QJsonDocument results = QJsonDocument::fromJson(resultsJson);
    emit resultsAvailable(results);
}

void SocketClient::readData()
{
    QByteArray resultsJson = p_socket.readAll();
    /*QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "SocketClient::readdata" << resultsJson << Qt::endl;
    file.close();*/
    qDebug() <<Q_FUNC_INFO << "Reply from Server " <<resultsJson;
    QJsonDocument results = QJsonDocument::fromJson(resultsJson);
    emit resultsAvailable(results);
}

SocketClient::~SocketClient()
{
    qDebug() <<Q_FUNC_INFO;
}
