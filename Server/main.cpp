#include <QCoreApplication>
#include "myserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyServer server;
    server.StartServer();
    //ClientCommunication client1;
    //ClientCommunication client2;

    return a.exec();
}
