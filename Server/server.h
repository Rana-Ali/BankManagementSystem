#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include "database.h"
#include <QThreadPool>
#include "clientcommunication.h"


class Server : public DataBase
{
    Q_OBJECT
public:
    explicit Server( DataBase *parent = nullptr);
    ~Server();
    void run();
    //void Handlerequest(QString,QString);
protected:
    void incomingConnection( qintptr handle );
signals:
public slots:
    void start();
    void quit();
    void newConnection();
    void disconnected();
    void handleAdminRequest(const QString& request);
    void handleUserRequest(const QString& request);
private:
    QTcpServer server;
    QThread serverThread;
    QThreadPool *pool;
    ClientCommunication* clientCommunication;

};

#endif // SERVER_H
