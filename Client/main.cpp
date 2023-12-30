#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include "client.h"
#include<iostream>
//#include "user.h"

void clearScreen()
{
    // ANSI escape code to clear the screen and move the cursor to the top-left corner
    qDebug() << "\033[1;1H\033[2J";
}
void login(QString &username,QString &password)
{
    QTextStream qin(stdin);
    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    username = qin.readLine();
    qInfo()<<"Password: ";
    password = qin.readLine();
    clearScreen();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client client;
    client.connectToHost("127.0.0.1", 1234);
    QString role,username,password;
    qInfo()<<"Welcome to the bank system \nPlease choose \"user\"or\"admin\"";
    QTextStream qin(stdin);
    role = qin.readLine();
    client.setRole(role);
    clearScreen();


     if(role.toUpper()=="USER")
     {
            login(username,password);
            qInfo()<<"welcome user: "<<username;
            QString request;
            int input;

            qInfo()<<"Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Transfer Account";
            std::cin>>input;
            clearScreen();
            switch(input)
             {
            case 1:
                request="View Account";
                client.sendrequesttoserver(request);
                break;
            case 4:
                   // qInfo()<<in;
                request="Transfer Account";
                client.sendrequesttoserver(request);
                break;
            }

     }
     else if(role.toUpper()=="ADMIN")
     {
            login(username,password);
            qInfo()<<"welcome admin: "<<username;
     }
     else
     {
         qInfo()<<"Sorry you're entering invalid input";
     }



    return a.exec();
}
