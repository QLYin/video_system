#include "tcpreceivefilethread.h"
#include "tcpfilehelper.h"

TcpReceiveFileThread::TcpReceiveFileThread(QTcpSocket *tcpSocket, QObject *parent) : QThread(parent)
{
    savePath = qApp->applicationDirPath();
    blockSize = 0;
    blockNumber = 0;
    packageIndex = 0;

    this->tcpSocket = tcpSocket;
    this->socketHost = tcpSocket->peerAddress().toString();
    this->socketPort = tcpSocket->peerPort();

    connect(this, SIGNAL(receiveFinsh(QString, int)), this, SLOT(quit()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disConnect()));    
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif
}

void TcpReceiveFileThread::receiveData(QByteArray &buffer)
{
    //初始化数据流用于接收并写入文件
    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    //设置关键字接收对应的标识符
    int key;
    QByteArray data;
    in >> key >> data;
    blockNumber++;

    switch (key) {
        case 0x01: {
            //接收到文件名称,将对应文件保存 默认可执行文件同一目录
            fileName = fileName.fromUtf8(data.data(), data.length());
            file.setFileName(savePath + "/" + fileName);

            qDebug() << TIMEMS << "receiveFileName:" << file.fileName();
            emit receiveFileName(socketHost, socketPort, file.fileName());

            //如果同名文件存在则先删除原有文件
            if (file.exists()) {
                qDebug() << TIMEMS << "file exist and remove file:" << fileName;
                file.remove();
            }

            //如果文件打不开写入则退出
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << TIMEMS << "can not open file for write";
                break;
            }

            break;
        }
        case 0x02: {
            //接收到文件大小
            QString size = QString::fromUtf8(data.data(), data.length());
            quint64 fileSize = size.toULongLong();
            qDebug() << TIMEMS << "receiveFileSize:" << fileSize;
            emit receiveFileSize(socketHost, socketPort, fileSize);
            break;
        }

        case 0x03: {
            //这里的data就是接收到的包数据
            //可对接收到的数据进行解密或者拼包
            TcpFileHelper::encryData(data, "123");

            //打印和输出当前包信息
            packageIndex++;
            //qDebug() << TIMEMS << QString("接收端 当前接收第 %1 个包, 当前包大小 %2 字节").arg(packageIndex).arg(data.size());
            emit receivePackage(socketHost, socketPort, packageIndex);

            //逐个写入文件内容,调用flush使其立即执行
            file.write(data.data(), data.length());
            file.flush();
            emit receiveData(socketHost, socketPort, data.length());
            break;
        }
        case 0x04: {
            //接收到文件结束符号
            file.close();

            //嵌入式linux需要延时1秒钟等到文件写入完成
#ifdef __arm__
            msleep(1000);
            TcpFileHelper::tarAndChmod(fileName, savePath);
#endif

            //主动断开连接
            tcpSocket->disconnectFromHost();
            emit receiveFinsh(socketHost, socketPort);
#ifdef __arm__
            msleep(100);
            TcpFileHelper::reboot();
#endif
            break;
        }
    }
}

void TcpReceiveFileThread::setSavePath(const QString &savePath)
{
    this->savePath = savePath;
}

void TcpReceiveFileThread::run()
{
    exec();
}

void TcpReceiveFileThread::disConnect()
{
    tcpSocket->deleteLater();
}

void TcpReceiveFileThread::readData()
{
    while (tcpSocket->bytesAvailable() >= sizeof(quint64)) {
        if (blockSize == 0) {
            if (tcpSocket->bytesAvailable() < sizeof(quint64)) {
                return;
            }
            //读取数据块大小标识符
            tcpSocket->read((char *)&blockSize, sizeof(quint64));
        }

        if (tcpSocket->bytesAvailable() < blockSize) {
            return;
        }

        //只有当收到的数据块大小和预期的一致才读取数据
        QByteArray data = tcpSocket->read(blockSize);
        //读取到的数据交给receiveData函数处理
        receiveData(data);
        blockSize = 0;
    }
}

void TcpReceiveFileThread::error()
{
    if (file.isOpen()) {
        file.close();
        //通信出错则打印错误内容并关闭文件和删除文件
        qDebug() << TIMEMS << "receive file error" << tcpSocket->errorString();
    }

    file.remove(fileName);
    qDebug() << TIMEMS << "remove file" << file.fileName();
}
