#ifndef USER_H
#define USER_H

#include "client.h"

class User : public Client
{
public:
    explicit User();
    void TransferAccount();
    void MakeTransaction();
    void ViewAccount();
    void GetAccNo();
    void ViewTransactionHistory();
    void sendrequesttoserver(QString request);
    bool Login();
    quint8 UserOptions();
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

};

#endif // USER_H
