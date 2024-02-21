#ifndef ONVIFBASE_H
#define ONVIFBASE_H

#include "onvifhead.h"

class OnvifBase : public QObject
{
    Q_OBJECT
public:
    explicit OnvifBase(QObject *parent = 0);

private:
    //onvif设备对象
    OnvifDevice *device;

public:
    //获取设备信息
    bool getDeviceInfo(OnvifDeviceInfo &deviceInfo);
    bool getScopes(OnvifDeviceInfo &deviceInfo);

    //获取设备服务文件
    bool getServices();
    //获取设备能力文件
    bool getCapabilities();

    //获取设备媒体文件
    QList<OnvifProfileInfo> getProfiles();
    //指定profile获取视频流地址
    QString getStreamUri(const QString &profileToken);
};

#endif // ONVIFBASE_H
