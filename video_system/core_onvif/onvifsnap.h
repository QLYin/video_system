#ifndef ONVIFSNAP_H
#define ONVIFSNAP_H

#include "onvifhead.h"

class OnvifSnap : public QObject
{
    Q_OBJECT
public:
    explicit OnvifSnap(QObject *parent = 0);

private:
    //onvif设备对象
    OnvifDevice *device;

public:
    //获取抓图地址
    QString getSnapshotUri(const QString &profileToken);
    //抓拍图片
    QImage snapImage(const QString &profileToken);

signals:
    void receiveInfo(const QString &data);
};

#endif // ONVIFSNAP_H
