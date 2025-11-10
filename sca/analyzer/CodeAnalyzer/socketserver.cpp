#include "socketserver.h"
#include<QDebug>
#include "analyzer.h"
#include "jsonutils.h"

extern char* gFileName;
SocketServer::SocketServer() : p_clientSocket(0)
{
    //connect(&p_timer, &QTimer::timeout, this, &SocketServer::timerTriggered);
    connect(this, &SocketServer::newConnection, this, &SocketServer::onNewConnection);
    listen(QHostAddress::LocalHost, 1234);
}

SocketServer::~SocketServer()
{
}

void SocketServer::onNewConnection()
{
    qDebug() << Q_FUNC_INFO <<"Incoming Connection";
    if(hasPendingConnections())
    {
        p_clientSocket = nextPendingConnection();
    }
    connect(p_clientSocket, &QTcpSocket::disconnected, this, &SocketServer::clientDisconnected);
    connect(p_clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readData);
}

void SocketServer::readData()
{
    QByteArray clientCommand = p_clientSocket->readAll();
    qDebug() <<Q_FUNC_INFO<< clientCommand;
    //QFile logfile("C:\\workspace\\my_repo\\sca\\test\\server.log");
    //logfile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    //QTextStream text(&logfile);


    QList<QByteArray> clientCommandList = clientCommand.split('#');
    if(clientCommandList.empty()) qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand;
    QByteArray command = clientCommandList[0];

    if(command == "run") {
        if (clientCommandList.size() < 2) { qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand; }

        std::string file = clientCommandList[1].toStdString();
        const char* fileName = file.c_str();
        Analyzer analyzer;
        std::vector<Result> results;

        QString outputPathQStr = QDir::tempPath(); //QCoreApplication::applicationDirPath();
        std::string outputStr = outputPathQStr.toStdString();
        const char* outputPath = outputStr.c_str();
        Status s = analyzer.execute(fileName, outputPath, results);

        QJsonObject jsonObject;
        if(s == SUCCESS) {
            qDebug() <<Q_FUNC_INFO<<"num of messages " <<results.size();
            for(Result r: results) {
                qDebug() <<"message " <<r.errorMessage;
            }
            QJsonArray resultsJson = JsonUtils::toJson(results);
            QJsonObject resultObject;
            resultObject["errorMessage"] = "Analysis succeeded";
            resultsJson.append(resultObject);
            jsonObject[command] = resultsJson;
            //JsonUtils::fromJson(resultsJson);
            //text << "SocketServer::Analysis success " << Qt::endl;
            qDebug()<< "SocketServer::Analysis success " << Qt::endl;

        }
        else {
            qDebug() <<Q_FUNC_INFO<<"Analysis failed";
            qDebug() << Q_FUNC_INFO << "num of messages " << results.size();
            for (Result r : results) {
                //text << "message " << r.errorMessage;
            }
            QJsonArray resultsJson = JsonUtils::toJson(results);
            QJsonObject resultObject;
            resultObject["errorMessage"] = "Analysis failed";
            resultsJson.append(resultObject);
            jsonObject[command] = resultsJson;
            //text << "SocketServer::Analysis failed " << Qt::endl;
            qDebug() << "SocketServer::Analysis failed " << Qt::endl;
        }

        QJsonDocument resultsDoc(jsonObject);
        p_clientSocket->write(resultsDoc.toJson());
        //text << "result written to clientSocket from server " << Qt::endl;
        qDebug() << "result written to clientSocket from server " << Qt::endl;

        for (Result r : results) {
            delete r.errorMessage;
        }
        //text << "result deleted " << Qt::endl;
        qDebug() << "result deleted " << Qt::endl;

        results.clear();
        //text << "result cleared " << Qt::endl;
        qDebug() << "result cleared " << Qt::endl;
        //text << outputPath << Qt::endl;

        analyzer.clear();
        //text << "analyzer cleared " << Qt::endl;
        qDebug() << "analyzer cleared " << Qt::endl;
    }
    else if(command=="getCFG") {
        if (clientCommandList.size() < 2) { qDebug() << Q_FUNC_INFO << "Invalid command" << clientCommand; }

        std::string file = clientCommandList[1].toStdString();
        const char* fileName = file.c_str();

        Analyzer analyzer;
        BasicBlock* cfgHead;
        std::vector<Result> results;

        QString outputPathQStr = QDir::tempPath(); //QCoreApplication::applicationDirPath();
        std::string outputStr = outputPathQStr.toStdString();
        const char* outputPath = outputStr.c_str();

        Status s = analyzer.getCFG(fileName, cfgHead, outputPath, results);
        QJsonObject jsonObject;

        if(s == SUCCESS) {
            qDebug() <<Q_FUNC_INFO<<"CFG Head " <<cfgHead;
            QJsonArray cfgJson = JsonUtils::toJson(cfgHead);
            jsonObject["getCFG"] = cfgJson;
            qDebug() << "CFG Json " << cfgJson;
            //JsonUtils::fromCFGJson(cfgJson);

            QJsonDocument cfgDoc(jsonObject);
            p_clientSocket->write(cfgDoc.toJson());
            //text << "result written to clientSocket from server " << Qt::endl;
            qDebug() << "result written to clientSocket from server " << Qt::endl;
        }
        else {
            qDebug() <<Q_FUNC_INFO<<"Analysis failed";
            qDebug() << Q_FUNC_INFO << "num of messages " << results.size();
            for (Result r : results) {
                qDebug() << "message " << r.errorMessage;
            }
            //QJsonArray resultsJson = JsonUtils::toJson(results);
            QJsonObject resultObject;
            //resultObject["errorMessage"] = "Analysis failed";
            //resultsJson.append(resultObject);
            jsonObject[command] = resultObject;

            QJsonDocument cfgDoc(jsonObject);
            p_clientSocket->write(cfgDoc.toJson());
            //text << "failure result written to clientSocket from server " << Qt::endl;
            qDebug() << "failure result written to clientSocket from server " << Qt::endl;
        }

        for (Result r : results) {
            delete r.errorMessage;
        }
        //text << "result deleted " << Qt::endl;
        qDebug() << "result deleted " << Qt::endl;

        results.clear();
        //text << "result cleared " << Qt::endl;
        qDebug() << "result cleared " << Qt::endl;
        //text << outputPath << Qt::endl;

        analyzer.clear();
        //text << "analyzer cleared " << Qt::endl;
        qDebug() << "analyzer cleared " << Qt::endl;
    }
    else {
        qDebug() <<Q_FUNC_INFO <<"Invalid command" <<command;
    }
    //logfile.close();
}

void SocketServer::clientDisconnected()
{
    qDebug() << Q_FUNC_INFO <<"Client Disconnected ";
    /*QFile file("C:\\workspace\\my_repo\\sca\\test\\server.log");
    file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text(&file);
    text << "app quit" << Qt::endl;
    file.close();*/
    qApp->quit();
}
