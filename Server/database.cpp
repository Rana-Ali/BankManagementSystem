/***********Dtabase Class**********************************************/
/**********Author: Rana Ali ******************************************/
/********************Ver.: 01*****************************************/
/****************Date:3/01/2024.*************************************/
/*******************************************************************/

#include "database.h"
#include <QDateTime>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
   Users_DB.setFileName("D:/qt projects/DB.json");
   Login_DB.setFileName("D:/qt projects/Login.json");
}

/******************************************************************/
/* Func Name:  checkLogin                                         */
/* **********************************************************     */
/* Input Par  : QString username,QString password                 */
/* Description: the username and password for each client         */
/* Return Val : bool                                              */
/* Description: check if username and password are in login DB    */
/* Description: used to check credentials                         */
/*                                                                */
/* ****************************************************************/
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
/******************************************************************/
/* Func Name:  ViewAccountBalance                                 */
/* **********************************************************     */
/* Input Par  : QString accountNumber                             */
/* Description: the accountnumber of the user                     */
/* Return Val : qint32                                            */
/* Description: get the balance field from usersDB file           */
/* Description: return the money to the user                      */
/*                                                                */
/* ****************************************************************/

qint32 DataBase::ViewAccountBalance(QString accountNumber)
{
    QString AccountBalance;
    bool ok;
    ok=GetField(accountNumber,"Balance",AccountBalance);
    if (ok)
    {
        return AccountBalance.toUInt();
    }
    else
        return -1;

}
/******************************************************************/
/* Func Name:  GetAccNo                                           */
/* **********************************************************     */
/* Input Par  : QString username                                  */
/* Description: the username of the user                          */
/* Return Val : QString                                           */
/* Description: get the account number from the username          */
/*                                                                */
/*                                                                */
/* ****************************************************************/

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
/****************************************************************************************/
/* Func Name:  TransferAmount                                                           */
/* **********************************************************                           */
/* Input Par  : QString toAccNo ,quint32 transferAmount                                 */
/* Description: the accountnumber of the user to which the amount will be transfered    */
/* Description: the amount of money to be transfered                                    */
/* Return Val : bool                                                                    */
/* Description: if the transaction failed or succeed                                    */
/*                                                                                       */
/*                                                                                       */
/* ************************************************************************************* */
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
/*********************************************************************/
/* Func Name:  GetField                                              */
/* **********************************************************        */
/* Input Par  : QString accountnumber                                 */
/* Description: the account number of the user                         */
/* Input Par  : QString field                                         */
/* Description: the field name                                        */
/* Input Par  : QString& fieldValue                                   */
/* Description: the variable to save the value in                     */
/* Return Val : bool                                                  */
/* Description: the field is existed and got successfully or not      */
/* ********************************************************************/
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
/*********************************************************************/
/* Func Name:  update field                                          */
/* **********************************************************        */
/* Input Par  : QString accountnumber                                 */
/* Description: the account number of the user                         */
/* Input Par  : QString field                                         */
/* Description: the field name                                        */
/* Input Par  : QString fieldValue                                   */
/* Description: the new value of the field                           */
/* Return Val : bool                                                  */
/* Description: the field is updated successfully or not              */
/* ********************************************************************/

