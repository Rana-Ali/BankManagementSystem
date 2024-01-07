#ifndef MYRUNNABLE_H
#define MYRUNNABLE_H

#include <QRunnable>
#include <QThread>
#include <QObject>
#include "clientcommunication.h"
#include <QTcpSocket>
#include <QDebug>

//QRunnable is not a thread it's the task executed on the thread in the thread pool
class MyRunnable :public QObject, public QRunnable
{
    Q_OBJECT
public:
    MyRunnable(qintptr socketDescriptor);
    void run() override;

    qintptr getSocketDescriptor() const;
    void setSocketDescriptor(qintptr newSocketDescriptor);
public slots:
   void handleDisconnection();

private:
    qintptr socketDescriptor;
    ClientCommunication* clientCommunication;
};
#endif // MYRUNNABLE_H
