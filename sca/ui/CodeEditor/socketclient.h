#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include<QTcpSocket>

class SocketClient : public QObject
{
public:
    SocketClient();
    ~SocketClient();

    void writeToSocket(QByteArray data);

private slots:
    void readData();
    void onConnected();
    void onConnectionError();

private:
    QTcpSocket p_socket;
};

#endif // SOCKETCLIENT_H
