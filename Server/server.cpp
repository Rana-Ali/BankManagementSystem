#include "server.h"

Server::Server(QObject *parent)
    : QObject{parent}
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
    //take the request message and the role of our client to be able to handle the request
    QString request,role ;
    inStream>>request>>role;
    qDebug()<<request<<role;
    //call this method to handle the request according to client role
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
        QString fromtransferaccount,totransferaccount;
        quint32 transferamount;
        inStream >> fromtransferaccount >> totransferaccount >> transferamount;
        qDebug() << "Received from client:" << fromtransferaccount<<" "<<totransferaccount<<" "<<transferamount ;
        //sending respond to the server using the socket
        outStream<<ok;

    }
    else if(request=="View Account")
    {
        // create a map container to test the code
        QMap<QString,qint32>data;
        data["4545"]=12500;
        QString accountnumber;
        inStream >> accountnumber;
         //sending respond to the server using the socket
        outStream<<data[accountnumber];
    }
    else if(request=="Make Transaction")
    {
        QMap<QString,qint32>data;
        data["4545"]=12500;
        QString accountnumber;
        qint32 TransactionAmount;
        inStream >> accountnumber>>TransactionAmount;
    }
    }
}
