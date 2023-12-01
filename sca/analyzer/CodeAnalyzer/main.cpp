#include <QCoreApplication>
#include "socketserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocketServer socketServer;
    return a.exec();
}
