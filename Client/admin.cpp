#include "admin.h"
#include <iostream>
Admin::Admin()
{
    m_role="admin";
    connect(&socket,&QTcpSocket::connected,this,&Admin::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&Admin::disconnected);
    connect(&socket,&QTcpSocket::stateChanged,this,&Admin::stateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&Admin::readyRead);
    connect(&socket,&QTcpSocket::errorOccurred,this,&Admin::error);
}

void Admin::ViewAccount()
{

}

void Admin::GetAccNo()
{

}

void Admin::ViewBankDatabase()
{

}

void Admin::CreateNewUser()
{

}

void Admin::Deleteuser()
{

}

void Admin::UpdateUser()
{

}

void Admin::ViewTransactionHistory()
{

}

void Admin::sendrequesttoserver(QString request)
{

}

bool Admin::Login()
{
    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    QString adminname,password;
    std::string name;
    std::cin>>name;
    adminname=QString::fromStdString(name);
    qInfo()<<"Password: ";
    std::string pass;
    std::cin>>pass;
    password=QString::fromStdString(pass);
    bool ok =false;
    quint8 count =0;
    while(count<3)
    {
        count++;
        if(!adminname.isEmpty()&&!password.isEmpty())
        {
            ok=true;
            break;
        }
    }
    return ok;
}

void Admin::connectToHost(QString host, quint16 port)
{
    if(socket.isOpen()) disconnect();
    //qInfo()<<"connecting to host"<<host<<"on port:"<<port;
    socket.connectToHost(host,port);
}

void Admin::disconnect()
{
    socket.close();
    socket.waitForDisconnected();
}

void Admin::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    socket.write(userInput.toUtf8());
}

void Admin::disconnected()
{
    qInfo()<<"Disconnected";
}

void Admin::error(QAbstractSocket::SocketError socketerror)
{
 qInfo()<<"Error:"<<socketerror<<socket.errorString();
}

void Admin::stateChanged(QAbstractSocket::SocketState socketstate)
{
    QMetaEnum metaenum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str= metaenum.valueToKey(socketstate);
}

void Admin::readyRead()
{
 QDataStream inStream(&socket);
}



