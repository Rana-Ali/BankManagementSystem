#include "client.h"
#include <QTextStream>
#include <iostream>
#include<QDataStream>
#include<QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
{
    connect(&socket,&QTcpSocket::connected,this,&Client::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&Client::disconnected);
    connect(&socket,&QTcpSocket::stateChanged,this,&Client::stateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&Client::readyRead);
    connect(&socket,&QTcpSocket::errorOccurred,this,&Client::error);

}

void Client::TransferAccount()
{
   // create a datastream to send the nedded info to the server to transfer the money.
    QDataStream outStream(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    qInfo()<<"Please send the account number from which you will transfer:";
    std::string fromaccountnumber;
    std::cin>>fromaccountnumber;
    qInfo()<<"Please send the account number to which you will transfer:";
    std::string toaccountnumber;
    std::cin>>toaccountnumber;
    qInfo()<<"Please send the transfer amount:";
    quint32 transferamount;
    std::cin>>transferamount;
    //convert the data from stdString to QString to send it to the server and process it.
    QString fromAccountNumber=QString::fromStdString(fromaccountnumber);
    QString ToAccountNumber=QString::fromStdString(toaccountnumber);
   //send the data to the server to handle that
    outStream<<fromAccountNumber<<ToAccountNumber<<transferamount;
   //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Transaction is :"<<serverrespond.toBool();


}

void Client::MakeTransaction()
{
    // create a datastream to send the nedded info to the server to make a transacton
    QDataStream outStream(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    qInfo()<<"Please send the account number:";
    std::string accountNumber;
    std::cin>>accountNumber;
    QString AccountNumber=QString::fromStdString(accountNumber);
    qInfo()<<"Please send the transaction amount:";
    qint32 TransactionAmount;
    std::cin>> TransactionAmount;
    //send the data to the server to handle that
    outStream<<AccountNumber<<TransactionAmount;
     socket.waitForReadyRead();
     qInfo()<<"Transaction is :"<<serverrespond.toBool();

}

void Client::ViewAccount()
{
  // create a datastream to send the nedded info to the server to get the the money in the account
  QDataStream outStream(&socket);
  outStream.setVersion(QDataStream::Qt_6_6);
  qInfo()<<"Please send the account number:";
  std::string accountNumber;
  std::cin>>accountNumber;
  QString AccountNumber=QString::fromStdString(accountNumber);
  outStream<<AccountNumber;
  //wait for the respond from the server to view it to the client
  socket.waitForReadyRead();
  qInfo()<<"Your account money is :"<<serverrespond.toInt();
}

void Client::ViewTransactionHistory()
{

}

void Client::sendrequesttoserver(QString request)
{
    //create an out datastream to send the request message and role of our client to the server
    QDataStream outStream(&socket);
    outStream<<request<<role;
    //check if our client is user or admin to assign the request to userrequest or adminrequest and handle the request
    if(role.toUpper()=="USER")
    {
       userrequest = request;

    if(request=="Transfer Account")
    {
        //call the method Transfer Account to handle this request
        TransferAccount();
    }
    else if(request=="View Account")
    {
        //call the method view Account to handle this request
        ViewAccount();
    }


    }
    else if(role.toUpper()=="ADMIN")
    {
        adminrequest= request;
    }



}

// void Client::Login(QString username,QString password)
// {
//     bool isvalid = false;
//     // do
//     // {
//     //     qInfo()<<"Please Enter Your Role `user` or `admin` ";
//     //     QTextStream qin(stdin);
//     //     role = qin.readLine();
//     //     qInfo()<<"username: ";
//     //     username = qin.readLine();
//     //     qInfo()<<"Password: ";
//     //     password = qin.readLine();
//     //     isvalid =true;
//     // }
//     // while(!isvalid);

// }

void Client::connectToHost(QString host, quint16 port)
{
    if(socket.isOpen()) disconnect();
    //qInfo()<<"connecting to host"<<host<<"on port:"<<port;
    socket.connectToHost(host,port);
}

void Client::disconnect()
{
    socket.close();
    socket.waitForDisconnected();
}

void Client::connected()
{
    //qInfo()<<"connected!";
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    socket.write(userInput.toUtf8());
}

void Client::disconnected()
{
    qInfo()<<"Disconnected";
}

void Client::error(QAbstractSocket::SocketError socketerror)
{
    qInfo()<<"Error:"<<socketerror<<socket.errorString();
}

void Client::stateChanged(QAbstractSocket::SocketState socketstate)
{
    QMetaEnum metaenum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
   QString str= metaenum.valueToKey(socketstate);
}

void Client::readyRead()
{
    //create a data stream to take the respond from the server using the socket
    QDataStream inStream(&socket);
    //check if our client is user or admin to handle the respond on the socket
    if(role.toUpper()=="USER")
    {
    if (userrequest=="View Account")
    {
       quint32 AccountMoney;
      //take the money in the account from the server and save it in the server respond
       inStream>>AccountMoney;
       serverrespond.setValue(AccountMoney);
    }
    else if(userrequest=="Transfer Account"||userrequest=="Make Transaction")
    {
        bool respond;
       //know if the transaction succeed or not from the server and save it in the server respond
        inStream>>respond;
        serverrespond.setValue(respond);
    }

    }
}

QString Client::getRole() const
{
    return role;
}

void Client::setRole(const QString &newRole)
{
    role = newRole;
}
