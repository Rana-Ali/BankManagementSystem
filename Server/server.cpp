#include "server.h"
#include "myrunnable.h"

Server::Server(DataBase *parent)
    : DataBase{parent}
{
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(5);
    // this->moveToThread(&serverThread);
    //server.listen(QHostAddress::Any, 1234);
    connect(&server,&QTcpServer::newConnection,this,&Server::newConnection);
    // serverThread.start();
}

Server::~Server()
{
    quit();
    serverThread.quit();
    serverThread.wait();
}

void Server::incomingConnection(qintptr handle)
{
    MyRunnable *task = new MyRunnable(handle);
    task->setAutoDelete(true);

    task->setSocketDescriptor(handle);

    pool->start(task);
    qDebug() << "pool started";
}

void Server::start()
{
    if(!server.listen(QHostAddress::Any,1234))
    {
        qWarning()<<server.errorString();
        return;
    }
    qInfo()<<"Listening";
}

void Server::quit()
{
    server.close();
}

void Server::newConnection()
{
    QTcpSocket* clientSocket=server.nextPendingConnection();
    QThread* clientThread = new QThread;
    ClientCommunication* clientCommunication = new ClientCommunication(clientSocket);
    connect(clientCommunication, &ClientCommunication::adminRequest, this, &Server::handleAdminRequest);
    connect(clientCommunication, &ClientCommunication::userRequest, this, &Server::handleUserRequest);
    connect(clientThread, &QThread::finished, clientCommunication, &ClientCommunication::deleteLater);
    connect(clientThread, &QThread::finished, clientThread, &QThread::deleteLater);
    // Move the clientCommunication object to the clientThread
    clientCommunication->moveToThread(clientThread);

    // Start the clientThread
    clientThread->start();
    qInfo()<<"connected to"<<clientSocket;
}

void Server::disconnected()
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    qInfo()<<"Disconnected"<<socket;
    qInfo()<<"parent"<<socket->parent();
}




