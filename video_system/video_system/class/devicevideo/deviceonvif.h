#ifndef DEVICEONVIF_H
#define DEVICEONVIF_H

#include "qdatetime.h"
#include "onvifhead.h"
#include "singleton.h"

class DeviceOnvif : public QThread
{
    Q_OBJECT SINGLETON_DECL(DeviceOnvif)

public:
    //校验地址对应的信息
    static bool checkUrl(const QString &url, OnvifDeviceData &deviceData);

    //获取预置位集合
    static QList<OnvifPresetInfo> getPresets(const QString &url);
    //云台控制 type: 0-停止移动 1-绝对移动 2-相对移动 3-连续移动
    static bool ptzControl(quint8 type, const QString &url, double x, double y, double z);
    //预置位处理 type: 0-调用预置位 1-添加预置位 2-删除预置位 3-调用起始位 4-设置起始位
    static bool ptzPreset(quint8 type, const QString &url, const QString &presetToken = QString(), const QString &presetName = QString());

    //获取OSD集合
    static QList<OnvifOsdInfo> getOSDs(const QString &url);
    //OSD操作 type: 0-创建 1-设置 2-删除
    static bool osdControl(quint8 type, const QString &url, const OnvifOsdInfo &osd);

    //获取和设置网络配置
    static OnvifNetConfig getNetConfig(const QString &url);
    static bool setNetConfig(const QString &url, const OnvifNetConfig &netConfig);

public:
    explicit DeviceOnvif(QObject *parent = 0);
    ~DeviceOnvif();

protected:
    void run();

private:
    //数据锁
    QMutex mutex;
    //停止线程标志位
    volatile bool stopped;

    //一轮是否完成
    bool finsh;
    //当前处理离线设备的索引
    int currentIndex;
    //显示抓拍图片标签
    QLabel labImage;

    //警情字典表(事件等级/唯一标识/报警字样/解除字样)
    QList<int> eventLevel;
    QStringList eventName;
    QStringList eventAlarm;
    QStringList eventNormal;

    //图文警情队列
    QStringList listMsg;
    QStringList listFile;

private slots:
    //加载警情字典
    void initData();
    //判断设备在线
    void checkOnline();

    //播放成功
    void receivePlayStart(int time);
    //播放结束
    void receivePlayFinsh();

    //抓拍图片返回
    void receiveImage(const QString &url, const QImage &image);
    //事件订阅返回
    void receiveEvent(const QString &url, const OnvifEventInfo &event);

    //添加图文警情
    void addMsgList(const QString &msg, const QString &result, const QImage &image);

signals:
    //执行结果返回
    void receiveResult(const QString &url, const QString &cmd, const QVariant &data);
};

#endif // DEVICEONVIF_H
