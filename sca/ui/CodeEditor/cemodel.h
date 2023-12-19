#ifndef CEMODEL_H
#define CEMODEL_H
#include<QObject>
#include "socketclient.h"

class CEModel : public QObject
{
    Q_OBJECT
public:
    CEModel();
    ~CEModel();
    void sendCommand(QString command);
signals:
    void resultsAvailable(QVariantList results);
public slots:
    void onResultsAvailable(QJsonDocument results);
private:
    SocketClient p_socketClient;
};

#endif // CEMODEL_H
