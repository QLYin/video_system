#ifndef APPEVENT_H
#define APPEVENT_H

#include "onvifhead.h"
#include "singleton.h"
class DeviceButton;

class AppEvent : public QObject
{
    Q_OBJECT SINGLETON_DECL(AppEvent)

public:
    explicit AppEvent(QObject *parent = 0);

signals:
    void exitAll();
    void mouseButtonRelease();

    void changeStyle();
    void changeLogo();
    void changeTitleInfo();
    void changeWindowOpacity();
    void changeMainForm(quint8 type);

    void changeVideoConfig();
    void changeVideoManage();

    void restartVideo();
    void saveIpcInfo();
    void doubleClicked(DeviceButton *btn);

    void receivePoint(int type, const QPoint &point);
    void fileDragOpen();
    void fullScreen(bool full);

    void startAlarm();
    void stopAlarm();
    void moveDevice(int id, const QString &lng, const QString &lat);

    void selectVideo(int channel);
    void selectVideo(const QString &url);

    void loadVideo(int channel, const QString &url);
    void changeVideo(int channel1, int channel2);

    void itemDoubleClicked(const QString &url, const QString &text, bool isNvr);
    void receiveLngLat(const QString &lnglat);

public slots:
    //软件退出
    void slot_exitAll();
    //鼠标右键松开
    void slot_mouseButtonRelease();

    //全局样式改变
    void slot_changeStyle();
    //软件图标改变
    void slot_changeLogo();
    //软件标题栏相关更改比如标题和右上角显示开关等
    void slot_changeTitleInfo();
    //窗体透明度值改变
    void slot_changeWindowOpacity();
    //主窗体变化 0-最小化 1-最大化 2-恢复 3-关闭 4-移动 5-尺寸变化
    void slot_changeMainForm(quint8 type);

    //视频控件参数更改
    void slot_changeVideoConfig();
    //视频管理参数更改
    void slot_changeVideoManage();

    //重新加载所有通道
    void slot_restartVideo();
    //保存摄像机信息
    void slot_saveIpcInfo(bool load = false);
    //设备按钮双击
    void slot_doubleClicked();

    //拖曳打开完成
    void slot_fileDragOpen();
    //全屏切换
    void slot_fullScreen(bool full);

    //报警图标开始闪烁
    void slot_startAlarm();
    //报警图标停止闪烁
    void slot_stopAlarm();
    //指定设备移动 id从0开始
    void slot_moveDevice(int id, const QString &lng, const QString &lat);

    //选中通道-视频通道按下触发通知节点通道选中
    void slot_selectVideo(int channel);
    //选中通道-节点通道按下触发通知视频通道选中
    void slot_selectVideo(const QString &url);

    //指定通道显示视频 id从0开始
    void slot_loadVideo(int channel, int ipcID);
    void slot_loadVideo(int channel, const QString &url);
    //通道交换
    void slot_changeVideo(int channel1, int channel2);

    //双击树状列表控件
    void slot_itemDoubleClicked(const QString &url, const QString &text, bool isNvr);
    //机器人经纬度改变
    void slot_receiveLngLat(const QString &lnglat);
};

#endif // APPEVENT_H
