#include "user.h"
#include <iostream>
#include<QDataStream>
#include<QDebug>

User::User()
{
    outStream.setDevice(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    inStream.setDevice(&socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    m_role="user";
    m_request="Login";
    m_requestflag="General";
    connect(&socket,&QTcpSocket::connected,this,&User::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&User::disconnected);
    connect(&socket,&QTcpSocket::stateChanged,this,&User::stateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&User::readyRead);
    connect(&socket,&QTcpSocket::errorOccurred,this,&User::error);
}


void User::TransferAccount()
{
    qInfo()<<"Please send the account number to which you will transfer:";
    std::string toaccountnumber;
    std::cin>>toaccountnumber;
    qInfo()<<"Please send the transfer amount:";
    quint32 transferamount;
    std::cin>>transferamount;
    //convert the data from stdString to QString to send it to the server and process it.
    QString ToAccountNumber=QString::fromStdString(toaccountnumber);
    //send the data to the server to handle that
    outStream<<ToAccountNumber<<transferamount;
    socket.waitForBytesWritten();
    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Transaction is :"<<m_serverrespond.toBool();
}

void User::MakeTransaction()
{
    qint32 TransactionAmount=0;
    //quint16 TransactionNumber=0;
    // qInfo()<<"1-Withdrawal\2-Deposit";
    // std::string in;
    // std::cin>>in;
    // std::cin.ignore();
    // QString input = QString::fromStdString(in);
    // switch(input.toUInt())
    // {
    // case 1:
    //     TransactionNumber=1;
    //     break;
    // case 2:
    //     TransactionNumber=2;
    //     break;
    // default:
    //     qInfo()<<"Your input is not valid";
    //     break;
    // }
    // qInfo()<<"Please send the account number:";
    // std::cin>>accountNumber;
    // AccountNumber=QString::fromStdString(accountNumber);
    qInfo()<<"Please send the transaction amount:";
    std::cin>> TransactionAmount;
    outStream<<TransactionAmount;
    socket.waitForReadyRead();
    qInfo()<<"Transaction is :"<<m_serverrespond.toBool();
}

void User::ViewAccount()
{

    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Your account money is :"<<m_serverrespond.toInt();
}

void User::GetAccNo()
{
    outStream<<m_userName;
    socket.waitForReadyRead();
    qInfo()<<"Your account number is:"<<m_serverrespond.toString();
}

void User::ViewTransactionHistory()
{
    qInfo()<<"Please send the number of transactions:";
    quint16 count;
    std::cin>>count;
    outStream<<count;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo().noquote()<<m_serverrespond.toString();
}

void User::sendrequesttoserver()
{
    outStream<<m_requestflag;
    outStream<<m_request<<m_role;
}

bool User::Login()
{
    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    m_request="Login";
    outStream<<m_requestflag;
    outStream<<m_request<<m_role;
    QString password;
    std::string name;
    std::cin>>name;
    m_userName=QString::fromStdString(name);
    qInfo()<<"Password: ";
    std::string pass;
    std::cin>>pass;
    password=QString::fromStdString(pass);
    bool ok =false;
    quint8 count =0;
    while(count<3)
    {
        count++;
        if(!m_userName.isEmpty()&&!password.isEmpty())
        {
            outStream<<m_userName<<password;
            socket.waitForBytesWritten();
            socket.waitForReadyRead();
            ok=m_serverrespond.toBool();
            break;
        }
    }
    clearScreen();
    return ok;
}

void User::Start(bool& islogged)
{
    std::string st_input;
    char in;
    qInfo()<<"Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Transfer Account\n5-MakeTransaction\n6-exit";
    std::cin>>st_input;
    QString input=QString::fromStdString(st_input);
    std::cin.ignore();
    clearScreen();
    switch(input.toUInt())
    {
    case 1:
        m_request="View Account";
        sendrequesttoserver();
        ViewAccount();
        break;
    case 2:
        m_request="View Transaction History";
        sendrequesttoserver();
        ViewTransactionHistory();
        break;
    case 3:
        m_request="GetAccNo";
        sendrequesttoserver();
        GetAccNo();
        break;
    case 4:
        m_request="Transfer Account";
        sendrequesttoserver();
        TransferAccount();
        break;
    case 5:
        m_request="Make Transaction";
        sendrequesttoserver();
        MakeTransaction();
        break;
    case 6:
        islogged = false;
        break;
    default:
        qInfo()<<"Your choice is invalid";
        break;
    }
    if (input.toUInt() == 1||
        input.toUInt() == 2||
        input.toUInt() == 3||
        input.toUInt() == 4||
        input.toUInt() == 5)
    {
    qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
    std::cin>>in;
    if(in=='n'||in=='N')
    {
        islogged=false;
    }
     qInfo()<<"----------------------------------------------------------------------------";
    }

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
    if (m_request=="View Account")
    {
        quint32 AccountMoney;
        //take the money in the account from the server and save it in the server respond
        inStream>>AccountMoney;
        m_serverrespond.setValue(AccountMoney);
    }
    else if(m_request=="Transfer Account"||m_request=="Make Transaction"||m_request=="Login")
    {
        bool respond;
        //know if the transaction succeed or not from the server and save it in the server respond
        inStream>>respond;
        m_serverrespond.setValue(respond);
    }
    else if(m_request=="GetAccNo")
    {
        QString AccNo;
        inStream>>AccNo;
        m_serverrespond.setValue(AccNo);
    }
    else if(m_request=="View Transaction History")
    {
        QString History;
        inStream>>History;
        m_serverrespond.setValue(History);
    }
}


