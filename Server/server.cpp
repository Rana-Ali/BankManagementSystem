#include "server.h"

Server::Server(DataBase *parent)
    : DataBase{parent}
{
    connect(&server,&QTcpServer::newConnection,this,&Server::newConnection);
}

void Server::start()
{
    if(!server.listen(QHostAddress::Any,1234))
    {
        qWarning()<<server.errorString();
        return;
    }
    qInfo()<<"Listening";
}

void Server::quit()
{
    server.close();
}

void Server::newConnection()
{
    QTcpSocket* socket=server.nextPendingConnection();
    connect(socket,&QTcpSocket::disconnected,this,&Server::disconnected);
    connect(socket,&QTcpSocket::readyRead,this,&Server::readyRead);
    qInfo()<<"connected to"<<socket;
}

void Server::disconnected()
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    qInfo()<<"Disconnected"<<socket;
    qInfo()<<"parent"<<socket->parent();
}

void Server::readyRead()
{
    //create pointer to the socket connected to the object send the request
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    //create an in datasream to take the needed data from the client
    QDataStream inStream(socket);
    inStream.setVersion(QDataStream::Qt_6_6);
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
    inStream>>request>>role;
    qDebug()<<request<<role;
    }
    //call this method to handle the request according to client role
    setRole(role);
    Handlerequest(request,role);
}

void Server::Handlerequest(QString request,QString role)
{
    //create pointer to the socket connected to the object send the request
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    //create an in datasream to take the needed data from the client
    QDataStream inStream(socket);
    //create an out datastream to send the respond to the client
    QDataStream outStream(socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    outStream.setVersion(QDataStream::Qt_6_6);
    //check if oour client is user or admin to handle his request
    if(role.toUpper()=="USER")
    {
    if(request=="Transfer Account")
    {
        bool ok = true;
        QString totransferaccount;
        quint32 transferamount;
        inStream >> totransferaccount >> transferamount;
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
        inStream>>username>>password;
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
    else if (role.toUpper()=="ADMIN")
    {
        if(request=="Login")
        {
            QString username , password;
            bool ok;
            inStream>>username>>password;
            qDebug() << "Received from client:" << username<<" "<<password ;
            ok=checkLogin(username,password);
            //sending respond to the server using the socket
            outStream<<ok;

        }
        else if(request=="View Account")
        {
            QString accountnumber;
            inStream >>accountnumber;
            qint32 Balance =ViewAccountBalance(accountnumber);
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
                inStream>>accountnumber>>map;
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
                QString username,password;
                QVariantMap map;
                inStream>>username>>password>>map;
                qInfo()<<username<<password;
                bool ok=CreateUser(username,password,map);
                outStream<<ok;

            }

        }
        else if(request=="View Transaction History")
        {
            QString accountnumber;
            quint16 count;
            inStream>>accountnumber>>count;
            QString data=ViewTransactionHistory(accountnumber,count);
            outStream<<data;


        }
    }
}
