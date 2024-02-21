#ifndef TCPRECEIVEFILESERVER_H
#define TCPRECEIVEFILESERVER_H

#include "tcpreceivefilethread.h"

class TcpReceiveFileServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpReceiveFileServer(QObject *parent = 0);

private:
    bool searchFile;        //请求文件模式
    QString saveFile;       //请求文件
    QString savePath;       //保存路径

    int listenPort;         //监听端口
    QString serverHost;     //服务器地址
    int serverPort;         //服务器端口

    QTcpServer *tcpServer;  //用于监听端口被动接收文件
    QTcpSocket *tcpSocket;  //用于主动连接发送端请求文件

private slots:
    //服务端模式连接建立
    void newConnection();
    //新建线程接收文件
    void receiveFile(QTcpSocket *tcpSocket);
    //读取数据校验结果
    void readData();

public:
    //设置请求文件模式
    void setSearchFile(bool searchFile);
    //设置请求文件
    void setSaveFile(const QString &saveFile);
    //设置文件默认保存路径
    void setSavePath(const QString &savePath);

    //设置监听端口,被动接收传输过来的文件
    void setListenPort(int listenPort);

    //设置发送端IP和端口,主动请求文件
    void setServerHost(const QString &serverHost);
    void setServerPort(int serverPort);

    //启动监听,开始被动接收文件
    bool startListen();
    void closeListen();

    //启动连接服务器,请求文件
    bool startConnect();
    void closeConnect();

signals:
    //接收到文件名称
    void receiveFileName(const QString &ip, int port, const QString &fileName);
    //接收到文件大小
    void receiveFileSize(const QString &ip, int port, quint64 fileSize);
    //当前接收包编号
    void receivePackage(const QString &ip, int port, quint64 packageIndex);
    //接收到文件写入大小
    void receiveData(const QString &ip, int port, quint64 size);
    //接收完成
    void receiveFinsh(const QString &ip, int port);
};

#endif // TCPRECEIVEFILESERVER_H
