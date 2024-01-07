#ifndef ONVIFPTZ_H
#define ONVIFPTZ_H

#include "onvifhead.h"

class OnvifPtz : public QObject
{
    Q_OBJECT
public:
    explicit OnvifPtz(QObject *parent = 0);

private:
    //onvif设备对象
    OnvifDevice *device;

public:
    //云台控制 type: 0-停止移动 1-绝对移动 2-相对移动 3-连续移动
    bool ptzControl(quint8 type, const QString &profileToken, qreal x, qreal y, qreal z);
    //预置位处理 type: 0-调用预置位 1-添加预置位 2-删除预置位 3-调用起始位 4-设置起始位
    bool ptzPreset(quint8 type, const QString &profileToken, const QString &presetToken = QString(), const QString &presetName = QString());
    //获取预置位集合
    QList<OnvifPresetInfo> getPresets(const QString &profileToken);
    //获取预置位巡航
    void getPresetTours(const QString &profileToken);
    //获取云台状态
    bool getStatus(const QString &profileToken, qreal &x, qreal &y, qreal &z);
};

#endif // ONVIFPTZ_H
