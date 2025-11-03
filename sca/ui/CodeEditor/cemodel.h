#ifndef CEMODEL_H
#define CEMODEL_H
#include<QObject>
#include<QProcess>
#include "socketclient.h"
#include "positionblock.h"
#include "edge.h"

class CEModel : public QObject
{
    Q_OBJECT
public:
    CEModel();
    ~CEModel();
    static CEModel* getInstance();
    void sendCommand(QString command);
    void disconnect();
signals:
    void resultsAvailable(const QVariantList results);
    void CFGAvailable(const QList<PositionBlock> cfg);
    void edgesAvailable(const QList<Edge> edges);
public slots:
    void onResultsAvailable(const QJsonDocument results);
private:
    QProcess p_server;
    SocketClient p_socketClient;
};

#endif // CEMODEL_H
