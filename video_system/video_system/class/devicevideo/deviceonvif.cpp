#include "deviceonvif.h"
#include "qthelper.h"
#include "dbquery.h"
#include "playwav.h"
#include "devicehelper.h"
#include "onvifthread.h"
#include "videomanage.h"
#include "urlhelper.h"

bool DeviceOnvif::checkUrl(const QString &url, OnvifDeviceData &deviceData)
{
    //不是rtsp开头也不是摄像机,为空也包含在这个判断中
    if (!url.startsWith("rtsp")) {
        return false;
    }

    //可能是主码流也可能是子码流
    int index1 = DbData::IpcInfo_RtspMain.indexOf(url);
    int index2 = DbData::IpcInfo_RtspSub.indexOf(url);
    int index = -1;
    if (index1 >= 0) {
        index = index1;
    } else if (index2 >= 0) {
        index = index2;
    }

    //没有码流地址不用继续
    if (index < 0) {
        return false;
    }

    //只有onvif地址存在才是onvif设备
    QString onvifAddr = DbData::IpcInfo_OnvifAddr.at(index);
    if (onvifAddr.isEmpty()) {
        return false;
    }

    //onvif地址中的IP和rtsp地址中的IP必须一致
    //为什么会出现这个现象(因为用户很可能直接在原来的正确的带有onvif地址的信息中修改了rtsp地址)
    if (UrlHelper::getUrlIP(onvifAddr) != UrlHelper::getUrlIP(url)) {
        return false;
    }

    //设置唯一标识
    QList<VideoWidget *> videoWidgets = VideoManage::Instance()->getVideoWidgets();
    foreach (VideoWidget *videoWidget, videoWidgets) {
        if (videoWidget->getVideoPara().videoUrl == url) {
            deviceData.flag = videoWidget->getWidgetPara().videoFlag;
            break;
        }
    }

    //对应结构体数据赋值
    deviceData.userName = DbData::IpcInfo_UserName.at(index);
    deviceData.userPwd = DbData::IpcInfo_UserPwd.at(index);
    deviceData.onvifAddr = DbData::IpcInfo_OnvifAddr.at(index);
    deviceData.profileToken = DbData::IpcInfo_ProfileToken.at(index);
    deviceData.videoSource = DbData::IpcInfo_VideoSource.at(index);
    return true;
}

QList<OnvifPresetInfo> DeviceOnvif::getPresets(const QString &url)
{
    QList<OnvifPresetInfo> presets;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        presets = device->getPresets(deviceData.profileToken);
        device->debug("onvif组件", QString("获取预置位置: %1").arg(presets.count()));
    }
    return presets;
}

bool DeviceOnvif::ptzControl(quint8 type, const QString &url, double x, double y, double z)
{
    bool result = false;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        result = device->ptzControl(type, deviceData.profileToken, x, y, z);
        device->debug("onvif组件", QString("执行云台控制: %1").arg(result));
    }
    return result;
}

bool DeviceOnvif::ptzPreset(quint8 type, const QString &url, const QString &presetToken, const QString &presetName)
{
    bool result = false;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        result = device->ptzPreset(type, deviceData.profileToken, presetToken, presetName);
        device->debug("onvif组件", QString("预置位置处理: %1").arg(result));
    }
    return result;
}

QList<OnvifOsdInfo> DeviceOnvif::getOSDs(const QString &url)
{
    QList<OnvifOsdInfo> osds;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        VideoWidget *widget = VideoManage::Instance()->getVideoWidget(url, url);
        if (widget) {
            osds = device->getOsds(deviceData.videoSource, QSize(widget->getVideoWidth(), widget->getVideoHeight()));
            device->debug("onvif组件", QString("获取OSD集合: %1").arg(osds.count()));
        }
    }
    return osds;
}

