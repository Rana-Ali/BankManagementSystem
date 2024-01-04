#include "admin.h"
#include <iostream>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
Admin::Admin()
{
    outStream.setDevice(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    inStream.setDevice(&socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    m_role="admin";
    connect(&socket,&QTcpSocket::connected,this,&Admin::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&Admin::disconnected);
    connect(&socket,&QTcpSocket::stateChanged,this,&Admin::stateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&Admin::readyRead);
    connect(&socket,&QTcpSocket::errorOccurred,this,&Admin::error);
}

void Admin::ViewAccount()
{
    std::string accountNumber;
    QString AccountNumber;

    //quint16 input;
    //std::cin>>input;
    //std::cin.ignore();
    clearScreen();

        qInfo()<<"Please Enter the account number";
        std::cin>>accountNumber;
        AccountNumber=QString::fromStdString(accountNumber);
        outStream<<AccountNumber;
        socket.waitForBytesWritten();
        socket.waitForReadyRead();
        qInfo()<<"Admin account balance is :"<<m_serverrespond.toInt();

}

void Admin::GetAccNo()
{
    qInfo()<<"Please Enter the username:";
    std::string username;
    std::cin>>username;
    QString UserName=QString::fromStdString(username);
    outStream<<UserName;
    socket.waitForBytesWritten();
    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    qInfo()<<"Account Number is :"<<m_serverrespond.toString();

}

void Admin::ViewBankDatabase()
{
    socket.waitForReadyRead();
    qInfo()<<m_serverrespond.toString();
}

void Admin::CreateNewUser()
{
    QVariantMap map;
    std::string username,fullname;
    quint8 age=0;
    quint8 counter=0;
    qint32 balance=0;
    bool ok =false;

   do
    {
        if(counter>0)
       {
           qInfo()<<"User name is already existed!!\nPlease Enter a different one:";
           std::cin>>username;
           QString Username =QString::fromStdString(username);
           outStream<<Username;
       }
        else
        {
            qInfo()<<"Username:";
            std::cin>>username;
            QString Username =QString::fromStdString(username);
            outStream<<Username;
        }

    counter++;
    socket.waitForReadyRead();
    ok =m_serverrespond.toBool();
    }while(!ok);

        qInfo()<<"Full Name:";
        std::cin>>fullname;
        qInfo()<<"Age:";
        std::cin>>age;
        qInfo()<<"Balance:";
        std::cin>>balance;

    map["Full Name"]=QString::fromStdString(fullname);
    map["Age"]=age;
    map["Balance"]=balance;
    outStream<<map;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    if(m_serverrespond.toBool()==true)
    {
        qInfo()<<"User is created Successfully";
    }
    else
    {
        qInfo()<<"User is not created!!";
    }

}

void Admin::Deleteuser()
{
    qInfo()<<"Please send the account number:";
    std::string accountNumber;
    std::cin>>accountNumber;
    QString AccountNumber=QString::fromStdString(accountNumber);
    outStream<<AccountNumber;
    socket.waitForBytesWritten();
    //wait for the respond from the server to view it to the client
    socket.waitForReadyRead();
    if(m_serverrespond.toBool()==true)
    {
        qInfo()<<"User is deleted";
    }
    else
    {
        qInfo()<<"User is not deleted!!";
    }

}

void Admin::UpdateUser()
{
    QVariantMap map;
    QString flag="check";
    std::string accountNumber,fullname,password;
    quint8 age=0;
    quint8 in=0;
    char key='y';
    quint8 counter=0;
    qint32 balance=0;
    bool ok =false;


    do
    {
        if(counter>0)
        {
            qInfo()<<"Account Number is not existed!!\nplease Enter a valid one:";
            std::cin>>accountNumber;
            QString AccountNumber=QString::fromStdString(accountNumber);
            outStream<<flag;
            outStream<<AccountNumber;
        }
        else
        {
            qInfo()<<"Account Number:";
            std::cin>>accountNumber;
            QString AccountNumber=QString::fromStdString(accountNumber);
            outStream<<flag;
            outStream<<AccountNumber;
        }
        counter++;
        socket.waitForReadyRead();
        ok =m_serverrespond.toBool();
    }while(!ok);
    flag="update";
    do
    {
        qInfo()<<"Choose the field you want to update:";
        qInfo()<<"1-Full Name\n2-Age\n3-Balance\n4-Password";
        std::cin>>in;
        switch (in) {
        case 1:
            qInfo()<<"New Full Name:";
            std::cin>>fullname;
            map["Full Name"]=QString::fromStdString(fullname);
            break;
        case 2:
            qInfo()<<"New Age:";
            std::cin>>age;
            map["Age"]=age;
            break;
        case 3:
            qInfo()<<"New Balance:";
            std::cin>>balance;
            map["Balance"]=balance;
            break;
        case 4:
            qInfo()<<"New Password:";
            std::cin>>password;
            map["Password"]=QString::fromStdString(password);
            break;
        default:
            qInfo()<<"Please enter a valid choice";
            break;
        }
        qInfo()<<"----------------------------------------------------------";
        qInfo()<<"If you want to update another field press 'y'\nIf you want to save and exit press'n'";
        std::cin>>key;
    }
    while((key != 'n')||(key != 'N'));
    outStream<<flag;
    outStream<<map;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    if(m_serverrespond.toBool()==true)
    {
        qInfo()<<"User Data is updated Successfully";
    }
    else
    {
        qInfo()<<"User Data is not updated!!";
    }

}

void Admin::ViewTransactionHistory()
{
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

void Admin::sendrequesttoserver()
{
    outStream<<m_request<<m_role;

    if(m_request=="View Bank DataBase")
    {
        //call the method View Bank DataBase to handle this request
        ViewBankDatabase();
    }
    else if(m_request=="View Account")
    {
        //call the method view Account to handle this request
        ViewAccount();
    }
    else if(m_request=="Create New User")
    {
        CreateNewUser();
    }
    else if(m_request=="Get Acc No")
    {
        GetAccNo();
    }
    else if(m_request=="View Transaction History")
    {
        ViewTransactionHistory();
    }
    else if(m_request=="Update User")
    {
        UpdateUser();
    }
    else if(m_request=="Delete User")
    {
        Deleteuser();
    }
    else if(m_request=="Login")
    {
        Login();

    }
    else
    {
        qFatal("The request message is not defined");
    }
}

bool Admin::Login()
{
    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    m_request="Login";
    outStream<<m_request<<m_role;
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
            outStream<<adminname<<password;
            socket.waitForBytesWritten();
            socket.waitForReadyRead();
            ok=m_serverrespond.toBool();
            m_adminname=adminname;
            break;
        }
    }
    clearScreen();
    return ok;
}

void Admin::Start(bool &islogged)
{
    quint16 input;
    char in;
    qInfo()<<"Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Create New User"
               "\n5-Update User Data\n6-Delete User\n7-View Bank DataBase\n8-exit";
    std::cin>>input;
    std::cin.ignore();
    clearScreen();
    switch(input)
    {
    case 1:
        m_request="View Account";
        break;
    case 2:
        m_request="View Transaction History";
        break;
    case 3:
        m_request="Get Account Number";
        break;
    case 4:
        m_request="Create New User";
        break;
    case 5:
        m_request="Update User Data";
        break;
    case 6:
        m_request="Delete User";
        break;
    case 7:
        m_request="View Bank DataBase";
        break;
    case 8:
        islogged=false;
        break;
    default:
        qInfo()<<"Your choice is invalid";
        break;
    }
    if (input != 8)
    {
        sendrequesttoserver();
        qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
        std::cin>>in;
        if(in=='n'||in=='N')
        {
            islogged=false;
        }
        qInfo()<<"----------------------------------------------------------------------------";
    }

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

    if(m_request=="View Bank DataBase")
    {
        QString Database;
        inStream>>Database;
        m_serverrespond.setValue(Database);

    }
    else if(m_request=="View Account")
    {
        quint32 AccountMoney;
        //take the money in the account from the server and save it in the server respond
        inStream>>AccountMoney;
        m_serverrespond.setValue(AccountMoney);
    }
    else if(m_request=="Create New User"||(m_request=="Delete User")||(m_request=="Update User Data")||(m_request=="Login"))
    {
        bool respond;
        inStream>>respond;
        m_serverrespond.setValue(respond);

    }
    else if(m_request=="Get Account Number")
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



