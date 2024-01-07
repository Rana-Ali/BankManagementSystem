/***********MyRunnable Class**********************************************/
/**********Author: Rana Ali ******************************************/
/********************Ver.: 01*****************************************/
/****************Date:7/01/2024.*************************************/
/*******************************************************************/
#include "myrunnable.h"

MyRunnable::MyRunnable(qintptr socketDescriptor)
    : QObject(nullptr), QRunnable(), socketDescriptor(socketDescriptor)
{
}

void MyRunnable::run()
{
    qInfo()<<this<<"run on"<<QThread::currentThread();

    if(!socketDescriptor)
    {
        return;
    }
    QTcpSocket* clientSocket=new QTcpSocket(nullptr);
    if (!clientSocket->setSocketDescriptor(socketDescriptor))
    {
        // Handle error
        qCritical() << clientSocket->errorString();
        delete clientSocket;
        return;
    }

    clientCommunication = new ClientCommunication(clientSocket);
    connect(clientCommunication, &ClientCommunication::disconnected, this, &MyRunnable::handleDisconnection,Qt::DirectConnection);
    qDebug() << "Thread ID:" << QThread::currentThreadId();

    while(clientSocket->isOpen())
    {
        clientSocket->waitForReadyRead();

    }

    qDebug()<<"socket is closed";

}

qintptr MyRunnable::getSocketDescriptor() const
{
    return socketDescriptor;
}

void MyRunnable::setSocketDescriptor(qintptr newSocketDescriptor)
{
    socketDescriptor = newSocketDescriptor;
}

 void MyRunnable::handleDisconnection()
 {
    qDebug() << "Client disconnected " ;
 }