bool DeviceOnvif::osdControl(quint8 type, const QString &url, const OnvifOsdInfo &osd)
{
    bool result = false;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        VideoWidget *widget = VideoManage::Instance()->getVideoWidget(url, url);
        if (widget) {
            QString flag;
            QSize size(widget->getVideoWidth(), widget->getVideoHeight());
            if (type == 0) {
                flag = "创建OSD信息";
                result = device->createOsd(size, osd);
            } else if (type == 1) {
                flag = "设置OSD信息";
                result = device->setOsd(size, osd);
            } else if (type == 2) {
                flag = "删除OSD信息";
                result = device->deleteOsd(osd.token);
            }

            device->debug("onvif组件", QString("%1: %2").arg(flag).arg(result));
        }
    }
    return result;
}

OnvifNetConfig DeviceOnvif::getNetConfig(const QString &url)
{
    OnvifNetConfig netConfig;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        netConfig = device->getNetConfig();
        device->debug("onvif组件", QString("获取网络配置: %1").arg(netConfig.ipAddress == "255.255.255.255" ? "false" : "true"));
    }
    return netConfig;
}

bool DeviceOnvif::setNetConfig(const QString &url, const OnvifNetConfig &netConfig)
{
    bool result = false;
    OnvifDeviceData deviceData;
    if (checkUrl(url, deviceData)) {
        OnvifDevice *device = OnvifThread::getOnvifDevice(deviceData);
        result = device->setNetConfig(netConfig);
    }
    return result;
}

SINGLETON_IMPL(DeviceOnvif)
DeviceOnvif::DeviceOnvif(QObject *parent) : QThread(parent)
{
    //注册数据类型
    qRegisterMetaType<QVector<int> >("QVector<int>");

    //显示截图的标签
    labImage.setWindowFlags(Qt::WindowStaysOnTopHint);
    labImage.setFixedSize(QSize(800, 600));
    labImage.setWindowTitle("抓拍图片预览");
    QtHelper::setFormInCenter(&labImage);

    //关联信号
    connect(OnvifThread::Instance(), SIGNAL(receiveImage(QString, QImage)), this, SLOT(receiveImage(QString, QImage)));
    connect(OnvifThread::Instance(), SIGNAL(receiveEvent(QString, OnvifEventInfo)), this, SLOT(receiveEvent(QString, OnvifEventInfo)));
    connect(OnvifThread::Instance(), SIGNAL(receiveResult(QString, QString, QVariant)), this, SIGNAL(receiveResult(QString, QString, QVariant)));

    //加载警情字典
    this->initData();
    //启动onvif线程
    OnvifThread::Instance()->start();
    //启动本线程
    stopped = false;
    this->start();
}

DeviceOnvif::~DeviceOnvif()
{
    stopped = true;
    this->wait();
}

void DeviceOnvif::run()
{
    finsh = true;
    currentIndex = 0;
    while (!stopped) {
        //优先处理图文警情信息
        if (listMsg.count() > 0) {
            //先延时一下以便抓图的文件确保生成
            msleep(100);
            mutex.lock();
            QString msg = listMsg.takeFirst();
            QString file = listFile.takeFirst();
            mutex.unlock();
            QMetaObject::invokeMethod(this, "addMsgList", Q_ARG(QString, msg), Q_ARG(QString, "待处理!"), Q_ARG(QImage, QImage(file)));
            continue;
        }

        //没有启用离线检测则不用继续
        if (!AppConfig::CheckOffline || DbData::IpcInfo_Count == 0) {
            msleep(100);
            continue;
        }

        //到了最后一个说明一轮完成了需要多休息一下
        if (finsh) {
            finsh = false;
            //下面的写法在保证延时很长一个时间的中途还能立即跳出而不是等延时完成
            //msleep(5000);
            int count = 0;
            while (!stopped && listMsg.count() == 0) {
                msleep(100);
                count++;
                if (count == 50) {
                    break;
                }
            }
        } else {
            msleep(10);
        }

        //判断设备是否在线(为什么这里放在下面执行是为了保证首次处理对应主界面设备树状控件全部加载完成)
        this->checkOnline();
    }

    stopped = false;
}

