#ifndef CLIENTCOMMUNICATION_H
#define CLIENTCOMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
#include "database.h"

class ClientCommunication : public DataBase
{
    Q_OBJECT
public:
    explicit ClientCommunication(QTcpSocket* socket,DataBase *parent = nullptr);

signals:
    void adminRequest(const QString& request);
    void userRequest(const QString& request);
    void requestReceived(const QString& request);
    void disconnected();
public slots:
    void handleRequest();
   // void sendResponse(const QVariant &response);
    //void recieveData(QVariant &data);
    void handleAdminRequest(const QString& request);
    void handleUserRequest(const QString& request);

    // Slot to handle disconnection
    void handleDisconnection();


private:
    void parseRequest(const QString& request,const QString &role);
     QDataStream outStream;
     QDataStream inStream;
};

#endif // CLIENTCOMMUNICATION_H
