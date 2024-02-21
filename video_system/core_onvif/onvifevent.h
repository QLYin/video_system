#ifndef ONVIFEVENT_H
#define ONVIFEVENT_H

#include "onvifhead.h"

class OnvifEvent : public QObject
{
    Q_OBJECT
public:
    explicit OnvifEvent(QObject *parent = 0);

private:
    //请求锁对象
    QMutex mutex;

    //onvif设备对象
    OnvifDevice *device;
    //事件订阅地址
    QString eventAddr;

    //定时器获取事件请求地址
    QTimer *timerEvent;
    //定时器获取事件
    QTimer *timerMessage;

public slots:
    //收到事件信号
    void receiveEvent(const OnvifEventInfo &event);

    //订阅事件服务 PT600S表示请求地址有效期600秒=10分钟
    //意味着超过这个时间以后要重新请求获取新的事件服务地址
    QString getEvent(const QString &timeout = "PT600S");

    //请求事件 PT1S表示请求超时1秒 PT1M 表示请求超时1分钟
    //运行机制大概是如下描述
    //1. client发送请求到server
    //2. 如果server有数据回复了,解析完数据后继续发送请求到server,直到拿完所有的事件数据
    //3. 到了超时时间client重新发送请求到server
    //4. 三个地方需要发送请求 首次发送+超时发送+有收到事件回复发送
    //5. 定时器只是用来做超时发送请求用
    //6. client发送请求以后如果没有回复则一直等待,有server有事件会主动发过来

    //至于server端如何知道发给client可能是下面的原因
    //1. 发送的请求数据带了超时时间,比如1分钟
    //2. 当请求以后,如果没有事件,则摄像机不会断开连接,直到超时或者有新的事件
    //3. 如果server端没有主动断开连接,则请求一直阻塞在那里
    void pullMessage(const QString &timeout = "PT1M");

signals:
    void receiveInfo(const QString &data);
    void receiveEvent(const QString &url, const OnvifEventInfo &event);
};

#endif // ONVIFEVENT_H