void DeviceOnvif::initData()
{
    eventLevel.clear();
    eventName.clear();
    eventAlarm.clear();
    eventNormal.clear();

    QFile file(QtHelper::appPath() + "/config/event.txt");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        //每次读取一行
        QString line = in.readLine();
        //去除空格回车换行
        line = line.trimmed();
        line.replace("\r", "");
        line.replace("\n", "");
        //空行或者注释行不用处理
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        //格式: 1,IsMotion,移动报警,移动结束
        QStringList list = line.split(",");
        if (list.count() != 4) {
            continue;
        }

        eventLevel << list.at(0).toInt();
        eventName << list.at(1);
        eventAlarm << list.at(2);
        eventNormal << list.at(3);
    }
}

void DeviceOnvif::checkOnline()
{
    QString url = DbData::getRtspAddr(currentIndex);
    QString ip = UrlHelper::getUrlIP(url);

    //rtsp除外的认为永远存在(可以根据需要自行约定规则)
    bool online = true;
    if (url.startsWith("rtsp")) {
        online = false;
        bool exist = false;
#if 0
        //找到每个已经播放的视频控件对应地址是否和当前要处理的地址一样
        int index = -1;
        int count = AppData::VideoUrls.count();
        for (int i = 0; i < count; ++i) {
            if (UrlHelper::getUrlIP(AppData::VideoUrls.at(i)) == ip) {
                index = i;
                break;
            }
        }

        //如果一样则跳过检测(尽可能将资源留给重连中的视频控件)
        if (index >= 0) {
            VideoWidget *videoWidget = VideoManage::Instance()->getVideoWidgets().at(index);
            if (videoWidget->getIsRunning() && videoWidget->getVideoThread()->getVideoWidth() == 0) {
                exist = true;
            }
        }
#endif
        if (!exist) {
            online = DeviceHelper::checkOnline(url);
        }
    }

    //过滤下只有当状态变化了才需要
    if (online) {
        if (!DbData::IpcInfo_IpcOnline.at(currentIndex)) {
            DeviceHelper::setVideoIcon2(ip, true);
        }
    } else {
        if (DbData::IpcInfo_IpcOnline.at(currentIndex)) {
            DeviceHelper::setVideoIcon2(ip, false);
        }
    }

    //qDebug() << TIMEMS << currentIndex << online << ip;
    DbData::IpcInfo_IpcOnline[currentIndex] = online;

    //每次递增直到末尾则重新开始
    currentIndex++;
    if (currentIndex == DbData::IpcInfo_Count) {
        finsh = true;
        currentIndex = 0;
    }
}

void DeviceOnvif::receivePlayStart(int time)
{
    //轮询阶段不处理
    if (AppConfig::Polling) {
        //return;
    }

    //如果没有启用任何服务则不用继续
    if (!AppConfig::OnvifNtp && !AppConfig::OnvifEvent) {
        //return;
    }

    //拿到触发信号的控件
    VideoWidget *videoWidget = (VideoWidget *)sender();
    //先校验当前视频对应的信息是否符合
    OnvifDeviceData deviceData;
    if (checkUrl(videoWidget->getVideoPara().videoUrl, deviceData)) {
        //交给线程执行指令
        OnvifThread::Instance()->append(deviceData, "getServices");
        if (AppConfig::OnvifEvent) {
            OnvifThread::Instance()->append(deviceData, "getEvent");
        }
        if (AppConfig::OnvifNtp) {
            OnvifThread::Instance()->append(deviceData, "setDateTime");
        }
    }
}

void DeviceOnvif::receivePlayFinsh()
{
    //先校验当前视频对应的信息是否符合
    VideoWidget *videoWidget = (VideoWidget *)sender();
    OnvifDeviceData deviceData;
    if (checkUrl(videoWidget->getVideoPara().videoUrl, deviceData)) {
        //交给线程执行指令
        OnvifThread::Instance()->append(deviceData, "remove");
    }
}

