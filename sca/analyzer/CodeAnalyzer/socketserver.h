#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#include<QTimer>
#include<QtCore>
#include<QTcpServer>
#include<QTcpSocket>

class SocketServer : public QTcpServer
{
    Q_OBJECT
public:
    SocketServer();
    ~SocketServer();
private slots:
    //void timerTriggered();
    void onNewConnection();
    void clientDisconnected();
    void readData();
private:
    QTimer p_timer;
    QTcpSocket* p_clientSocket;
};

#endif // SOCKETSERVER_H
