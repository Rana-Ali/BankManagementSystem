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
    void sendrequesttoserver();
    bool Login();
    quint8 UserOptions();
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
    QString m_userName;
    QString m_accountNumber;

};

#endif // USER_H
