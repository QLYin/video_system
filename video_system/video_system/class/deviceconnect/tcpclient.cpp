#include "tcpclient.h"
#include "qthelper.h"

namespace {
    QHostAddress kServerAddress(QString("113.118.202.36"));
    quint16 kServerPort = QString("61111").toUShort();
};

SINGLETON_IMPL(TcpClient)
TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
}

bool TcpClient::init()
{
    if (m_socket == nullptr)
    {
        m_socket = new QTcpSocket(this);
    }

    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::socketDisconnect);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::readData, Qt::QueuedConnection);
    connect(m_socket,static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &TcpClient::displayError);

    connectServer();

    return true;
}

void TcpClient::connectServer()
{
    if (m_socket)
    {
        m_socket->connectToHost(kServerAddress, kServerPort);
    }
}

bool TcpClient::uninit()
{
    if (m_socket)
    {
        m_socket->abort();
        // m_socket->disconnectFromServer();
        m_socket->close();
        m_socket = nullptr;
    }

    return true;
}

void TcpClient::readData()
{
    if (!m_socket || !m_socket->isValid())
        return;

    QString fullData;
    while (m_socket->bytesAvailable())
    {
        fullData += m_socket->readAll();
    }

    QStringList result;
    QString currentSubString;
    QStringList lines = fullData.split("\r\n");
    for (const QString& line : lines) {
        if (line.startsWith("cmd :")) {
            if (!currentSubString.isEmpty()) {
                result.append(currentSubString);
                currentSubString.clear();
            }
            currentSubString = line + "\r\n";
        }
        else {
            currentSubString += line + "\r\n";
        }
    }
    if (!currentSubString.isEmpty()) {
        result.append(currentSubString);
    }
    qDebug() << "[TcpClient]read result: " << result;

    for (auto& cmd : result)
    {
        QStringList lines = cmd.split("\r\n");
        QVariantMap data;
        for (auto& line : lines)
        {
            qDebug() << "[TcpClient]read line: " << line;
            if (!line.isEmpty() && line.contains(":"))
            {
                auto key = line.split(":").at(0).trimmed();
                auto value = line.split(":").at(1).trimmed();
                data.insert(key, value);
            }
        }
        if (!data.isEmpty())
        {
            emit socketData(data);
        }
    }

    return;
}

void TcpClient::sendData(const QString& data)
{
    if (m_socket && m_socket->isOpen())
    {
        qDebug() << "[TcpClient]Sending data: " << data;
        m_socket->write(data.toUtf8());
        m_socket->flush();
    }
    return;
}

void TcpClient::displayError(QAbstractSocket::SocketError err)
{
    qDebug() << "[TcpClient]socket err: " << err;
    emit socketError(err);
    return;
}

bool TcpClient::isConnnected()
{
    if (!m_socket)
        return false;
    return m_socket->state() == QAbstractSocket::ConnectingState || m_socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::onConnected()
{
    qDebug() << "[TcpClient]onConnected ";
    emit socketConnected();
}
