#ifndef DEVICEHELPER_H
#define DEVICEHELPER_H

#include "head.h"
class VideoWidget;
class frmMsgList;

#ifndef TIME
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#endif

class DeviceHelper
{
public:
    //设置对应的控件
    static void setLabel(QLabel *label);
    static void setTableWidget(QTableWidget *tableWidget);
    static void setTreeWidget(QTreeWidget *treeWidget);
    static void setListWidget(QListWidget *listWidget);    
    static void setMsgList(frmMsgList *msgList);

    //添加+清空临时消息
    static void initDeviceTable();
    //type 0-正常消息 1-报警消息 log=true表示要插入到数据库记录
    static void addMsg(const QString &msg, quint8 type = 0, bool log = false);
    static void clearMsg();

    //添加卡片图文消息
    static void addMsgList(const QString &msg, const QString &result, const QImage &image, const QString &time = TIME);
    static void clearMsgList();

    //加载树状列表
    static void initDeviceTree();
    static void initDeviceTree(QTreeWidget *treeWidget);

    //初始化图标
    static void initVideoIcon();
    //改变图标比如上下线
    static void setVideoIcon(const QString &url, bool online = true);
    static void setVideoIcon2(const QString &ip, bool online = true);

    //加载地图
    static void initDeviceMap();
    static void initDeviceMap(QListWidget *listWidget);

    //显示当前地图设备
    static void initDeviceMapCurrent(const QString &imgName);
    static void initDeviceMapCurrent(QLabel *label, const QString &imgName);

    //初始化设备按钮+面板
    static void initDeviceButton();
    static void initDeviceButton(QLabel *label);

    static void saveDeviceButtonPosition();
    static void changeDeviceButtonPosition();

    //加载/保存/清空/更新 通道视频地址
    static int initUrls();
    static void saveUrls();
    static void clearUrls();
    static void changeUrls();

    //获取设备信息
    static void getDeviceInfo(QStringList &names, QStringList &addrs, QStringList &points);
    //检查地址在线
    static bool checkOnline(const QString &url);
    //弹出预览视频
    static void showVideo(const QString &url, const QString &flag = QString(), int recordTime = 0, const QString &fileName = QString());

    //报警视频回放
    static void showAlarmVideo(const QString &url);
    //报警图片预览
    static void showAlarmImage(const QString &url);

    //通用初始化视频控件
    static void initVideoWidget(VideoWidget *videoWidget, int soundValue = 100, bool soundMuted = false, bool bannerEnable = false, int readTimeout = 0);

    //播放声音
    static void playAlarm(const QString &soundName, bool stop = false);
    static void playSound(const QString &soundName, int playCount = 1, bool stop = true);
    static void stopSound();
};

#endif // DEVICEHELPER_H
