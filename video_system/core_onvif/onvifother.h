#ifndef ONVIFOTHER_H
#define ONVIFOTHER_H

#include "onvifhead.h"

class OnvifOther : public QObject
{
    Q_OBJECT
public:
    explicit OnvifOther(QObject *parent = 0);

private:
    //onvif设备对象
    OnvifDevice *device;

    //分割字符串
    QString getResult(const QString &value);
    int getResult2(const QString &value);
    QString getResult3(const QString &value, const QString &key);

    //通用处理 key表示节点关键字 value表示需要拿到返回值的节点关键字
    QString writeData(const QString &key, const QString &value, const QString &flag,
                      bool xmlns = true, bool value4 = false);

public:
    //重启设备
    QString systemReboot();
    //复位硬件
    QString hardReset();
    //获取自动配置参数
    QString getZeroConfig();

    //NTP校时
    QString getNtp();
    bool setNtp(const QString &ntp, bool isIp = false);

    //主机地址
    QString getHostName();
    bool setHostName(const QString &hostname);

    //网关
    QString getGateway();
    bool setGateway(const QString &gateway);

    //DNS
    QString getDns();
    bool setDns(const QString &dns, const QString &dhcp);

    //网卡接口
    OnvifNetConfig getInterface();
    bool setInterface(const OnvifNetConfig &netConfig);

    //接口协议
    QString getProtocol();
    bool setProtocol();

    //统一函数获取网络信息
    OnvifNetConfig getNetConfig();
    bool setNetConfig(const OnvifNetConfig &netConfig);

    //时间
    QString getDateTime();
    bool setDateTime(const QDateTime &datetime, bool ntp = false);

    //设置图片参数
    bool setImageSetting(const QString &videoSource, const OnvifImageSetting &imageSetting);
    //设置或创建OSD
    bool setOrCreateOsd(const QSize &videoSize, const OnvifOsdInfo &osd, bool create);
    //设置OSD
    bool setOsd(const QSize &videoSize, const OnvifOsdInfo &osd);
    //创建OSD
    bool createOsd(const QSize &videoSize, const OnvifOsdInfo &osd);
};

#endif // ONVIFOTHER_H
