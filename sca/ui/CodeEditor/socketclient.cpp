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
    qDebug() << "SocketClient::onConnected ";
}

void SocketClient::onConnectionError()
{
    qDebug() << Q_FUNC_INFO <<p_socket.errorString();
}

void SocketClient::readData()
{
    qDebug() <<"Reply from Server " <<p_socket.readAll();
    //bytearray to QJsonDocument and then output
}

SocketClient::~SocketClient()
{

}
