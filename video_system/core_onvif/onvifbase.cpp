#include "onvifbase.h"

OnvifBase::OnvifBase(QObject *parent) : QObject(parent)
{
    this->device = (OnvifDevice *)parent;
}

bool OnvifBase::getDeviceInfo(OnvifDeviceInfo &deviceInfo)
{
    //传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifXml::getSendData(device->getHeadData(), "GetDeviceInformation");

    //带用户认证发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->onvifAddr, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取设备信息");
    if (ok) {
        //解析设备信息
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            query.getDeviceInfo(deviceInfo);
        }
    }

    return ok;
}

bool OnvifBase::getScopes(OnvifDeviceInfo &deviceInfo)
{
    //传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifXml::getSendData(device->getHeadData(), "GetScopes");

    //带用户认证发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->onvifAddr, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取设备领域");
    if (ok) {
        //解析设备信息
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            query.getScopes(deviceInfo);
        }
    }

    return ok;
}

bool OnvifBase::getServices()
{
    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/GetServices.xml");
    file = file.arg(device->getHeadData());

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->onvifAddr, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取服务文件");
    if (ok) {
        //解析服务配置信息
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            OnvifHttpAddr httpAddr = query.getServices(device->addrPort);
            device->mediaUrl = httpAddr.addrMedia;
            device->ptzUrl = httpAddr.addrPtz;
            device->imageUrl = httpAddr.addrImaging;
            device->eventUrl = httpAddr.addrEvents;
        }
    }

    return ok;
}

bool OnvifBase::getCapabilities()
{
    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/GetCapabilities.xml");

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->onvifAddr, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取能力文件");
    if (ok) {
        //解析能力文件信息
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            OnvifHttpAddr httpAddr = query.getCapabilities(device->addrPort);
            device->mediaUrl = httpAddr.addrMedia;
            device->ptzUrl = httpAddr.addrPtz;
        }
    }

    return ok;
}

QList<OnvifProfileInfo> OnvifBase::getProfiles()
{
    QList<OnvifProfileInfo> profiles;
    if (device->mediaUrl.isEmpty()) {
        return profiles;
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/GetProfiles.xml");
    file = file.arg(device->getHeadData());

    //带用户认证发送请求数据
    QByteArray dataSend = file.toUtf8();
    //这里超时时间设置大一点,有些是NVR会有多个profile
    QNetworkReply *reply = device->request->post(device->mediaUrl, dataSend, OnvifRequest::timeout + 3000);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取媒体文件");
    if (ok) {
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            profiles = query.getProfiles();
            //主动设置一个默认的
            if (profiles.count() > 0) {
                device->profile = profiles.first().token;
            }
        }
    }

    return profiles;
}

QString OnvifBase::getStreamUri(const QString &profileToken)
{
    if (device->mediaUrl.isEmpty() || profileToken.isEmpty()) {
        return QString();
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/GetStreamUri.xml");
    file = file.arg(device->getHeadData()).arg(profileToken);

    //带用户认证发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->mediaUrl, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取视频地址");
    if (ok) {
        //解析视频流地址
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            device->rtspUrl = query.getStreamUri(device->ip, device->rtspPort);
        }
    }

    return device->rtspUrl;
}
