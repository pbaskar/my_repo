#ifndef CEMODEL_H
#define CEMODEL_H
#include<QObject>
#include "socketclient.h"
#include "positionblock.h"

class CEModel : public QObject
{
    Q_OBJECT
public:
    CEModel();
    ~CEModel();
    static CEModel* getInstance();
    void sendCommand(QString command);
signals:
    void resultsAvailable(QVariantList results);
    void CFGAvailable(const QList<PositionBlock> cfg);
public slots:
    void onResultsAvailable(QJsonDocument results);
private:
    SocketClient p_socketClient;
};

#endif // CEMODEL_H
