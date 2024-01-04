#include <QCoreApplication>
#include <QDebug>
#include "user.h"
#include "admin.h"
#include<iostream>




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString role;

    qInfo()<<"Welcome to the bank system \nPlease choose \"user\"or\"admin\"or\"exit\"";
    std::string clientrole;
    std::cin>>clientrole;
    role = QString::fromStdString(clientrole);


     if(role.toUpper()=="USER")
     {
        User user;
        user.connectToHost("127.0.0.1", 1234);
        user.clearScreen();
        bool islogged =user.Login();
        user.clearScreen();
        qInfo()<<"welcome ";
            while(islogged)
            {
            user.Start(islogged);
            }
     }
     else if(role.toUpper()=="ADMIN")
     {
         Admin admin;
         admin.connectToHost("127.0.0.1", 1234);
         bool islogged= admin.Login();
         admin.clearScreen();
         qInfo()<<"welcome ";
         while(islogged)
         {
             admin.Start(islogged);
         }
     }
     else if(role.toUpper()=="EXIT")
     {
         qInfo()<<"Thank you!....Sysyem is closing..";
     }
     else
     {
         qInfo()<<"Sorry you're entering invalid input";
     }

     qInfo()<<"BYE...BYE";
     exit(0);
     return a.exec();
}
