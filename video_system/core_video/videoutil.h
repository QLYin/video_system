#ifndef VIDEOUTIL_H
#define VIDEOUTIL_H

#include "videohead.h"

class VideoUtil
{
public:
    //获取当前视频内核版本
    static QString getVersion();
    static QString getVersion(const VideoCore &videoCore);

    //设置解码内核(type可以指定内核/否则按照定义的优先级)
    static VideoCore getVideoCore(int type = 0);
    //获取声卡设备名称
    static QStringList getAudioDevice(const VideoCore &videoCore);

    //创建视频采集类
    static VideoThread *newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode);

    //重命名录制的文件(vlc内核专用)
    static void renameFile(const QString &fileName);
    //鼠标指针复位
    static void resetCursor();

    //加载解析内核到下拉框
    static void loadVideoCore(QComboBox *cbox, int &videoCore, bool simple = false);
    //加载视频地址到下拉框
    static void loadVideoUrl(QComboBox *cbox, const QString &videoUrl, int type);
    //加载画面策略到下拉框
    static void loadScaleMode(QComboBox *cbox);
    //加载视频模式到下拉框
    static void loadVideoMode(QComboBox *cbox);
    //加载解码策略到下拉框
    static void loadDecodeType(QComboBox *cbox);
    //加载编码策略到下拉框
    static void loadEncodeType(QComboBox *cbox);
    //加载硬件加速名称到下拉框
    static void loadHardware(QComboBox *cbox, const VideoCore &videoCore, QString &hardware);
    //加载通信协议到下拉框
    static void loadTransport(QComboBox *cbox);
    //加载缓存时间到下拉框
    static void loadCaching(QComboBox *cbox, const VideoCore &videoCore, int &caching);

    //加载打开间隔到下拉框
    static void loadOpenSleepTime(QComboBox *cbox);
    //加载读取超时到下拉框
    static void loadReadTimeout(QComboBox *cbox);
    //加载连接超时到下拉框
    static void loadConnectTimeout(QComboBox *cbox);

    //加载倍速到下拉框
    static void loadSpeed(QComboBox *cbox);

    //写入数据到文件
    static void writeData(QFile *file, char *data, qint64 len);
};

#endif // VIDEOUTIL_H
