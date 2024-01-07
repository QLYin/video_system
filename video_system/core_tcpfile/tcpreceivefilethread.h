#ifndef TCPRECEIVEFILETHREAD_H
#define TCPRECEIVEFILETHREAD_H

#include "tcpfilehelper.h"

class TcpReceiveFileThread : public QThread
{
    Q_OBJECT
public:
    explicit TcpReceiveFileThread(QTcpSocket *tcpSocket, QObject *parent = 0);

protected:
    void run();

private:
    QFile file;             //接收文件对象
    QString fileName;       //文件名称
    QString savePath;       //文件保存目录

    QTcpSocket *tcpSocket;  //tcp通信对象
    QString socketHost;     //通信对象主机地址
    int socketPort;         //通信对象主机端口

    quint64 blockSize;      //数据块大小
    quint64 blockNumber;    //数据块编号
    quint64 packageIndex;   //当前包索引

private slots:
    //读取网络数据
    void readData();
    //连接断开
    void disConnect();
    //网络通信出错
    void error();

    //逐个接收文件数据
    void receiveData(QByteArray &buffer);

public:
    //设置保存文件夹
    void setSavePath(const QString &savePath);

signals:
    //接收到文件名称
    void receiveFileName(const QString &ip, int port, const QString &fileName);
    //接收到文件大小
    void receiveFileSize(const QString &ip, int port, quint64 fileSize);
    //当前接收包编号
    void receivePackage(const QString &ip, int port, quint64 packageIndex);
    //接收到文件写入大小
    void receiveData(const QString &ip, int port, quint64 fileSize);
    //接收完成
    void receiveFinsh(const QString &ip, int port);
};

#endif // TCPRECEIVEFILETHREAD_H
