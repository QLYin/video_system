#ifndef TCPSENDFILETHREAD_H
#define TCPSENDFILETHREAD_H

#include "tcpfilehelper.h"

class TcpSendFileThread : public QThread
{
    Q_OBJECT
public:
    explicit TcpSendFileThread(QObject *parent = 0);

protected:
    void run();

private:
    //发送配置结构体
    SendFileConfig sendFileConfig;

private slots:
    //发送文件
    void sendFile();
    //发送一个数据块
    bool sendFile(QTcpSocket *tcpSocket, const QByteArray &block);
    //执行出错处理
    void doError(int type);

    //更新进度
    void updateProgress(quint64 size);

public:
    //设置发送配置结构体
    void setSendFileConfig(const SendFileConfig &sendFileConfig);

signals:
    //发送文件数据 package=包索引 size=包大小
    void sendPackage(const QString &ip, int port, quint64 package, quint64 size);
    //发送文件完成 error=错误内容 为空表示没有出错
    void sendFinsh(const QString &ip, int port, const QString &error);
};

#endif // TCPSENDFILETHREAD_H
