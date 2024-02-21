#include "onvifsnap.h"

OnvifSnap::OnvifSnap(QObject *parent) : QObject(parent)
{
    device = (OnvifDevice *)parent;
}

QString OnvifSnap::getSnapshotUri(const QString &profileToken)
{
    if (device->mediaUrl.isEmpty() || profileToken.isEmpty()) {
        return QString();
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/GetSnapshotUri.xml");
    file = file.arg(device->getHeadData()).arg(profileToken);

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->mediaUrl, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "请求截图");
    if (ok) {
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            device->snapUrl = query.getSnapshotUri(device->addrPort);
            if (!device->snapUrl.isEmpty()) {
                //重新加上用户认证,取图片也需要认证的
                emit receiveInfo(QString("抓图地址 -> %1").arg(device->snapUrl));
                QString userInfo = QString("http://%1:%2@").arg(device->userName).arg(device->userPwd);
                device->snapUrl.replace("http://", userInfo);
            }
        }
    }

    return device->snapUrl;
}

QImage OnvifSnap::snapImage(const QString &profileToken)
{
    //这里还可以考虑过滤,比如已经存在了 snapUrl 则不需要去获取地址
    getSnapshotUri(profileToken);

    QImage image;
    if (!device->snapUrl.isEmpty() && !profileToken.isEmpty()) {
        //请求图片数据回复 1080P 大概需要 600ms
        //默认采用的是同步阻塞获取,并不会卡主界面
        QNetworkReply *reply = device->request->get(device->snapUrl);
        QByteArray dataReceive;
        bool ok = device->checkData(reply, dataReceive, "收到截图");
        if (ok) {
            //下面这行代码非常耗时 1080P 图片大概需要 80ms 如果需要频繁的截图建议放入线程处理
            image.loadFromData(dataReceive);
        }
    }

    return image;
}
