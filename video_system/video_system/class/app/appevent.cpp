#include "appevent.h"
#include "qthelper.h"
#include "dbquery.h"
#include "devicehelper.h"
#include "devicebutton.h"

SINGLETON_IMPL(AppEvent)
AppEvent::AppEvent(QObject *parent) : QObject(parent)
{
}

void AppEvent::slot_exitAll()
{
    //退出前保存配置文件
    AppConfig::FormFull = false;
    AppConfig::writeConfig();

    //发送退出信号
    emit exitAll();
    //延时一点时间给处理
    QtHelper::sleep(100);
}

void AppEvent::slot_mouseButtonRelease()
{
    emit mouseButtonRelease();
}

void AppEvent::slot_changeStyle()
{
    //复位下状态避免换肤后设备树状列表图标不正确
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        DbData::IpcInfo_IpcOnline[i] = false;
    }

    emit changeStyle();
}

void AppEvent::slot_changeLogo()
{
    emit changeLogo();
}

void AppEvent::slot_changeTitleInfo()
{
    emit changeTitleInfo();
}

void AppEvent::slot_changeWindowOpacity()
{
    emit changeWindowOpacity();
}

void AppEvent::slot_changeMainForm(quint8 type)
{
    emit changeMainForm(type);
}

void AppEvent::slot_changeVideoConfig()
{
    emit changeVideoConfig();
}

void AppEvent::slot_changeVideoManage()
{
    emit changeVideoManage();
}

void AppEvent::slot_restartVideo()
{
    emit restartVideo();
}

void AppEvent::slot_saveIpcInfo(bool load)
{
    if (load) {
        DbQuery::loadNvrInfo();
        DbQuery::loadIpcInfo();
        DbQuery::loadPollInfo();
        DeviceHelper::initDeviceTree();
        DeviceHelper::initVideoIcon();
        DeviceHelper::initDeviceMap();
        DeviceHelper::initDeviceButton();
        DeviceHelper::changeUrls();
    }

    emit saveIpcInfo();
}

void AppEvent::slot_doubleClicked()
{    
    DeviceButton *btn = (DeviceButton *)sender();
    emit doubleClicked(btn);
}

void AppEvent::slot_fileDragOpen()
{
    emit fileDragOpen();
}

void AppEvent::slot_fullScreen(bool full)
{
    emit fullScreen(full);
}

void AppEvent::slot_startAlarm()
{
    emit startAlarm();
}

void AppEvent::slot_stopAlarm()
{
    emit stopAlarm();
}

void AppEvent::slot_moveDevice(int id, const QString &lng, const QString &lat)
{
    emit moveDevice(id, lng, lat);
}

void AppEvent::slot_selectVideo(int channel)
{
    if (channel >= 0) {
        emit selectVideo(AppData::VideoUrls.at(channel));
    }
}

void AppEvent::slot_selectVideo(const QString &url)
{
    int channel = AppData::VideoUrls.indexOf(url);
    if (channel >= 0) {
        emit selectVideo(channel);
    }
}

void AppEvent::slot_loadVideo(int channel, int ipcID)
{
    //找到对应摄像机编号的视频流地址
    int index = DbData::IpcInfo_IpcID.indexOf(ipcID);
    if (index >= 0) {
        QString url = DbData::getRtspAddr(ipcID);
        slot_loadVideo(channel, url);
    }
}

void AppEvent::slot_loadVideo(int channel, const QString &url)
{
    if (channel < AppConfig::VideoCount) {
        emit loadVideo(channel, url);
    }
}

void AppEvent::slot_changeVideo(int channel1, int channel2)
{
    if (channel1 < AppConfig::VideoCount && channel2 < AppConfig::VideoCount && channel1 != channel2) {
        emit changeVideo(channel1, channel2);
    }
}

void AppEvent::slot_itemDoubleClicked(const QString &url, const QString &text, bool isNvr)
{
    emit itemDoubleClicked(url, text, isNvr);
}

void AppEvent::slot_receiveLngLat(const QString &lnglat)
{
    emit receiveLngLat(lnglat);
}

void AppEvent::slot_tcpConnected()
{
    emit tcpConnected();
}

void AppEvent::slot_tcpSockectData(const QVariantMap data)
{
    emit tcpSockectData(data);
}