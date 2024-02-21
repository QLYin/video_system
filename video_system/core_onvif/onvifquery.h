#ifndef ONVIFQUERY_H
#define ONVIFQUERY_H

#include "onvifhead.h"

class OnvifQuery : public QObject
{
    Q_OBJECT
public:
    explicit OnvifQuery(QObject *parent = 0);

private:
    //处理的数据
    QByteArray buffer;
    //传入的文件名称
    QString fileName;

    //节点地址
    OnvifWsdlAddr wsdlAddr;
    //xml数据解析对象
    QDomDocument doc;

public:
    //支持直接指定本地的文件进行处理
    bool setData(const QByteArray &data, const QString &fileName = QString());    

    //获取搜索设备 能够拿到部分设备信息
    void getSearchInfo(OnvifDeviceInfo &deviceInfo, const QString &ip);
    //获取设备信息 拿到剩余部分设备信息
    void getDeviceInfo(OnvifDeviceInfo &deviceInfo);
    //获取设备信息 拿到剩余部分设备信息
    void getScopes(OnvifDeviceInfo &deviceInfo);

    //重新校验地址
    void checkAddr(const QString &addrPort, QString &addr, bool checkOnvif);

    //获取服务地址(早期onvif协议要用getCapabilities)
    OnvifHttpAddr getServices(const QString &addrPort);
    OnvifHttpAddr getCapabilities(const QString &addrPort);

    //获取服务文件
    QList<OnvifProfileInfo> getProfiles();
    //获取预置位集合
    QList<OnvifPresetInfo> getPresets();
    //获取云台状态
    void getStatus(qreal &x, qreal &y, qreal &z);

    //通用获取属性或者值
    QString getValueByTagName(const QString &name, const QString &wsdl);
    QString getValueByTagName(QDomElement element, const QString &name);
    QStringList getValueByTagName(QDomElement element, const QString &name, const QStringList &keys);

    //获取视频流地址
    QString getStreamUri(const QString &ip, int port);
    //获取截图地址
    QString getSnapshotUri(const QString &addrPort);

    //获取订阅事件请求地址
    QString getEventAddr(const QString &addrPort);
    //获取事件内容
    OnvifEventInfo getEventInfo();

    //获取视频配置集合
    QList<OnvifVideoSource> getVideoSources();

    //指定节点获取OSD
    OnvifOsdInfo getOsd(QDomElement element, const QSize &videoSize);
    //获取指定OSD信息
    OnvifOsdInfo getOsd(const QSize &videoSize);
    //获取OSD信息集合
    QList<OnvifOsdInfo> getOsds(const QSize &videoSize);

    //获取图片参数范围及参数值
    void getImageOption(const QString &tagName, qreal &min, qreal &max);
    void getImageOption(OnvifImageSetting &imageSetting);
    void getImageSetting(OnvifImageSetting &imageSetting);
};

#endif // ONVIFQUERY_H
