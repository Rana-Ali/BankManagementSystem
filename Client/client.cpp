#include "client.h"
#include <QTextStream>
#include<QDataStream>
#include<QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
{

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

