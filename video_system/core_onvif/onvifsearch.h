#ifndef ONVIFSEARCH_H
#define ONVIFSEARCH_H

#include "onvifhead.h"

class OnvifSearch : public QObject
{
    Q_OBJECT
public:
    explicit OnvifSearch(QObject *parent = 0);
    ~OnvifSearch();

private:
    bool isOk;                      //网络通信是否正常
    QString currentIP;              //当前绑定的网卡IP
    QString currentFile;            //当前发送的文件
    QString searchFilter;           //过滤设备
    int searchInterval;             //搜索间隔 单位毫秒

    QMutex mutex;                   //数据锁
    int deviceCount;                //设备计数
    QTimer *timerData;              //处理数据定时器
    QList<QString> ips;             //地址队列
    QList<QByteArray> buffers;      //数据队列

    QTimer *timerSearch;            //搜索设备定时器
    QUdpSocket *udpSocket;          //网络通信对象

    //设备信息集合
    QList<OnvifDeviceInfo> deviceInfos;

private slots:
    void sendData();
    void writeData(const QString &ip);
    void readData(const QString &fileName);

    void readData();
    void checkData();
    void checkData(const QByteArray &data, const QString &ip);
    bool checkExist(const QString &addr);

public:
    void setSearchFilter(const QString &searchFilter);
    void setSearchInterval(int searchInterval);

    //清空设备信息
    void clear();
    //搜索设备(指定了deviceIP则表示是单播)
    bool search(const QString &localIP, const QString &deviceIP);

    //获取设备信息集合
    QList<OnvifDeviceInfo> getDeviceInfos();
    //获取设备地址集合
    QStringList getDeviceAddrs();

signals:
    void sendData(const QByteArray &data);
    void receiveData(const QByteArray &data);
    void receiveInfo(const QString &data);
    void receiveError(const QString &data);
    void receiveDevice(const OnvifDeviceInfo &deviceInfo);
};

#endif // ONVIFSEARCH_H
