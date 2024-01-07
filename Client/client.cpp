/**************Client Abstraction class**********************************************/
/**********Author: Rana Ali ********************************************************/
/********************Ver.: 01******************************************************/
/****************Date:1/01/2024.**************************************************/
/********************************************************************************/
#include "client.h"
#include <QTextStream>
#include<QDataStream>
#include<QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
{
    m_requestflag="General";
}

void Client::clearScreen()
{
    system("cls");
}




