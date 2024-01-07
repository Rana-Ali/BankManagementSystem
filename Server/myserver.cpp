/***********server Class**********************************************/
/**********Author: Rana Ali ******************************************/
/********************Ver.: 01*****************************************/
/****************Date:5/01/2024.*************************************/
/*******************************************************************/
#include "myserver.h"
#include "myrunnable.h"

MyServer::MyServer(QObject *parent)
    : QTcpServer{parent}
{
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(5);
    qDebug()<<"Threads:"<<pool->maxThreadCount();
}

void MyServer::StartServer()
{
    qInfo()<<this<<"start"<<QThread::currentThread();
    if(this->listen(QHostAddress::Any,1234))
    {
        qDebug()<<"Listening";
    }
    else
    {
        qDebug()<<"server not started"<<this->errorString();
    }
}

void MyServer::incomingConnection(qintptr handle)
{
    MyRunnable *task = new MyRunnable(handle);
    task->setAutoDelete(true);

    pool->start(task);

    qDebug() << "pool started";
    qInfo()<<"connected to"<<socketDescriptor();
}

void MyServer::quit()
{
    this->close();
    qInfo()<<"server stopped";
}