void DeviceOnvif::receiveImage(const QString &url, const QImage &image)
{
    QImage img = image;
    if (!img.isNull()) {
        //等比例缩放一下
        img = img.scaled(labImage.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        labImage.setPixmap(QPixmap::fromImage(img));
        labImage.show();
    }
}

void DeviceOnvif::receiveEvent(const QString &url, const OnvifEventInfo &event)
{
    //可能临时关闭了事件订阅
    if (!AppConfig::OnvifEvent) {
        return;
    }

    //事件内容存放在结构体数据中
    //qDebug() << TIMEMS << event;
    QString name = event.dataName;
    QVariant value = event.dataValue;

    //有多种关键字可以自行过滤需要的
    if (!eventName.contains(name)) {
        //对应收到的未知的报警可以自行根据厂家规则约定加到 config/event.txt 事件字典表中
        qDebug() << TIMEMS << "未知报警事件" << QtHelper::getIP(url) << name << value;
        return;
    } else {
        qDebug() << TIMEMS << "收到报警事件" << QtHelper::getIP(url) << name << value;
    }

    //过滤不在本系统中的设备发过来的报警
    int index = DbData::IpcInfo_OnvifAddr.indexOf(url);
    if (index < 0) {
        return;
    }

    //true/1 false/0 字符转成bool类型方便统一判断
    bool alarm = value.toBool();
    QString ipcName = DbData::IpcInfo_IpcName.at(index);
    int i = eventName.indexOf(name);
    QString info = (alarm ? eventAlarm.at(i) : eventNormal.at(i));

    //添加到信息栏
    QString msg = QString("%1%2").arg(ipcName).arg(info);
    DeviceHelper::addMsg(msg, alarm ? 2 : 0);

    //抓拍对应通道图像
    QString flag, fileName;
    if (alarm) {
        QString rtspMain = DbData::IpcInfo_RtspMain.at(index);
        QString rtspSub = DbData::IpcInfo_RtspSub.at(index);
        VideoWidget *videoWidget = VideoManage::Instance()->getVideoWidget(rtspMain, rtspSub);
        if (videoWidget) {
            flag = videoWidget->objectName();
            fileName = QString("%1/image_alarm/%2/%3_%4.jpg").arg(QtHelper::appPath()).arg(QDATE).arg(flag).arg(STRDATETIMEMS);
            videoWidget->snap(fileName);

            //放入队列线程处理(保证抓图完成)
            mutex.lock();
            listMsg << info;
            listFile << fileName;
            mutex.unlock();
        }
    }

    //右下角弹出提示
    if (AppConfig::TipInterval != 10000) {
        QtHelper::showTipBox("提示", msg, AppConfig::FullScreen, true, AppConfig::TipInterval);
    }

    //暂定开关量报警弹出报警视频并录像
    if (alarm && info == "开关量报警") {
        QString url = DbData::getRtspAddr(index);
        //弹出了报警视频则日志记录中优先存储报警录像文件的名称(直接用报警图片的路径以便统一)
        fileName.replace(".jpg", ".mp4");
        fileName.replace("image_alarm", "video_alarm");
        DeviceHelper::showVideo(url, flag, AppConfig::AlarmSaveTime, fileName);
    }

    //播放报警声音并插入到日志记录数据库(报警带上报警图片路径)
    if (alarm) {
        DeviceHelper::playAlarm("8.wav");
        DbQuery::addUserLog(flag.right(2), "报警日志", msg, fileName);
    } else {
        DeviceHelper::stopSound();
        DbQuery::addUserLog("报警日志", msg);
    }
}

void DeviceOnvif::addMsgList(const QString &msg, const QString &result, const QImage &image)
{
    if (!image.isNull()) {
        DeviceHelper::addMsgList(msg, result, image);
    }
}
