#include "database.h"
#include <QDateTime>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
   Users_DB.setFileName("D:/qt projects/DB.json");
   Login_DB.setFileName("D:/qt projects/Login.json");
}

bool DataBase::checkLogin(QString username, QString password)
{
    // Load JSON file
    if (!Login_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }

    // Read JSON data
    QByteArray jsonData = Login_DB.readAll();
    Login_DB.close();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data";
        return false;
    }

    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        return false;
    }


     m_login = jsonDoc.object();
    if(m_role.toUpper()=="USER")
    {
        // Check if the username exists in the JSON object
        if(m_login.contains("users")&&m_login["users"].isArray())
        {
            QJsonArray usersArray = m_login["users"].toArray();
            for (const QJsonValue &userVal : usersArray)
            {
                if(!userVal.isObject())
                    return false;
                QJsonObject userObject = userVal.toObject();
                if(userObject.contains(username))
                {
                    QJsonObject userData = userObject[username].toObject();
                    if (userData.contains("password") && userData["password"] .toString() == password)
                    {
                        qDebug() << "User authenticated: " <<username;
                        m_username=username;
                        m_accountnumber=userData["accountnumber"].toString();
                        //checkDataBase();
                        return true;  // Authentication successful
                    }

                }

            }

        }

    }
    else if(m_role.toUpper()=="ADMIN")
    {
        // Check if the username exists in the JSON object
        if(m_login.contains("admins")&&m_login["admins"].isArray())
        {
            QJsonArray usersArray = m_login["admins"].toArray();
            for (const QJsonValue &userVal : usersArray)
            {
                if(!userVal.isObject())
                    return false;
                QJsonObject userObject = userVal.toObject();
                if(userObject.contains(username))
                {
                    QJsonObject userData = userObject[username].toObject();
                    if (userData.contains("password") && userData["password"] .toString() == password)
                    {
                        qDebug() << "User authenticated: " <<username;
                        m_username=username;
                        return true;  // Authentication successful
                    }

                }

            }

        }
    }
    else
    {
        return false;
    }
    return false;  // Authentication failed

}

qint32 DataBase::ViewAccountBalance(QString accountNumber)
{
    QString AccountBalance;
    bool ok;
    ok=GetField(accountNumber,"Balance",AccountBalance);
    if (ok)
    {
        return AccountBalance.toInt();
    }
    else
        return -1;

}

QString DataBase::GetAccNo(QString username)
{
    QString error ="Username is not found";

    QJsonArray usersArray = m_login["users"].toArray();
        for (const QJsonValue &userVal : usersArray)
        {
            QJsonObject userObject = userVal.toObject();
            if(userObject.contains(username))
            {QJsonObject userData = userObject[username].toObject();
                return (userData["accountnumber"].toString());
            }
        }

        return error;
}

bool DataBase::TransferAmount( QString toAccNo, quint32 transferAmount)
{
    QString mybalance,tobalance;
    quint32 Mybalance,Tobalance;
    bool ok =GetField(m_accountnumber,"Balance",mybalance);
    if(ok)
    {
        Mybalance=mybalance.toUInt();
        if(Mybalance==0 && Mybalance < transferAmount)
        {
            qDebug()<<"Sorry ,Your balance is not enough";
            return false;
        }

        Mybalance -= transferAmount;
        ok=UpdateField(m_accountnumber,"Balance",QString::number(Mybalance));
        if(!ok)
        {
            return false;
        }
    }
    ok=GetField(toAccNo,"Balance",tobalance);
    if(ok)
    {
        Tobalance=tobalance.toUInt();
        Tobalance += transferAmount;
        ok=UpdateField(toAccNo,"Balance",QString::number(Tobalance));
        QString details_from = QString::number(transferAmount)+" to "+toAccNo;
        QString details_to = QString::number(transferAmount)+" from "+m_accountnumber;
        SaveTransaction(m_accountnumber,details_from);
        SaveTransaction(toAccNo,details_to);
        return ok;
    }
    return false;
}

bool DataBase::GetField(QString accountnumber, QString field, QString& fieldValue)
{
    // Load JSON file
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }

    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();
    Users_DB.close();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from get";
        return false;
    }

    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        return false;
    }


    QJsonObject m_Users = jsonDoc.object();

    if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
    {
        QJsonObject userObject = m_Users[accountnumber].toObject();
        fieldValue=userObject[field].toString();
        return true;  // Authentication successful
    }
    return false;

}

