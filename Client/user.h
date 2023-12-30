#ifndef USER_H
#define USER_H

#include "client.h"

class User : public Client
{
public:
    explicit User();
    QString GetAccNo(QString username);
    void TransferAccount(QString fromaccountnumber,QString toaccountnumber,qint32 transferamount);
private:
    QTcpSocket socket;
    QString userrequest;
    QVariant serverrespond;

};

#endif // USER_H
