#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMetaEnum>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void Login(QString username,QString password);
    // admins only requests:
    void ViewBankDatabase();
    void CreateNewUser(QString userdata);
    void Deleteuser(QString accountnumber);
    void UpdateUser(QString accountnumber,QString newdata);
    //users only requests:
    void TransferAccount();
    void MakeTransaction();
    //users and admins requests:
    void GetAccNo();
    void ViewAccount();
    void ViewTransactionHistory();
    // method to send the client request to the server
    void sendrequesttoserver(QString request);
    QString getRole() const;
    void setRole(const QString &newRole);

signals:
public slots:
    void connectToHost(QString host,quint16 port);
    void disconnect();
protected slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketerror);
    void stateChanged(QAbstractSocket::SocketState socketstate);
    void readyRead();
 private:
    QTcpSocket socket;
    QString role;
    QString userrequest;
    QString adminrequest;
    QVariant serverrespond;

};

#endif // CLIENT_H
