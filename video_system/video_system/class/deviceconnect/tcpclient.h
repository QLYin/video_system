#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "singleton.h"
#include<QTcpSocket>
#include <QVariantMap>

class TcpClient : public QObject
{
    Q_OBJECT SINGLETON_DECL(TcpClient)

public:
    explicit TcpClient(QObject *parent = 0);

public:
    bool init();
    bool uninit();
    void connectServer();

public:
    void readData();
    void sendData(const QString& data);
    void displayError(QAbstractSocket::SocketError err);
    bool isConnnected();
    void onConnected();

    Q_SIGNAL void socketError(QAbstractSocket::SocketError err);
    Q_SIGNAL void socketData(const QVariantMap data);
    Q_SIGNAL void socketDisconnect();
    Q_SIGNAL void socketConnected();

private:
    QTcpSocket* m_socket = nullptr;

};

#endif // TCPCLIENT_H
