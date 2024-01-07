#include "onvifthread.h"

QList<OnvifDevice *> OnvifThread::onvifDevices = QList<OnvifDevice *>();
OnvifDevice *OnvifThread::getOnvifDevice(const OnvifDeviceData &deviceData)
{
    OnvifDevice *device = 0;
    foreach (OnvifDevice *onvifDevice, onvifDevices) {
        if (onvifDevice->getOnvifAddr() == deviceData.onvifAddr) {
            device = onvifDevice;
            break;
        }
    }

    //不在列表中的要重新new一个
    if (!device) {
        device = new OnvifDevice;
        device->setFlag(deviceData.flag);
        onvifDevices << device;
    }

    device->setDeviceData(deviceData);
    return device;
}

QScopedPointer<OnvifThread> OnvifThread::self;
OnvifThread *OnvifThread::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new OnvifThread);
        }
    }

    return self.data();
}

OnvifThread::OnvifThread(QObject *parent) : QThread(parent)
{
    stopped = false;
    working = false;
}

OnvifThread::~OnvifThread()
{
    this->stop();
    this->wait();
}

void OnvifThread::run()
{
    while (!stopped) {
        //先判断是否在工作
        if (!working) {
            //异步执行
            QMetaObject::invokeMethod(this, "work");
        }

        //可以自行调整休息的时间
        msleep(100);
    }

    stopped = false;
    working = false;
}

void OnvifThread::stop()
{
    stopped = true;
}

void OnvifThread::work()
{
    //设置正在工作标志位
    working = true;

    if (devices.count() > 0) {
        mutex.lock();
        OnvifDevice *device = devices.takeFirst();
        QString cmd = cmds.takeFirst();
        QVariant data = datas.takeFirst();
        mutex.unlock();

        QVariantList list = data.toList();
        QString url = device->getOnvifAddr();
        if (cmd == "remove") {
#if 0
            //实际上可以不用移除(重复利用该类即可)
            bool ok = onvifDevices.removeOne(device);
            device->debug("onvif组件", QString("执行移除对象: %1").arg(ok ? "成功" : "失败"));
            //这里需要加个判断不然有空的对象进行删除导致崩溃
            if (ok) {
                device->deleteLater();
            }
#endif
        } else if (cmd == "systemReboot") {
            QString result = device->systemReboot();
            device->debug("onvif组件", QString("远程重启设备: %1").arg(result));
        } else if (cmd == "setDateTime") {
            //有两种方式设置时间 一种是直接设置日期时间字符串 一种是触发NTP同步
            bool ok = device->setDateTime(QDateTime::currentDateTime().toUTC());
            if (!ok) {
                ok = device->setDateTime(QDateTime::currentDateTime().toUTC(), true);
            }
            device->debug("onvif组件", QString("设置设备时间: %1").arg(ok ? "成功" : "失败"));
        } else if (cmd == "getServices") {
            bool ok = device->getServices();
            device->debug("onvif组件", QString("获取服务文件: %1").arg(ok ? "成功" : "失败"));
            //如果没有获取到地址则说明设备可能是 onvif1.0 重新发送1.0对应的请求数据
            if (device->getMediaUrl().isEmpty() || device->getPtzUrl().isEmpty()) {
                device->getCapabilities();
            }
        } else if (cmd == "getEvent") {
            QString result = device->getEvent();
            device->debug("onvif组件", QString("订阅报警事件: %1").arg(result));
        } else if (cmd == "snapImage") {
            QString profileToken = list.at(0).toString();
            QImage image = device->snapImage(profileToken);
            emit receiveImage(url, image);
            device->debug("onvif组件", QString("手动抓拍图片: %1 尺寸: %2x%3").arg(profileToken).arg(image.width()).arg(image.height()));
        } else if (cmd == "getVideoSources") {
            QList<OnvifVideoSource> videoSources = device->getVideoSources();
            QVariantList datas;
            foreach (OnvifVideoSource videoSource, videoSources) {
                QVariantList data;
                data << videoSource.token;
                data << videoSource.framerate;
                data << videoSource.width;
                data << videoSource.height;
                data << videoSource.brightness;
                data << videoSource.colorSaturation;
                data << videoSource.contrast;
                data << videoSource.sharpness;
                datas << data;
            }

            emit receiveResult(url, cmd, datas);
            device->debug("onvif组件", QString("获取视频配置: %1").arg(videoSources.count()));
        } else if (cmd == "getImageSetting") {
            QString videoSource = list.at(0).toString();
            OnvifImageSetting imageSetting;
            device->getImageOption(videoSource, imageSetting);
            device->getImageSetting(videoSource, imageSetting);

            QVariantList datas;
            datas << imageSetting.brightness;
            datas << imageSetting.colorSaturation;
            datas << imageSetting.contrast;
            datas << imageSetting.sharpness;
            datas << imageSetting.brightnessMin << imageSetting.brightnessMax;
            datas << imageSetting.colorSaturationMin << imageSetting.colorSaturationMax;
            datas << imageSetting.contrastMin << imageSetting.colorSaturationMax;
            datas << imageSetting.sharpnessMin << imageSetting.sharpnessMax;

            emit receiveResult(url, cmd, datas);
            device->debug("onvif组件", QString("获取图片参数: [ %1 ]").arg(videoSource));
        } else if (cmd == "setImageSetting") {
            QString videoSource = list.at(0).toString();
            OnvifImageSetting imageSetting;
            imageSetting.brightness = list.at(1).toInt();
            imageSetting.colorSaturation = list.at(2).toInt();
            imageSetting.contrast = list.at(3).toInt();
            imageSetting.sharpness = list.at(4).toInt();
            bool ok = device->setImageSetting(videoSource, imageSetting);
            device->debug("onvif组件", QString("设置图片参数: %1 [ %2 ]").arg(ok ? "成功" : "失败").arg(videoSource));
        }
    }

    working = false;
}

void OnvifThread::append(const OnvifDeviceData &deviceData, const QString &cmd, const QVariant &data)
{
    //过滤下是否有重复的(同样的地址和同样的指令则认为重复)
    int count = devices.count();
    for (int i = 0; i < count; ++i) {
        if (deviceData.onvifAddr == devices.at(i)->getOnvifAddr() && cmd == cmds.at(i)) {
            return;
        }
    }

    mutex.lock();

    //取出onvif对象并绑定信号
    OnvifDevice *device = getOnvifDevice(deviceData);
    connect(device, SIGNAL(receiveEvent(QString, OnvifEventInfo)), this, SIGNAL(receiveEvent(QString, OnvifEventInfo)), Qt::UniqueConnection);

    //添加到队列等待处理
    devices << device;
    cmds << cmd;
    datas << data;

    mutex.unlock();
}
