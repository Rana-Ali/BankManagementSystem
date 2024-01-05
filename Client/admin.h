#ifndef ADMIN_H
#define ADMIN_H

#include "client.h"

class Admin : public Client
{
public:
    explicit Admin();
    void ViewAccount();
    void GetAccNo();
    void ViewBankDatabase();
    void CreateNewUser();
    void Deleteuser();
    void UpdateUser();
    void ViewTransactionHistory();
    void sendrequesttoserver();
    bool Login();
    void Start(bool&);

signals:
public slots:
    void connectToHost(QString host,quint16 port);
    void disconnect();
protected slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketerror);
    void stateChanged(QAbstractSocket::SocketState socketstate);
    void readyRead();
private:
        QTcpSocket socket;
        QDataStream outStream;
        QDataStream inStream;
        QString m_adminname;


};

#endif // ADMIN_H
