#include "tcpsearchfileserver.h"

TcpSearchFileServer::TcpSearchFileServer(QObject *parent) : QObject(parent)
{
    fileName = "1.mp4";
    filePath = ".";
    serverIP = "127.0.0.1";
    serverPort = 6001;
    listenPort = 6001;

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataClient()));

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpSearchFileServer::readDataClient()
{
    if (tcpSocket->bytesAvailable() > 0) {
        QString data = tcpSocket->readAll();
        qDebug() << TIMEMS << "readDataClient" << data;
        emit searchFileResult(data == "ok");

        //立即关闭连接
        tcpSocket->disconnectFromHost();
        qDebug() << TIMEMS << "client disconnectFromHost";
    }
}

void TcpSearchFileServer::readDataServer()
{
    QTcpSocket *tcpSocket = (QTcpSocket *)sender();

    if (tcpSocket->bytesAvailable() > 0) {
        QString data = tcpSocket->readAll();
        qDebug() << TIMEMS << "readDataServer" << data;

        //过滤符合要求的请求命令
        if (data.startsWith("search")) {
            //取出文件名称,从当前目录找是否存在
            QString file = data.right(data.length() - 7);
            QDir dir(filePath);
            QStringList files = dir.entryList(QStringList() << "*.*");
            bool ok = files.contains(file);

            //将查询结果返回给客户端
            data = ok ? "ok" : "fail";
            qDebug() << TIMEMS << "search file" << data;
            tcpSocket->write(data.toLatin1());

            //查询成功则发送对应的文件名并关闭监听
            if (ok) {
                emit receiveFileName(file);
                QTimer::singleShot(100, this, SLOT(close()));
            }
        }
    }

    tcpSocket->disconnectFromHost();
    tcpSocket->deleteLater();
    qDebug() << TIMEMS << "tcpServer disconnectFromHost";
}

void TcpSearchFileServer::newConnection()
{
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataServer()));
        qDebug() << TIMEMS << "newConnection" << tcpSocket->peerAddress().toString() << tcpSocket->peerPort();
    }
}

void TcpSearchFileServer::setFileName(const QString &fileName)
{
    this->fileName = fileName;
}

void TcpSearchFileServer::setFilePath(const QString &filePath)
{
    this->filePath = filePath;
}

void TcpSearchFileServer::setServerIP(const QString &serverIP)
{
    this->serverIP = serverIP;
}

void TcpSearchFileServer::setServerPort(int serverPort)
{
    this->serverPort = serverPort;
}

void TcpSearchFileServer::setListenPort(int listenPort)
{
    this->listenPort = listenPort;
}

void TcpSearchFileServer::connectServer()
{
    //连接服务器
    tcpSocket->abort();
    tcpSocket->connectToHost(serverIP, serverPort);
    bool ok = tcpSocket->waitForConnected(1000);
    qDebug() << TIMEMS << "connect" << serverIP << serverPort << ok;

    //连接成功后问服务器询问文件是否存在
    if (ok) {
        QString data = QString("search:%1").arg(fileName);
        tcpSocket->write(data.toLatin1());

        //最大等待3秒钟,自动关闭连接
        QTimer::singleShot(3000, this, SLOT(disConnectServer()));
    }
}

void TcpSearchFileServer::disConnectServer()
{
    if (tcpSocket->state() != QTcpSocket::UnconnectedState) {
        tcpSocket->disconnectFromHost();
        qDebug() << TIMEMS << "client disconnectFromHost";
        emit searchFileResult(false);
    }
}

void TcpSearchFileServer::listen()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    bool ok = tcpServer->listen(QHostAddress::AnyIPv4, listenPort);
#else
    bool ok = tcpServer->listen(QHostAddress::Any, listenPort);
#endif
    qDebug() << TIMEMS << "listen" << listenPort << ok;
}

void TcpSearchFileServer::close()
{
    tcpServer->close();
    qDebug() << TIMEMS << "tcpServer close";
}
