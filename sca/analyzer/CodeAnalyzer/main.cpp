#include <QCoreApplication>
#include <iostream>
#include "socketserver.h"

char* gFileName;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    /*if (argc < 2) {
        std::cout <<"Not enough arguments " <<std::endl;
        return 0;
    }
    gFileName = argv[1];
    std::cout << "command line argument :: file name " <<gFileName <<std::endl;*/
    SocketServer socketServer;
    return a.exec();
}
