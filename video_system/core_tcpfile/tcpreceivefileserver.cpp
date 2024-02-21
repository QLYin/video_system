#include "tcpreceivefileserver.h"
#include "tcpfilehelper.h"

TcpReceiveFileServer::TcpReceiveFileServer(QObject *parent) : QObject(parent)
{
    searchFile = false;
    saveFile = "1.mp4";
    savePath = qApp->applicationDirPath();

    listenPort = 6904;
    serverHost = "127.0.0.1";
    serverPort = 6000;

    tcpSocket = 0;
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpReceiveFileServer::newConnection()
{
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        if (searchFile) {
            //发送请求文件命令
            disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
            connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
            QString data = QString("search:%1").arg(saveFile);
            tcpSocket->write(data.toLatin1());
        } else {
            receiveFile(tcpSocket);
        }
    }
}

void TcpReceiveFileServer::receiveFile(QTcpSocket *tcpSocket)
{
    TcpReceiveFileThread *receiveFileThread = new TcpReceiveFileThread(tcpSocket, this);
    connect(receiveFileThread, SIGNAL(receiveFileName(QString, int, QString)), this, SIGNAL(receiveFileName(QString, int, QString)));
    connect(receiveFileThread, SIGNAL(receiveFileSize(QString, int, quint64)), this, SIGNAL(receiveFileSize(QString, int, quint64)));
    connect(receiveFileThread, SIGNAL(receivePackage(QString, int, quint64)), this, SIGNAL(receivePackage(QString, int, quint64)));
    connect(receiveFileThread, SIGNAL(receiveData(QString, int, quint64)), this, SIGNAL(receiveData(QString, int, quint64)));
    connect(receiveFileThread, SIGNAL(receiveFinsh(QString, int)), this, SIGNAL(receiveFinsh(QString, int)));
    receiveFileThread->setSavePath(savePath);
    receiveFileThread->start();
}

void TcpReceiveFileServer::readData()
{
    QTcpSocket *tcpSocket = (QTcpSocket *)sender();
    if (tcpSocket->bytesAvailable() > 0) {
        qDebug() << TIMEMS << "请求文件成功";
        disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
        receiveFile(tcpSocket);
    }
}

void TcpReceiveFileServer::setSearchFile(bool searchFile)
{
    this->searchFile = searchFile;
}

void TcpReceiveFileServer::setSaveFile(const QString &saveFile)
{
    this->saveFile = saveFile;
}

void TcpReceiveFileServer::setSavePath(const QString &savePath)
{
    this->savePath = savePath;
}

void TcpReceiveFileServer::setListenPort(int listenPort)
{
    this->listenPort = listenPort;
}

void TcpReceiveFileServer::setServerHost(const QString &serverHost)
{
    this->serverHost = serverHost;
}

void TcpReceiveFileServer::setServerPort(int serverPort)
{
    this->serverPort = serverPort;
}

bool TcpReceiveFileServer::startListen()
{
    //目录不存在则新建目录
    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkdir(TcpFileHelper::getPath(savePath));
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    bool ok = tcpServer->listen(QHostAddress::AnyIPv4, listenPort);
#else
    bool ok = tcpServer->listen(QHostAddress::Any, listenPort);
#endif

    qDebug() << TIMEMS << "listen receivefile port" << (ok ? "ok" : "error");
    return ok;
}

void TcpReceiveFileServer::closeListen()
{
    tcpServer->close();
}

bool TcpReceiveFileServer::startConnect()
{
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(serverHost, serverPort);
    bool ok = tcpSocket->waitForConnected();

    if (ok) {
        if (searchFile) {
            //发送请求文件命令
            disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
            connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
            QString data = QString("search:%1").arg(saveFile);
            tcpSocket->write(data.toLatin1());
        } else {
            receiveFile(tcpSocket);
        }
    }

    qDebug() << "connect tcpServer" << (ok ? "ok" : "error");
    return ok;
}

void TcpReceiveFileServer::closeConnect()
{
    if (tcpSocket && tcpSocket->isOpen()) {
        tcpSocket->disconnectFromHost();
    }
}
