#include "user.h"
#include <iostream>
#include<QDataStream>
#include<QDebug>

User::User()
{
    m_role="user";
    connect(&socket,&QTcpSocket::connected,this,&User::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&User::disconnected);
    connect(&socket,&QTcpSocket::stateChanged,this,&User::stateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&User::readyRead);
    connect(&socket,&QTcpSocket::errorOccurred,this,&User::error);
}


void User::TransferAccount()
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
    socket.waitForBytesWritten();
    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Transaction is :"<<m_serverrespond.toBool();
}

void User::MakeTransaction()
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
    qInfo()<<"Transaction is :"<<m_serverrespond.toBool();
}

void User::ViewAccount()
{
    // create a datastream to send the nedded info to the server to get the the money in the account
    QDataStream outStream(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    qInfo()<<"Please send the account number:";
    std::string accountNumber;
    std::cin>>accountNumber;
    QString AccountNumber=QString::fromStdString(accountNumber);
    outStream<<AccountNumber;
    socket.waitForBytesWritten();
    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Your account money is :"<<m_serverrespond.toInt();
}

void User::GetAccNo()
{
    socket.waitForReadyRead();
    qInfo()<<"Your account number is:"<<m_serverrespond.toString();
}

void User::ViewTransactionHistory()
{
    QDataStream outStream(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    qInfo()<<"Please send the account number:";
    std::string accountNumber;
    std::cin>>accountNumber;
    QString AccountNumber=QString::fromStdString(accountNumber);
    qInfo()<<"Please send the number of transactions:";
    quint32 count;
    std::cin>>count;
    outStream<<AccountNumber<<count;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo()<<m_serverrespond.toString();
}

void User::sendrequesttoserver(QString request)
{
    QDataStream outStream(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    outStream<<request<<m_role;
    m_request=request;

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
    else if(request=="Make Transaction")
    {
        MakeTransaction();
    }
    else if(request=="Get Acc No")
    {
        GetAccNo();
    }
    else if(request=="View Transaction History")
    {
        ViewTransactionHistory();
    }
    else
    {
        qFatal()<<"The request message is not defined";
    }

}

bool User::Login()
{
    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    QString username,password;
    std::string name;
    std::cin>>name;
    username=QString::fromStdString(name);
    qInfo()<<"Password: ";
    std::string pass;
    std::cin>>pass;
    password=QString::fromStdString(pass);
    bool ok =false;
    quint8 count =0;
    while(count<3)
    {
        count++;
        if(!username.isEmpty()&&!password.isEmpty())
        {
            ok=true;
            break;
        }
    }
    return ok;
}



void User::connectToHost(QString host, quint16 port)
{
    if(socket.isOpen()) disconnect();
    //qInfo()<<"connecting to host"<<host<<"on port:"<<port;
    socket.connectToHost(host,port);
}

void User::disconnect()
{
    socket.close();
    socket.disconnectFromHost();
    socket.waitForDisconnected();
}

void User::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    socket.write(userInput.toUtf8());
}

void User::disconnected()
{
    qInfo()<<"Thank you!...system is closing";
    qInfo()<<"Disconnected";
}

void User::error(QAbstractSocket::SocketError socketerror)
{
    qInfo()<<"Error:"<<socketerror<<socket.errorString();
}

void User::stateChanged(QAbstractSocket::SocketState socketstate)
{
    QMetaEnum metaenum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str= metaenum.valueToKey(socketstate);
}

void User::readyRead()
{
 QDataStream inStream(&socket);
 inStream.setVersion(QDataStream::Qt_6_6);

    if (m_request=="View Account")
    {
        quint32 AccountMoney;
        //take the money in the account from the server and save it in the server respond
        inStream>>AccountMoney;
        m_serverrespond.setValue(AccountMoney);
    }
    else if(m_request=="Transfer Account"||m_request=="Make Transaction")
    {
        bool respond;
        //know if the transaction succeed or not from the server and save it in the server respond
        inStream>>respond;
        m_serverrespond.setValue(respond);
    }
    else if(m_request=="Get Acc No")
    {
        QString AccNo;
        inStream>>AccNo;
        m_serverrespond.setValue(AccNo);
    }
    else if(m_request=="View Transaction History")
    {
        QString History;
        inStream>>History;
        m_serverrespond.
    }
}


