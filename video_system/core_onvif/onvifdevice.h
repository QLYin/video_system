#ifndef ONVIFDEVICE_H
#define ONVIFDEVICE_H

#include "onvifhead.h"
class OnvifRequest;
class OnvifBase;
class OnvifPtz;
class OnvifVideo;
class OnvifEvent;
class OnvifSnap;
class OnvifOther;

class OnvifDevice : public QObject
{
    Q_OBJECT
public:
    //将功能拆分成友元类
    friend class OnvifBase;
    friend class OnvifPtz;
    friend class OnvifVideo;
    friend class OnvifEvent;
    friend class OnvifSnap;
    friend class OnvifOther;

    //打印线程消息类别(0-不打印 1-完整地址 2-IP地址)
    static int debugInfo;
    explicit OnvifDevice(QObject *parent = 0);
    ~OnvifDevice();

private:
    QString ip;                 //设备地址
    QString flag;               //唯一标识

    QString addrPort;           //地址端口(外网映射用)
    int rtspPort;               //视频端口(外网映射用)

    QString userName;           //用户名称
    QString userPwd;            //用户密码

    QString onvifAddr;          //设备onvif地址
    QString mediaUrl;           //媒体请求地址
    QString ptzUrl;             //云台请求地址
    QString imageUrl;           //图片请求地址
    QString eventUrl;           //事件请求地址

    QString rtspUrl;            //视频流地址
    QString snapUrl;            //图片抓拍地址

    QString profile;            //默认配置文件
    QString timezone;           //默认时区类型
    qint64 timeOffset;          //设备时间和电脑时间差值

    OnvifRequest *request;      //onvif请求类
    OnvifBase *onvifBase;       //onvif基础类
    OnvifPtz *onvifPtz;         //onvif云台类
    OnvifVideo *onvifVideo;     //onvif视频类
    OnvifEvent *onvifEvent;     //onvif事件类
    OnvifSnap *onvifSnap;       //onvif抓图类
    OnvifOther *onvifOther;     //onvif其他类

    OnvifDeviceInfo deviceInfo; //设备信息 设备厂家型号等信息

public:
    QString getHeadData()       const;
    QString getUserToken()      const;

    QString getOnvifAddr()      const;
    QString getMediaUrl()       const;
    QString getPtzUrl()         const;
    QString getImageUrl()       const;
    QString getEventUrl()       const;

public:
    //设置外网映射rtsp端口
    void setRtspPort(int rtspPort);
    //设置唯一标识(用来打印输出方便区分)
    void setFlag(const QString &flag);
    //统一格式打印信息
    void debug(const QString &head, const QString &msg);

    //设置用户信息
    void setUserInfo(const QString &userName, const QString &userPwd);
    //设置onvif地址
    void setOnvifAddr(const QString &onvifAddr);

    //设置用户信息和onvif地址
    void setDeviceData(const OnvifDeviceData &deviceData);
    //广播搜索回来的设备的部分信息设置过来
    void setDeviceInfo(const OnvifDeviceInfo &deviceInfo);

    //通用处理接收数据
    bool checkData(QNetworkReply *reply, QByteArray &data, const QString &flag);

    //获取设备信息
    OnvifDeviceInfo getDeviceInfo();

    //按照顺序依次获取基础信息
    bool getServices();
    bool getCapabilities();

    //获取配置文件集合
    QList<OnvifProfileInfo> getProfiles();
    //获取视频流地址
    QString getStreamUri(const QString &profileToken);

    //云台控制 type: 0-停止移动 1-绝对移动 2-相对移动 3-连续移动
    bool ptzControl(quint8 type, const QString &profileToken, qreal x, qreal y, qreal z);
    //预置位处理 type: 0-调用预置位 1-添加预置位 2-删除预置位 3-调用起始位 4-设置起始位
    bool ptzPreset(quint8 type, const QString &profileToken, const QString &presetToken = QString(), const QString &presetName = QString());
    //获取预置位集合
    QList<OnvifPresetInfo> getPresets(const QString &profileToken);
    //获取预置位巡航
    void getPresetTours(const QString &profileToken);
    //获取当前预置位
    OnvifPresetInfo getCurrentPreset(const QString &videoSource);
    //获取云台状态
    bool getStatus(const QString &profileToken, qreal &x, qreal &y, qreal &z);

    //获取视频配置集合
    QList<OnvifVideoSource> getVideoSources();
    //获取和设置图片参数
    void getImageOption(const QString &videoSource, OnvifImageSetting &imageSetting);
    void getImageSetting(const QString &videoSource, OnvifImageSetting &imageSetting);
    bool setImageSetting(const QString &videoSource, const OnvifImageSetting &imageSetting);

    //获取指定OSD信息
    OnvifOsdInfo getOsd(const QString &osdToken, const QSize &videoSize);
    //获取OSD信息集合
    QList<OnvifOsdInfo> getOsds(const QString &videoSource, const QSize &videoSize);
    //设置OSD信息
    bool setOsd(const QSize &videoSize, const OnvifOsdInfo &osd);
    //创建OSD信息
    bool createOsd(const QSize &videoSize, const OnvifOsdInfo &osd);
    //删除指定OSD
    bool deleteOsd(const QString &osdToken);

    //事件订阅
    QString getEvent(const QString &timeout = "PT600S");
    void pullMessage(const QString &timeout = "PT1M");

    //抓拍图片
    QString getSnapshotUri(const QString &profileToken);
    QImage snapImage(const QString &profileToken);

    //系统重启
    QString systemReboot();

    //获取和设置NTP服务器
    QString getNtp();
    bool setNtp(const QString &ntp, bool isIp = false);

    //获取和设置网络配置
    OnvifNetConfig getNetConfig();
    bool setNetConfig(const OnvifNetConfig &netConfig);

    //获取和设置设备时间
    QString getDateTime();
    bool setDateTime(const QDateTime &datetime, bool ntp = false);

signals:
    void sendData(const QString &url, const QByteArray &data);
    void receiveData(const QString &url, const QByteArray &data);
    void receiveError(const QString &url, const QByteArray &data);

    void receiveInfo(const QString &data);
    void receiveError(const QString &data);
    void receiveEvent(const QString &url, const OnvifEventInfo &event);
};

#endif // ONVIFDEVICE_H