bool DataBase::UpdateField(QString accountnumber, QString field, QString fieldValue)
{
    QMutexLocker locker(&mutex);

    //update fields  username or account number
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
        Users_DB.close();
        return false;
    }
    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        Users_DB.close();
        return false;
    }
    QJsonObject m_Users = jsonDoc.object();
    if(field=="password")
    {
        QString username;
        if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
        {
             QJsonObject userObject = m_Users[accountnumber].toObject();
             username=userObject["Username"].toString();
        }
        if (!Login_DB.open(QIODevice::ReadWrite| QIODevice::Text)) {
            qDebug() << "Error: Can't open the Login_DB file";
            Users_DB.close();
            return false;
        }
        QByteArray loginData = Login_DB.readAll();
        QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);

        // Check if the document is an object
        if (loginDoc.isObject()) {
            // Get the existing users array
            QJsonObject m_login = loginDoc.object();
            QJsonArray usersArray = m_login["users"].toArray();
            for(int i=0;i<usersArray.size();++i)
            {
                if (!usersArray[i].isObject()) {
                    qDebug() << "Error: User entry is not an object";
                    Users_DB.close();
                    Login_DB.close();
                    return false;
                }

                QJsonObject userObject = usersArray[i].toObject();
                if(userObject.contains(username))
                {
                    QJsonObject user=userObject[username].toObject();
                    user["password"]=fieldValue;
                    userObject[username]=user;
                    usersArray.replace(i,userObject);
                   // Update the JSON document with the modified users array
                   m_login["users"]=usersArray;
                   // Write the updated m_login to Login_DB
                   Login_DB.seek(0);
                   Login_DB.write(QJsonDocument(m_login).toJson(QJsonDocument::Indented));
                   Login_DB.close();
                   Users_DB.close();
                   return true;

            }


        }
    }
}

    else
    {

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
    }
       Users_DB.close();
        return false;
}
/*********************************************************************/
/* Func Name:  MakeTransaction                                             */
/* **********************************************************        */
/* Input Par  : qint32 TransactionAmount                                */
/* Description: the transaction amount                                */
/* Return Val : bool                                                  */
/* Description: the transaction is done successfully or not           */
/* ********************************************************************/

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
/****************************************************************************************/
/* Func Name:  SaveTransaction                                                          */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user in which the transaction will be saved    */
/* Input Par  : QString &TransactionDetails                                             */
/* Description: the details of the transaction to be saved (date and type)              */
/* Return Val : void                                                                    */
/* Description: N/A                                                                      */
/*                                                                                       */
/*                                                                                       */
/* ************************************************************************************* */

void DataBase::SaveTransaction( QString accountnumber,QString &TransactionDetails)
{
    QMutexLocker locker(&mutex);
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
/****************************************************************************************/
/* Func Name:  ViewTransactionHistory                                                         */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user in which the transaction will be viewed   */
/* Input Par  : quint16 count                                                          */
/* Description: the number of transactions to be viewed                                 */
/* Return Val : QString                                                                   */
/* Description: Transaction History of the account number                                */
/* ************************************************************************************* */
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
/****************************************************************************************/
/* Func Name:  Check Acc No                                                        */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user to check if its exist in DB or not       */
/* Return Val : bool                                                                  */
/* Description: if the account number exist or not                                    */
/* ************************************************************************************* */
bool DataBase::checkAccNo(QString accountnumber)
{
    // Load JSON file
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }

    // Read JSON data
    QByteArray jsonData1 = Users_DB.readAll();
    Users_DB.close();

    // Parse JSON
    QJsonDocument jsonDoc1 = QJsonDocument::fromJson(jsonData1);
    if (jsonDoc1.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from check";
        return false;
    }

    // Check if the root element is an object
    if (!jsonDoc1.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        return false;
    }
    QJsonObject usersObject = jsonDoc1.object();

    if(usersObject.contains(accountnumber)&&usersObject[accountnumber].isObject())
    {
        return true;
    }

    return false;
}
/****************************************************************************************/
/* Func Name:  Check field                                                       */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user for which we search for a field in DB     */
/* Input Par  : QString field                                                           */
/* Description: the field to search about if its existed or not                         */
/* Return Val : bool                                                                     */
/* Description: if field exists or not in DB                                             */
/* ************************************************************************************* */

