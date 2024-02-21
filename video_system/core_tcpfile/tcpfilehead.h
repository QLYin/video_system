#ifndef TCPFILEHEAD_H
#define TCPFILEHEAD_H

#include <QtGui>
#include <QtNetwork>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS QTime::currentTime().toString("hh:mm:ss zzz")
#endif

//发送文件结构体
struct SendFileConfig {
    QString fileName;           //文件名称
    quint64 fileSize;           //文件大小

    int timeout;                //超时时间
    int packageMerge;           //合并包数
    quint64 maxPackageSize;     //分包大小

    bool decryData;             //加密数据
    QString decryKey;           //加密秘钥

    QString receiveHost;        //接收地址
    int receivePort;            //接收端口
    int socketDescriptor;       //连接标识
    QProgressBar *progressBar;  //进度控件

    //默认构造函数
    SendFileConfig() {
        fileName = "qt_app.tar.gz";
        fileSize = 0;

        timeout = 3000;
        packageMerge = 1;
        maxPackageSize = 0xFFFF;

        decryData = false;
        decryKey = "123";

        receiveHost = "192.168.8.239";
        receivePort = 6904;
        socketDescriptor = 0;
        progressBar = 0;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const SendFileConfig &sendFileConfig) {
        QStringList list;
        list << QString("文件名称: %1").arg(sendFileConfig.fileName);
        list << QString("文件大小: %1").arg(sendFileConfig.fileSize);

        list << QString("超时时间: %1").arg(sendFileConfig.timeout);
        list << QString("合并包数: %1").arg(sendFileConfig.packageMerge);
        list << QString("分包大小: %1").arg(sendFileConfig.maxPackageSize);

        list << QString("加密数据: %1").arg(sendFileConfig.decryData);
        list << QString("加密秘钥: %1").arg(sendFileConfig.decryKey);

        list << QString("接收地址: %1").arg(sendFileConfig.receiveHost);
        list << QString("接收端口: %1").arg(sendFileConfig.receivePort);
        list << QString("连接标识: %1").arg(sendFileConfig.socketDescriptor);
        //list << QString("进度控件: %1").arg(sendFileConfig.progressBar);

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
        debug.noquote() << list.join("\n");
#else
        debug << list.join("\n");
#endif
        return debug;
    }
};

#endif // TCPFILEHEAD_H
