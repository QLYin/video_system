#include "onvifdevice.h"

int OnvifDevice::debugInfo = 2;
OnvifDevice::OnvifDevice(QObject *parent) : QObject(parent)
{
    userName = "admin";
    userPwd = "123456";
    timezone = "CST-8";
    timeOffset = -1;

    //挨个实例化对应的类并绑定信号槽,最后数据的收发等信号都由本类发出去
    request = new OnvifRequest(this);
    connect(request, SIGNAL(sendData(QString, QByteArray)), this, SIGNAL(sendData(QString, QByteArray)));
    connect(request, SIGNAL(receiveData(QString, QByteArray)), this, SIGNAL(receiveData(QString, QByteArray)));

    //onvif基础类 获取设备信息/能力文件/媒体文件等
    onvifBase = new OnvifBase(this);

    //onvif云台类 云台控制/添加删除修改预置位
    onvifPtz = new OnvifPtz(this);

    //onvif视频类 获取视频配置文件/图片参数获取和设置
    onvifVideo = new OnvifVideo(this);

    //onvif事件类 各种事件订阅
    onvifEvent = new OnvifEvent(this);
    connect(request, SIGNAL(receiveEvent(OnvifEventInfo)), onvifEvent, SLOT(receiveEvent(OnvifEventInfo)));
    connect(onvifEvent, SIGNAL(receiveInfo(QString)), this, SIGNAL(receiveInfo(QString)));
    connect(onvifEvent, SIGNAL(receiveEvent(QString, OnvifEventInfo)), this, SIGNAL(receiveEvent(QString, OnvifEventInfo)));

    //onvif抓图类 抓图
    onvifSnap = new OnvifSnap(this);
    connect(onvifSnap, SIGNAL(receiveInfo(QString)), this, SIGNAL(receiveInfo(QString)));

    //onvif其他类 重启/网络配置参数/校时等
    onvifOther = new OnvifOther(this);
}

OnvifDevice::~OnvifDevice()
{
}

QString OnvifDevice::getHeadData() const
{
    return OnvifXml::getHeadData(getUserToken());
}

QString OnvifDevice::getUserToken() const
{
    return OnvifXml::getUserToken(userName, userPwd, timeOffset);
}

QString OnvifDevice::getOnvifAddr() const
{
    return this->onvifAddr;
}

QString OnvifDevice::getMediaUrl() const
{
    return this->mediaUrl;
}

QString OnvifDevice::getPtzUrl() const
{
    return this->ptzUrl;
}

QString OnvifDevice::getImageUrl() const
{
    return this->imageUrl;
}

QString OnvifDevice::getEventUrl() const
{
    return this->eventUrl;
}

void OnvifDevice::setRtspPort(int rtspPort)
{
    this->rtspPort = rtspPort;
}

void OnvifDevice::setFlag(const QString &flag)
{
    this->flag = flag;
}

void OnvifDevice::debug(const QString &head, const QString &msg)
{
    if (debugInfo == 0) {
        return;
    }

    //如果设置了唯一标识则放在打印前面
    QString text = head;
    if (!flag.isEmpty()) {
        text = QString("标识[%1] %2").arg(flag).arg(text);
    }

    QString addr = (debugInfo == 2 ? ip : onvifAddr);
    if (msg.isEmpty()) {
        qDebug() << TIMEMS << QString("%1 -> 地址: %2").arg(text).arg(addr);
    } else {
        qDebug() << TIMEMS << QString("%1 -> %2 地址: %3").arg(text).arg(msg).arg(addr);
    }
}

void OnvifDevice::setUserInfo(const QString &userName, const QString &userPwd)
{
    this->userName = userName;
    this->userPwd = userPwd;
}

void OnvifDevice::setOnvifAddr(const QString &onvifAddr)
{
    //从onvif地址取出ip地址和端口 http://100.68.190.56:6008/onvif/device_service
    QStringList list = onvifAddr.split("/");
    this->ip = OnvifHelper::getUrlIP(onvifAddr);
    this->addrPort = list.at(2);
    this->rtspPort = 0;
    this->onvifAddr = onvifAddr;
}

void OnvifDevice::setDeviceData(const OnvifDeviceData &deviceData)
{
    this->userName = deviceData.userName;
    this->userPwd = deviceData.userPwd;
    this->setOnvifAddr(deviceData.onvifAddr);
}

void OnvifDevice::setDeviceInfo(const OnvifDeviceInfo &deviceInfo)
{
    this->deviceInfo = deviceInfo;
}

bool OnvifDevice::checkData(QNetworkReply *reply, QByteArray &data, const QString &flag)
{
    if (!reply) {
        return false;
    }

    //读取数据并销毁对象
    QString url = reply->url().toString();
    data = reply->readAll();
    reply->deleteLater();

    bool ok = false;
    if (reply->error() == QNetworkReply::NoError) {
        if (!data.isEmpty()) {
            ok = true;
            if (flag != "收到截图") {
                emit receiveData(url, data);
            }
        }
    } else {
        emit receiveError(url, data);
        emit receiveError(QString("%1 -> 错误标识: %2  错误信息: %3").arg(flag).arg(reply->error()).arg(reply->errorString()));
    }

    return ok;
}

