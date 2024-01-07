#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>
#include <QDebug>

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);
    void StartServer();
protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE ;
private:
    QThreadPool* pool;
public slots:
    void quit();
};

#endif // MYSERVER_H
