#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include<QTcpSocket>
#include<QJsonDocument>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    SocketClient();
    ~SocketClient();

    void connectToServer();
    void writeToSocket(QByteArray data);
    void close();
signals:
    void resultsAvailable(const QJsonDocument results);

private slots:
    void readData();
    void onConnected();
    void onConnectionError();

private:
    QTcpSocket p_socket;
};

#endif // SOCKETCLIENT_H
