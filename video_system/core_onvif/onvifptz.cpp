#include "onvifptz.h"

OnvifPtz::OnvifPtz(QObject *parent) : QObject(parent)
{
    device = (OnvifDevice *)parent;
}

bool OnvifPtz::ptzControl(quint8 type, const QString &profileToken, qreal x, qreal y, qreal z)
{
    if (device->ptzUrl.isEmpty() || profileToken.isEmpty()) {
        return false;
    }

    //不同的类型对应不同的文件
    QString cmd = "PtzStop";
    QString flag = "云台停止移动";
    if (type == 1) {
        cmd = "PtzAbsoluteMove";
        flag = "云台绝对移动";
    } else if (type == 2) {
        //xy=0表示此时控制焦距缩放
        if (x == 0.0 && y == 0.0) {
            cmd = "PtzRelativeMoveZoom";
            flag = "云台相对焦距";
        } else {
            cmd = "PtzRelativeMove";
            flag = "云台相对移动";
        }
    } else if (type == 3) {
        //xy=0表示此时控制焦距缩放
        if (x == 0.0 && y == 0.0) {
            cmd = "PtzContinuousMoveZoom";
            flag = "云台连续焦距";
        } else {
            cmd = "PtzContinuousMove";
            flag = "云台连续移动";
        }
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(QString(":/onvifsend/%1.xml").arg(cmd));
    QString userToken = device->getHeadData();
    if (type == 1) {
        file = file.arg(userToken).arg(profileToken).arg(x).arg(y).arg(z);
    } else if (type == 2 || type == 3) {
        //连续调整焦距命令不一样
        if (cmd.endsWith("Zoom")) {
            file = file.arg(userToken).arg(profileToken).arg(z);
        } else {
            file = file.arg(userToken).arg(profileToken).arg(x).arg(y);
        }
    } else {
        file = file.arg(userToken).arg(profileToken);
    }

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->ptzUrl, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    return device->checkData(reply, dataReceive, flag);
}

bool OnvifPtz::ptzPreset(quint8 type, const QString &profileToken, const QString &presetToken, const QString &presetName)
{
    if (device->ptzUrl.isEmpty() || profileToken.isEmpty()) {
        return false;
    }

    //不同的类型对应不同的文件
    QString cmd = "PtzGotoPreset";
    QString flag = "调用预置位";
    if (type == 1) {
        cmd = "PtzSetPreset";
        flag = "添加预置位";
    } else if (type == 2) {
        cmd = "PtzRemovePreset";
        flag = "删除预置位";
    } else if (type == 3) {
        cmd = "PtzGotoHomePosition";
        flag = "调用起始位";
    } else if (type == 4) {
        cmd = "PtzSetHomePosition";
        flag = "设置起始位";
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(QString(":/onvifsend/%1.xml").arg(cmd));
    QString head = device->getHeadData();
    if (type == 1) {
        //presetToken为空表示只是添加预置位
        //不为空表示是需要更新预置位,包括更新名称和位置
        if (presetToken.isEmpty()) {
            file = file.arg(head).arg(profileToken).arg(presetName);
        } else {
            file = OnvifHelper::getFile(QString(":/onvifsend/%1.xml").arg("PtzSetPreset2"));
            file = file.arg(head).arg(profileToken).arg(presetToken).arg(presetName);
        }
    } else if (type == 0 || type == 2) {
        file = file.arg(head).arg(profileToken).arg(presetToken);
    } else if (type == 3 || type == 4) {
        file = file.arg(head).arg(profileToken);
    }

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->ptzUrl, dataSend);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    return device->checkData(reply, dataReceive, flag);
}

QList<OnvifPresetInfo> OnvifPtz::getPresets(const QString &profileToken)
{
    QList<OnvifPresetInfo> presets;
    if (device->ptzUrl.isEmpty() || profileToken.isEmpty()) {
        return presets;
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/PtzGetPresets.xml");
    file = file.arg(device->getHeadData()).arg(profileToken);

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    //这里建议用ptzUrl,使用下来发现用mediaUrl居然也行
    //数据量比较多,这里超时时间拉长一点
    QNetworkReply *reply = device->request->post(device->ptzUrl, dataSend, OnvifRequest::timeout + 2000);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取预置位");
    if (ok) {
        //解析预置位信息
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            presets = query.getPresets();
        }
    }

    return presets;
}

void OnvifPtz::getPresetTours(const QString &profileToken)
{
    if (device->ptzUrl.isEmpty() || profileToken.isEmpty()) {
        return;
    }

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/PtzGetPresetTours.xml");
    file = file.arg(device->getHeadData()).arg(profileToken);

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->ptzUrl, dataSend, OnvifRequest::timeout);
    qDebug() << reply->readAll();
    reply->deleteLater();
}

bool OnvifPtz::getStatus(const QString &profileToken, qreal &x, qreal &y, qreal &z)
{
    if (device->ptzUrl.isEmpty() || profileToken.isEmpty()) {
        return false;
    }

    //没有赋值的话可能就是一些随机数 6.9655e-308 4.56076e+262 3.93721e-302
    x = 0;
    y = 0;
    z = 0;

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/PtzGetStatus.xml");
    file = file.arg(device->getHeadData()).arg(profileToken);

    //发送请求数据
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->ptzUrl, dataSend, OnvifRequest::timeout);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "获取云台状态");
    if (ok) {
        //解析云台状态
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            query.getStatus(x, y, z);
            return true;
        }
    }

    return false;
}
