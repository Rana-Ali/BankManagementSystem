#ifndef ADMIN_H
#define ADMIN_H

#include "client.h"

class Admin : public Client
{
public:
    explicit Admin();
    static void GetAccNo(QString admin,QString username);
    static void ViewBankDatabase();
    static void CreateNewUser(QString userdata);
    static void Deleteuser(QString accountnumber);
    static void UpdateUser(QString accountnumber,QString newdata);
private:
    QTcpSocket socket;
    QString adminrequest;
    QVariant serverrespond;

};

#endif // ADMIN_H
