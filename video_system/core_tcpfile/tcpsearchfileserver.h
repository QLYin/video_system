#ifndef TCPSEARCHFILESERVER_H
#define TCPSEARCHFILESERVER_H

#include "tcpfilehelper.h"

class TcpSearchFileServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpSearchFileServer(QObject *parent = 0);

private:
    QString fileName;       //文件名称
    QString filePath;       //文件目录
    QString serverIP;       //服务器IP
    int serverPort;         //服务器端口
    int listenPort;         //监听端口

    QTcpSocket *tcpSocket;  //客户端对象
    QTcpServer *tcpServer;  //服务器对象

private slots:
    void readDataClient();  //读取客户端数据
    void readDataServer();  //读取服务器数据
    void newConnection();   //新的连接建立

public:
    //设置文件名称
    void setFileName(const QString &fileName);
    //设置文件目录
    void setFilePath(const QString &filePath);
    //设置服务器IP
    void setServerIP(const QString &serverIP);
    //设置服务器端口
    void setServerPort(int serverPort);
    //设置监听端口
    void setListenPort(int listenPort);

    //客户端向服务器询问文件是否存在
    void connectServer();
    void disConnectServer();

    //服务端对客户端发来的文件请求进行回复
    void listen();
    void close();

signals:
    void searchFileResult(bool ok);
    void receiveFileName(const QString &fileName);
};

#endif // TCPSEARCHFILESERVER_H
