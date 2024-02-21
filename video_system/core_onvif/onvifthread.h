#ifndef ONVIFTHREAD_H
#define ONVIFTHREAD_H

#include "onvifhead.h"

class OnvifThread : public QThread
{
    Q_OBJECT
public:
    //onvif设备对象集合
    static QList<OnvifDevice *> onvifDevices;
    //获取指定url的onvif设备对象
    static OnvifDevice *getOnvifDevice(const OnvifDeviceData &deviceData);

    static OnvifThread *Instance();
    explicit OnvifThread(QObject *parent = 0);
    ~OnvifThread();

protected:
    void run();

private:
    //智能指针
    static QScopedPointer<OnvifThread> self;
    //全局锁
    QMutex mutex;
    //线程停止标志位
    volatile bool stopped;
    //正在工作标志位
    volatile bool working;

    //执行的指令onvif对象地址集合(从上面的onvifDevices中取出来)
    QList<OnvifDevice *> devices;
    //执行的指令集合
    QList<QString> cmds;
    //执行的指令参数数据集合
    QList<QVariant> datas;

public slots:
    //停止线程
    void stop();
    //主动工作一次
    void work();

    //添加执行指令
    void append(const OnvifDeviceData &deviceData, const QString &cmd, const QVariant &data = QVariant(QVariant::Invalid));

signals:
    //抓拍图片返回
    void receiveImage(const QString &url, const QImage &image);
    //事件订阅返回
    void receiveEvent(const QString &url, const OnvifEventInfo &event);
    //执行结果返回
    void receiveResult(const QString &url, const QString &cmd, const QVariant &data);
};

#endif // ONVIFTHREAD_H
