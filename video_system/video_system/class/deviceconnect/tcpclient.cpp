#include "tcpclient.h"
#include "qthelper.h"
#include "tcpcmddef.h"
#include "cmdhandlermgr.h"

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
    QHostAddress kServerAddress(AppConfig::DeviceIP);
    quint16 kServerPort = QString("61111").toUShort();
    if (m_socket)
    {
        qDebug() << __FUNCTION__ << " device ip: " <<kServerAddress;
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
    {
        qDebug() << "[TcpClient]Invaild ";
        return;

    }

    QString fullData;
    while (m_socket->bytesAvailable())
    {
        fullData += m_socket->readAll();
    }
    qDebug() << "[TcpClient]read data: " << fullData;

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

    for (auto& cmd : result)
    {
        if (!cmd.isEmpty())
        {
            CmdHandlerMgr::Instance()->handle(cmd);
            QVariantMap data;
            QStringList lines = cmd.split("\r\n");
            if (!lines.isEmpty() && lines.at(0).contains("cmd :"))
            {
                data["cmd"] = lines.at(0).split(":").at(1).trimmed();
                QString cmdData;
                auto firstIndex = cmd.indexOf("\r\n") + 2;
                if (cmd.size() > firstIndex)
                {
                    cmdData = cmd.mid(firstIndex, cmd.length() - firstIndex);
                }
                data["cmdData"] = cmdData;
            }

            if (!data.isEmpty())
            {
                emit socketData(data);
            }

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
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::onConnected()
{
    qDebug() << "[TcpClient]onConnected ";
    emit socketConnected();
}