OnvifDeviceInfo OnvifDevice::getDeviceInfo()
{
    onvifBase->getDeviceInfo(deviceInfo);
    onvifBase->getScopes(deviceInfo);
    return deviceInfo;
}

bool OnvifDevice::getServices()
{
    return onvifBase->getServices();
}

bool OnvifDevice::getCapabilities()
{
    return onvifBase->getCapabilities();
}

QList<OnvifProfileInfo> OnvifDevice::getProfiles()
{
    return onvifBase->getProfiles();
}

QString OnvifDevice::getStreamUri(const QString &profileToken)
{
    return onvifBase->getStreamUri(profileToken);
}

bool OnvifDevice::ptzControl(quint8 type, const QString &profileToken, qreal x, qreal y, qreal z)
{
    return onvifPtz->ptzControl(type, profileToken, x, y, z);
}

bool OnvifDevice::ptzPreset(quint8 type, const QString &profileToken, const QString &presetToken, const QString &presetName)
{
    return onvifPtz->ptzPreset(type, profileToken, presetToken, presetName);
}

QList<OnvifPresetInfo> OnvifDevice::getPresets(const QString &profileToken)
{
    return onvifPtz->getPresets(profileToken);
}

void OnvifDevice::getPresetTours(const QString &profileToken)
{
    onvifPtz->getPresetTours(profileToken);
}

OnvifPresetInfo OnvifDevice::getCurrentPreset(const QString &videoSource)
{
    return onvifVideo->getCurrentPreset(videoSource);
}

bool OnvifDevice::getStatus(const QString &profileToken, qreal &x, qreal &y, qreal &z)
{
    return onvifPtz->getStatus(profileToken, x, y, z);
}

QList<OnvifVideoSource> OnvifDevice::getVideoSources()
{
    return onvifVideo->getVideoSources();
}

void OnvifDevice::getImageOption(const QString &videoSource, OnvifImageSetting &imageSetting)
{
    onvifVideo->getImageOption(videoSource, imageSetting);
}

void OnvifDevice::getImageSetting(const QString &videoSource, OnvifImageSetting &imageSetting)
{
    onvifVideo->getImageSetting(videoSource, imageSetting);
}

bool OnvifDevice::setImageSetting(const QString &videoSource, const OnvifImageSetting &imageSetting)
{
    return onvifOther->setImageSetting(videoSource, imageSetting);
}

OnvifOsdInfo OnvifDevice::getOsd(const QString &osdToken, const QSize &videoSize)
{
    return onvifVideo->getOsd(osdToken, videoSize);
}

QList<OnvifOsdInfo> OnvifDevice::getOsds(const QString &videoSource, const QSize &videoSize)
{
    return onvifVideo->getOsds(videoSource, videoSize);
}

bool OnvifDevice::setOsd(const QSize &videoSize, const OnvifOsdInfo &osd)
{
    return onvifOther->setOsd(videoSize, osd);
}

bool OnvifDevice::createOsd(const QSize &videoSize, const OnvifOsdInfo &osd)
{
    return onvifOther->createOsd(videoSize, osd);
}

bool OnvifDevice::deleteOsd(const QString &osdToken)
{
    return onvifVideo->deleteOsd(osdToken);
}

QString OnvifDevice::getEvent(const QString &timeout)
{
    return onvifEvent->getEvent(timeout);
}

void OnvifDevice::pullMessage(const QString &timeout)
{
    onvifEvent->pullMessage(timeout);
}

QString OnvifDevice::getSnapshotUri(const QString &profileToken)
{
    return onvifSnap->getSnapshotUri(profileToken);
}

QImage OnvifDevice::snapImage(const QString &profileToken)
{
    return onvifSnap->snapImage(profileToken);
}

QString OnvifDevice::systemReboot()
{
    return onvifOther->systemReboot();
}

QString OnvifDevice::getNtp()
{
    return onvifOther->getNtp();
}

bool OnvifDevice::setNtp(const QString &ntp, bool isIp)
{
    return onvifOther->setNtp(ntp, isIp);
}

OnvifNetConfig OnvifDevice::getNetConfig()
{
    return onvifOther->getNetConfig();
}

bool OnvifDevice::setNetConfig(const OnvifNetConfig &netConfig)
{
    return onvifOther->setNetConfig(netConfig);
}

QString OnvifDevice::getDateTime()
{
    return onvifOther->getDateTime();
}

bool OnvifDevice::setDateTime(const QDateTime &datetime, bool ntp)
{
    //设置时间后再立即获取时间(保证时间差值正确)
    bool ok = onvifOther->setDateTime(datetime, ntp);
    onvifOther->getDateTime();
    return ok;
}