bool DataBase::UpdateField(QString accountnumber, QString field, QString fieldValue)
{
    // Load JSON file
    if (!Users_DB.open(QIODevice::ReadWrite | QIODevice::Text )) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }
    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from update";
        return false;
    }
    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        return false;
    }
    QJsonObject m_Users = jsonDoc.object();

    if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
    {
        QJsonObject userObject = m_Users[accountnumber].toObject();
        userObject[field]=fieldValue;
        m_Users[accountnumber] = userObject;
        Users_DB.seek(0);
        Users_DB.write(QJsonDocument(m_Users).toJson());
        Users_DB.close();
        return true;
    }
    Users_DB.close();
    return false;
}

bool DataBase::MakeTransaction( qint32 TransactionAmount)
{
    //checkDataBase();
    QString oldBalance,Balance,details;
    bool ok = GetField(m_accountnumber,"Balance",oldBalance);
    if(ok)
    {
        qint32 newBalance=oldBalance.toInt();
        newBalance += TransactionAmount;
        Balance = QString::number(newBalance);
        if(TransactionAmount>0)
        {
         details = "+"+QString::number(TransactionAmount);
        }
        else
        {
          details = QString::number(TransactionAmount);
        }
        SaveTransaction(m_accountnumber,details);
    }
    else
    {
        return false;
    }

    ok=UpdateField(m_accountnumber,"Balance",Balance);
    return ok;
}

void DataBase::SaveTransaction( QString accountnumber,QString &TransactionDetails)
{
    if (!Users_DB.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
    }

    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data ";
    }

    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
    }

    QJsonObject usersObject = jsonDoc.object();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QJsonObject transactionObject;
    transactionObject["datetime"] = dateTimeString;
    transactionObject["details"] = TransactionDetails;
    if(usersObject.contains(accountnumber)&&usersObject[accountnumber].isObject())
    {
      QJsonObject  userObject = usersObject[accountnumber].toObject();

    if (!userObject.contains("Transaction History")) {
        // If not, create a new empty array for "Transaction History"
        userObject["Transaction History"] = QJsonArray();
    }

    // Get the existing "Transaction History" array
    QJsonArray transactionHistoryArray = userObject["Transaction History"].toArray();

    // Prepend the new transaction to the front of the array
    transactionHistoryArray.prepend(transactionObject);
    userObject["Transaction History"] = transactionHistoryArray;
    usersObject[accountnumber]=userObject;
    Users_DB.seek(0);
    Users_DB.write(QJsonDocument(usersObject).toJson());
}
    Users_DB.close();
}

QString DataBase::ViewTransactionHistory(QString accountnumber,quint16 count)
{
    QString error="This account doesn't have a Transaction Hisstory";
    QString data;
    QString modifiedString;
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
    }

    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();
    Users_DB.close();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data ";
    }

    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
    }
      QJsonObject usersObject = jsonDoc.object();

    if(usersObject.contains(accountnumber)&&usersObject[accountnumber].isObject())
    {

        QJsonObject  userObject = usersObject[accountnumber].toObject();
        if(!userObject.contains("Transaction History"))
            return error;
         QJsonArray transactionHistoryArray = userObject["Transaction History"].toArray();

        for (int i = 0; i < qMin(count, static_cast<quint16>(transactionHistoryArray.size())); ++i) {
            QJsonValue userVal = transactionHistoryArray.at(i);
            QJsonObject userobj= userVal.toObject();
            QJsonDocument jsonDocument(userobj);
            data=( jsonDocument.toJson(QJsonDocument::Indented));
            QStringList lines = data.split('\n');
            for (const QString &line : lines) {
                modifiedString += line + "\n";
            }

        }
        return modifiedString;

}
    return error;
}

bool DataBase::checkField(QFile &, QString)
{

}

// void DataBase::checkDataBase()
// {
//     if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         qDebug() << "Error: Can't open the DataBase file";
//     }

//     // Read JSON data
//     QByteArray jsonData = Users_DB.readAll();
//     Users_DB.close();

//     // Parse JSON
//     QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
//     if (jsonDoc.isNull()) {
//         qDebug() << "Error: Failed to parse JSON data";
//     }

//     // Check if the root element is an object
//     if (!jsonDoc.isObject()) {
//         qDebug() << "Error: JSON data is not an object";
//     }
//   QJsonObject usersObject = jsonDoc.object();
//     if(usersObject.contains(m_accountnumber)&&usersObject[m_accountnumber].isObject())
//     {
//         userObject = usersObject[m_accountnumber].toObject();
//     }
// }

QString DataBase::role() const
{
    return m_role;
}

void DataBase::setRole(const QString &newRole)
{
    m_role = newRole;
}

QString DataBase::username() const
{
    return m_username;
}

QString DataBase::accountnumber() const
{
    return m_accountnumber;
}


