#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMetaEnum>
#include <windows.h>

class Client : public QObject
{
    Q_OBJECT
public:
   explicit Client(QObject *parent = nullptr);
   virtual void GetAccNo()=0;
   virtual void ViewAccount()=0;
   virtual void ViewTransactionHistory()=0;
   virtual void sendrequesttoserver()=0;
   virtual bool Login()=0;
   void clearScreen();

 protected:
    QString m_role;
    QString m_request;
    QVariant m_serverrespond;

};

#endif // CLIENT_H
