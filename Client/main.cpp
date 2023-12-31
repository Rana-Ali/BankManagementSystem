#include <QCoreApplication>
#include <QDebug>
#include "user.h"
#include "admin.h"
#include<iostream>
#include <windows.h>

void clearScreen()
{
    system("cls");

}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString role;

    qInfo()<<"Welcome to the bank system \nPlease choose \"user\"or\"admin\"or\"exit\"";
    std::string clientrole;
    std::cin>>clientrole;
    role = QString::fromStdString(clientrole);
    //connectToHost(QHostAddress::LocalHost, 1234);
    clearScreen();

     if(role.toUpper()=="USER")
     {
        User user;
        user.connectToHost("127.0.0.1", 1234);
        bool islogged =user.Login();
        quint16 input;
        qInfo()<<"welcome ";
        QString request;
        char in;
            while(islogged)
            {
                 qInfo()<<"Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Transfer Account\n5-MakeTransaction\n6-exit";
                 std::cin>>input;
                 std::cin.ignore();
                 clearScreen();


                  switch(input)
                   {
                     case 1:
                     request="View Account";
                     user.sendrequesttoserver(request);
                     qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
                     std::cin>>in;
                      if(in=='n'||in=='N')
                       {
                        islogged=false;
                       }
                        break;

                      case 4:

                         request="Transfer Account";
                         user.sendrequesttoserver(request);
                         qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
                         std::cin>>in;
                         if(in=='n'||in=='N')
                         {
                             islogged=false;
                         }
                         break;
                     case 6:

                         islogged = false;
                         break;


                     default:
                         qInfo()<<"default";
                         break;
                     }

                     clearScreen();

            }
     }
     else if(role.toUpper()=="ADMIN")
     {
         Admin admin;
         admin.connectToHost("127.0.0.1", 1234);
         bool islogged= admin.Login();
         while(islogged)
         {
             qInfo()<<"welcome admin: ";
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
     exit(122);
     return a.exec();
}