bool DataBase::checkField(QString accountnumber, QString field)
{
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
    }
    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();
    Users_DB.close();
    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject usersObject = jsonDoc.object();
    QJsonObject  userObject = usersObject[accountnumber].toObject();
    if(userObject.contains(field))
        return true;
    return false;

}
/****************************************************************************************/
/* Func Name:  Update user                                                       */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user in which we will update the fields in DB   */
/* Input Par  :  QVariantMap map                                                        */
/* Description: the fieldS to be changed and their new values                           */
/* Return Val : bool                                                                     */
/* Description: if user updated or not                                                    */
/* ************************************************************************************* */
bool DataBase::UpdateUser(QString accountnumber, QVariantMap map)
{
    // bool ok=checkAccNo(accountnumber);
    // if(!ok)
    // {
    //     return false;
    // }
    bool ok=false;
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
    }
    // Read JSON data
    QByteArray jsonData = Users_DB.readAll();
    Users_DB.close();
    // Parse JSON
    qInfo()<<"okkkk";
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject usersObject = jsonDoc.object();
    QJsonObject  userObject = usersObject[accountnumber].toObject();
    foreach (QString key, map.keys()) {
        if(key != "password")
        {
            qInfo()<<"not pass";
            ok =checkField(accountnumber,key);

        }
        else
        {
            ok=true;
        }

        if(ok)
        {
            QString val=map[key].toString();
            qInfo()<<val;
            ok=UpdateField(accountnumber,key,val);
        }
    }
    return ok;
}
/****************************************************************************************/
/* Func Name:  Create user                                                             */
/* **********************************************************                           */
/* Input Par  : QString username                                                        */
/* Description: the username of the new user                                            */
/* Input Par  : QString password                                                        */
/* Description: the password of the new user                                            */
/* Input Par  :  QVariantMap map                                                        */
/* Description: the fieldS to be ecreated and their  valuesn                             */
/* Return Val : bool                                                                     */
/* Description: if user created or not                                                    */
/* ************************************************************************************* */
bool DataBase::CreateUser(QString username,QString password,QVariantMap map)
{
    QMutexLocker locker(&mutex);

    //create user object in Users_DB file
    /***************************************************************************/
    if (!Users_DB.open(QIODevice::ReadWrite| QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }
    QByteArray jsonData2 =Users_DB.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData2);
    if (jsonDoc.isNull()) {
        qDebug()<<"hello";
        qDebug() << "Error: Failed to parse JSON data from update";
        Users_DB.close();
        return false;
    }
    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        Users_DB.close();
        return false;
    }
    QJsonObject usersObject  = jsonDoc.object();

    QString accountnumber =GenerateAccNo(username);
    QJsonObject userObject;
    userObject["Accountnumber"]=accountnumber;
    foreach (QString key, map.keys()) {
        userObject[key]=map[key].toString();
    }
    usersObject[accountnumber]=userObject;
    jsonDoc.setObject(usersObject);
    Users_DB.seek(0);
    Users_DB.write(jsonDoc.toJson(QJsonDocument::Indented));
    Users_DB.close();
    /*****************************************************************************************/
    //create user object in Login_DB file
    // Open the Login_DB file for writing
    if (!Login_DB.open(QIODevice::ReadWrite| QIODevice::Text)) {
        qDebug() << "Error: Can't open the Login_DB file";
        return false;
    }
    QByteArray loginData = Login_DB.readAll();
    QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);

    // Check if the document is an object
    if (loginDoc.isObject()) {
        // Get the existing users array
        QJsonObject m_login = loginDoc.object();
        QJsonArray usersArray = m_login["users"].toArray();

        // Create a new user object for Login_DB
        QJsonObject newUser;
        newUser["accountnumber"] = accountnumber;
        newUser["password"] = password;

        // Wrap the new user object in an array element
        QJsonObject newUserArrayElement;
        newUserArrayElement[username] = newUser;

        // Add the new user object to the users array
        usersArray.append(newUserArrayElement);

        // Update the JSON document with the modified users array
        m_login["users"]=usersArray;


        // Write the updated m_login to Login_DB
        Login_DB.seek(0);
        Login_DB.write(QJsonDocument(m_login).toJson(QJsonDocument::Indented));
    }
    else {
        qDebug() << "Error: Invalid JSON data in Login_DB";
        Login_DB.close();
        return false;
    }

    Login_DB.close();

    return true;

}
/****************************************************************************************/
/* Func Name:  Check Username                                                        */
/* **********************************************************                           */
/* Input Par  : QString username                                                   */
/* Description: the username of the user to check if its exist in DB or not       */
/* Return Val : bool                                                                  */
/* Description: if the username exist or not                                    */
/* ************************************************************************************* */
bool DataBase::checkUsername(QString username)
{
    if (!Login_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the Login_DB file";
        return false;
    }

    QByteArray jsonData = Login_DB.readAll();
    Login_DB.close();
    qInfo() << "Checking username";

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Error: Failed to parse JSON data in Login_DB";
        return false;
    }

    QJsonObject m_login = jsonDoc.object();
    if (m_login.contains("users") && m_login["users"].isArray()) {
        QJsonArray usersArray = m_login["users"].toArray();

        for (const QJsonValue &userVal : usersArray) {
            if (!userVal.isObject()) {
                qDebug() << "Error: User entry is not an object";
                return false;
            }

            QJsonObject userObject = userVal.toObject();
            if (userObject.contains(username)) {
                qInfo() << "Username already exists";
                return false;  // Username found in the database
            }
        }
    } else {
        qDebug() << "Error: 'users' key not found or not an array in Login_DB";
        return false;
    }

    qInfo() << "Username is available";
    return true;  // Username is not found in the database
}
/****************************************************************************************/
/* Func Name:  Delete user                                                             */
/* **********************************************************                           */
/* Input Par  : QString accountnumber                                                   */
/* Description: the accountnumber of the user to be deleted from DB       */
/* Return Val : bool                                                                  */
/* Description: if the user deleted or not                                    */
/* ************************************************************************************* */
bool DataBase::DeleteUser(QString accountnumber)
{
    QMutexLocker locker(&mutex);
    //check if account number exist
    bool ok=checkAccNo(accountnumber);
    if(!ok)
    {
        return ok;
    }
    // if account number exist , then get the username to be able to delete user from login DB
    QString username;
    ok=GetField(accountnumber,"Username",username);
    if(!ok)
    {
        qDebug()<<"couldn't get the username";
    }
    // username is now here so we will delete the user first from login DB
    if (!Login_DB.open(QIODevice::ReadWrite| QIODevice::Text)) {
        qDebug() << "Error: Can't open the Login_DB file";
        return false;
    }
    QByteArray loginData = Login_DB.readAll();
    QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);

    // Check if the document is an object
    if (loginDoc.isObject()) {
        // Get the existing users array
        QJsonObject m_login = loginDoc.object();
        QJsonArray usersArray = m_login["users"].toArray();
        for(int i=0;i<usersArray.size();++i)
        {
            if (!usersArray[i].isObject()) {
                qDebug() << "Error: User entry is not an object";
                Login_DB.close();
                return false;
            }

            QJsonObject userObject = usersArray[i].toObject();
            if(userObject.contains(username))
            {
                usersArray.removeAt(i);
                // Update the JSON document with the modified users array
                m_login["users"]=usersArray;
                // Write the updated m_login to Login_DB
                break;
            }

        }
        Login_DB.seek(0);
        Login_DB.resize(0);
        Login_DB.write(QJsonDocument(m_login).toJson(QJsonDocument::Indented));
        Login_DB.close();
    }
    else
    {
        qDebug()<<"loginDoc is not object";
        Login_DB.close();
        return false;
    }
    // then delete the user from Users DB
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
        Users_DB.close();
        return false;
    }
    // Check if the root element is an object
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        Users_DB.close();
        return false;
    }
    QJsonObject m_Users = jsonDoc.object();
    m_Users.remove(accountnumber);
    Users_DB.seek(0);
    Users_DB.resize(0); // Truncate the file
    Users_DB.write(QJsonDocument(m_Users).toJson(QJsonDocument::Indented));
    Users_DB.close();
    return true;

}
/****************************************************************************************/
/* Func Name:  ViewBankDataBase                                                       */
/* **********************************************************                           */
/* Input Par  : N/A                                                  */
/* Description: N/A                                                                     */
/* Return Val : QStirng                                                                 */
/* Description: the users database                                                       */
/* ************************************************************************************* */
QString DataBase::ViewBankDataBase()
{
    if (!Users_DB.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the bank database file";
        return QString();  // Return an empty string on error
    }

    QByteArray jsonData = Users_DB.readAll();
    Users_DB.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Error: Invalid JSON data in the bank database file";
        return QString();  // Return an empty string on error
    }

    QJsonObject bankObject = jsonDoc.object();

    // Assuming users are stored as objects with usernames as keys
    QJsonArray bankArray;
    for (const QString& accountnumber : bankObject.keys()) {
        QJsonObject userObject = bankObject[accountnumber].toObject();
        bankArray.append(userObject);
    }

    return QJsonDocument(bankArray).toJson(QJsonDocument::Indented);
}
/****************************************************************************************/
/* Func Name:  GenerateAccNo                                                     */
/* **********************************************************                           */
/* Input Par  :  QString& username                                                 */
/* Description: unique username to generate unique account number                       */
/* Return Val : QStirng                                                                 */
/* Description: the new user accountnumber                                                      */
/* ************************************************************************************* */
QString DataBase::GenerateAccNo(QString& username)
{
    qulonglong accno =qHash(username);
    return QString::number(accno);

}

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


