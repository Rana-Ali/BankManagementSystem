/***********client communication Class**********************************************/
/**********Author: Rana Ali *******************************************************/
/********************Ver.: 01*****************************************************/
/****************Date:4/01/2024.*************************************************/
/*******************************************************************************/
#include "clientcommunication.h"

ClientCommunication::ClientCommunication(QTcpSocket* socket,DataBase *parent)
    : DataBase{parent}
{
    connect(socket, &QTcpSocket::readyRead, this, &ClientCommunication::handleRequest,Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &ClientCommunication::handleDisconnection,Qt::DirectConnection);
    outStream.setDevice(socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    inStream.setDevice(socket);
    inStream.setVersion(QDataStream::Qt_6_6);
}

void ClientCommunication::handleRequest()
{
    //take the request message and the role of our client to be able to handle the request
    QString request,role,requestflag;
    inStream>>requestflag;
    if(requestflag=="CreateUser")
    {
        role="admin";
        request="Create User";
    }
    else if(requestflag=="UpdateUser")
    {
        role="admin";
        request="Update User";
    }
    else if(requestflag=="General")
    {
        inStream>>request;
        inStream>>role;
        qDebug()<<request<<role;
    }
    parseRequest(request,role);

}



void ClientCommunication::parseRequest(const QString &request,const QString &role)
{
   if (role.toUpper() == "ADMIN") {
        handleAdminRequest(request);
   } else if (role.toUpper() == "USER") {
       handleUserRequest(request);
   }

}
void ClientCommunication::handleAdminRequest(const QString &request)
{
    setRole("admin");
    if(request=="Login")
    {
        QString  username , password;
        bool ok;
        inStream>>username;
        inStream>>password;

        qDebug() << "Received from client:" << username <<" "<<password;
        ok=checkLogin(username,password);
        //sending respond to the server using the socket
        qInfo()<<ok;
        outStream<<ok;

    }
    else if(request=="View Account")
    {

        QString accountnumber;
        inStream>>accountnumber;
        qInfo()<<accountnumber;
        quint32 Balance =ViewAccountBalance(accountnumber);
        qInfo()<<Balance;
            //sending respond to the server using the socket
        outStream<<Balance;
    }
    else if(request=="GetAccNo")
    {
        QString username,accountnumber;
        inStream>>username;
        accountnumber = GetAccNo(username);
        outStream<<accountnumber;

    }
    else if(request=="Update User")
    {
        QString flag;
        inStream>>flag;
        if(flag=="check")
        {
            QString accountnumber;
            inStream>>accountnumber;
            qInfo()<<accountnumber;
            bool ok=checkAccNo(accountnumber);
            outStream<<ok;

        }
        else if(flag=="update")
        {
            QString accountnumber;
            QVariantMap map;
            inStream>>accountnumber;
            inStream>>map;
            qInfo()<<accountnumber;
            bool ok=UpdateUser(accountnumber,map);
            outStream<<ok;
        }

    }
    else if(request=="Create User")
    {
        QString flag;
        inStream>>flag;
        if(flag=="check")
        {
            QString username;
            inStream>>username;
            bool ok=checkUsername(username);
             outStream<<ok;

        }
        else if(flag=="update")
        {
            QVariantMap map;
            QString username,password;
            inStream>>username;
            inStream>>password;
            inStream>>map;

            qInfo()<<username<<password;
            bool ok=CreateUser(username,password,map);
            outStream<<ok;
        }

    }
    else if(request=="View Transaction History")
    {

        QString accountnumber;
        quint16 count;
        inStream>>accountnumber;
        inStream>>count;
        QString data=ViewTransactionHistory(accountnumber,count);
        outStream<<data;


    }
    else if(request=="Delete User")
    {
        QString accountnumber;
        inStream>>accountnumber;
        bool ok=DeleteUser(accountnumber);
        outStream<<ok;

    }
    else if(request=="View Bank DataBase")
    {
        QString Database=ViewBankDataBase();
        outStream<<Database;

    }
}

void ClientCommunication::handleUserRequest(const QString &request)
{
    setRole("user");
    if(request=="Transfer Account")
    {
        bool ok = true;
        QString totransferaccount;
        quint32 transferamount;
        inStream >> totransferaccount;
        inStream >> transferamount;
        qDebug() << "Received from client:"<<totransferaccount<<" "<<transferamount ;
        ok=TransferAmount(totransferaccount,transferamount);
        //sending respond to the server using the socket
        outStream<<ok;

    }
    else if(request=="View Account")
    {
        QString m_accountnumber=accountnumber();
        qint32 Balance =ViewAccountBalance(m_accountnumber);
            //sending respond to the server using the socket
        outStream<<Balance;
    }
    else if(request=="Make Transaction")
    {
        bool ok = true;
        //quint16 TransactionNumber=0;
        qint32 TransactionAmount=0;
        inStream >> TransactionAmount;
        ok=MakeTransaction(TransactionAmount);
        outStream<<ok;
    }
    else if(request=="Login")
    {
        QString username , password;
        bool ok;
        inStream>>username;
        inStream>>password;
        qDebug() << "Received from client:" << username<<" "<<password ;
        ok=checkLogin(username,password);
        //sending respond to the server using the socket
        outStream<<ok;

    }
    else if(request=="GetAccNo")
    {
        QString username,accountnumber;
        inStream>>username;
        accountnumber = GetAccNo(username);
        outStream<<accountnumber;

    }
    else if(request =="View Transaction History")
    {
        quint16 count;
        QString m_accountnumber=accountnumber();
        inStream>>count;
        QString data=ViewTransactionHistory(m_accountnumber,count);
        outStream<<data;
    }
}

 void ClientCommunication::handleDisconnection()
 {
     emit disconnected();

 }
