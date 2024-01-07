#include "tcpsendfilethread.h"
#include "tcpfilehelper.h"

TcpSendFileThread::TcpSendFileThread(QObject *parent) : QThread(parent)
{
    //发送完毕销毁自身
    connect(this, SIGNAL(sendFinsh(QString, int, QString)), this, SLOT(deleteLater()));
}

void TcpSendFileThread::run()
{
    //在线程中执行发送文件
    sendFile();
}

void TcpSendFileThread::sendFile()
{
    //常用的数据从结构体中拿出来,后面的代码可以短一点,哈哈
    QString fileName = sendFileConfig.fileName;
    QString receiveHost = sendFileConfig.receiveHost;
    int receivePort = sendFileConfig.receivePort;

    //获取文件大小
    QFile file(fileName);
    quint64 fileSize = file.size();
    //获取文件名称
    QString name = QFileInfo(fileName).fileName();

    //检测文件是否存在已经文件能否打开
    if (fileSize == 0 || !file.open(QIODevice::ReadOnly)) {
        return;
    }

    //新建socket连接,保证在线程中运行
    QTcpSocket *tcpSocket = new QTcpSocket;
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));

    //很巧妙的设计,通过标识符的设置可以让外部建立的连接移到线程运行
    if (sendFileConfig.socketDescriptor != 0) {
        tcpSocket->setSocketDescriptor(sendFileConfig.socketDescriptor);
    }

    //连接不存在则建立连接连接服务器
    bool ok = true;
    if (!tcpSocket->isOpen()) {
        tcpSocket->connectToHost(receiveHost, receivePort);
        ok = tcpSocket->waitForConnected(sendFileConfig.timeout);
    }

    //连接失败不用继续
    if (!ok) {
        doError(0);
        return;
    }

    //每次包的索引要重置
    int packageIndex = 0;
    //数据池用于合并多个包
    QByteArray buffer;

    //准备数据流,设置数据流版本
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    //第一步: 写入开始符及文件名称
    block.clear();
    out.device()->seek(0);
    out << 0x01 << name.toUtf8();
    if (!sendFile(tcpSocket, block)) {
        return;
    }

    //第二步: 写入文件大小
    block.clear();
    out.device()->seek(0);
    out << 0x02 << QString::number(fileSize).toUtf8();
    if (!sendFile(tcpSocket, block)) {
        return;
    }

    //第三步: 循环写入文件数据
    do {
        //从文件头开始读取
        block.clear();
        out.device()->seek(0);

        //这里做个限制,每次最多发送 最大包数大小 个字节
        //这里的data就是每次拿到的文件的每个包的数据
        QByteArray data = file.read(sendFileConfig.maxPackageSize);
        quint64 size = data.size();

        //可对拿到的数据进行加密,再将加密后的数据发送
        TcpFileHelper::decryData(data, "123");
        packageIndex++;

        //合并包后接收端接收到的包数会对应减少
        int count = sendFileConfig.packageMerge;
        if (count > 1) {
            int value = packageIndex % count;
            int index = (value == 0 ? count : value);
            buffer.append(data);
            //qDebug() << TIMEMS << QString("发送端 已读取第 %1 个包, 当前包大小 %2 字节").arg(index).arg(size);

            //这里还要排除是不是最后一个包
            //当前包大小如果不是maxPackageSize最大包大小则表示最后一个包
            if (value != 0 && size == sendFileConfig.maxPackageSize) {
                continue;
            }

            data = buffer;
            buffer.clear();
        }

        size = data.size();
        out << 0x03 << data;
        //qDebug() << TIMEMS << QString("发送端 当前发送第 %1 个包, 当前包大小 %2 字节").arg(packageIndex).arg(size);

        //每次发送包数据后都更新进度条
        QMetaObject::invokeMethod(this, "updateProgress", Q_ARG(quint64, size));
        emit sendPackage(receiveHost, receivePort, packageIndex, size);
        if (!sendFile(tcpSocket, block)) {
            return;
        }
    } while (!file.atEnd());

    //第四步: 写入结束符及文件名称
    block.clear();
    out.device()->seek(0);
    out << 0x04 << name.toUtf8();
    if (!sendFile(tcpSocket, block)) {
        return;
    }

    //等待断开连接
    if (tcpSocket->waitForDisconnected()) {
        QMetaObject::invokeMethod(this, "updateProgress", Q_ARG(quint64, 0));
        emit sendFinsh(receiveHost, receivePort, "");
    } else {
        doError(2);
    }
}

bool TcpSendFileThread::sendFile(QTcpSocket *tcpSocket, const QByteArray &block)
{
    //先写入当前数据块大小,以便接收端收到后判断
    quint64 size = block.size();
    tcpSocket->write((char *)&size, sizeof(quint64));
    tcpSocket->write(block.data(), size);

    //然后等待写入数据流,写入失败则断开连接返回
    if (!tcpSocket->waitForBytesWritten()) {
        tcpSocket->disconnectFromHost();
        doError(1);
        return false;
    }

    return true;
}

void TcpSendFileThread::doError(int type)
{
    //更新进度条
    QMetaObject::invokeMethod(this, "updateProgress", Q_ARG(quint64, -1));

    //发送出错
    QString error;
    if (type == 0) {
        error = "连接服务器失败";
    } else if (type == 1) {
        error = "发送数据包失败";
    } else if (type == 2) {
        error = "断开服务器失败";
    }

    emit sendFinsh(sendFileConfig.receiveHost, sendFileConfig.receivePort, error);
}

void TcpSendFileThread::updateProgress(quint64 size)
{
    TcpFileHelper::updateProgressBar(sendFileConfig.progressBar, sendFileConfig.fileSize, size);
}

void TcpSendFileThread::setSendFileConfig(const SendFileConfig &sendFileConfig)
{
    this->sendFileConfig = sendFileConfig;
}
