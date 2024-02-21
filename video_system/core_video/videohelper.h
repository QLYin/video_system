#ifndef VIDEOHELPER_H
#define VIDEOHELPER_H

#include "videohead.h"

class VideoHelper
{
public:
    //获取当前视频内核版本
    static QString getVersion();
    static QString getVersion(const VideoCore &videoCore);
    //设置解码内核(type可以指定内核/否则按照定义的优先级)
    static VideoCore getVideoCore(int type = 0);

    //重命名录制的文件(vlc内核专用)
    static void renameFile(const QString &fileName);
    //鼠标指针复位
    static void resetCursor();

    //根据旋转角度旋转图片
    static void rotateImage(int rotate, QImage &image);
    //根据旋转度数交换宽高
    static void rotateSize(int rotate, int &width, int &height);
    //根据旧的范围值和值计算新的范围值对应的值
    static int getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax);

    //校验网络地址是否可达
    static bool checkUrl(const QString &url, int timeout = 500);
    //检查地址是否正常(文件是否存在或者网络地址是否可达)
    static bool checkUrl(VideoThread *videoThread, const VideoType &videoType, const QString &videoUrl, int timeout = 500);
    //获取转义后的地址(有些视频流带了用户信息有特殊字符需要先转义)
    static QString getRightUrl(const VideoType &videoType, const QString &videoUrl);
    //区分本地文件和网络文件
    static QUrl getUrl(const VideoType &videoType, const QString &videoUrl);

    //根据地址获取类型
    static VideoType getVideoType(const QString &videoUrl);
    //根据地址获取是否只是音频
    static bool getOnlyAudio(const QString &videoUrl, const QString &formatName = QString());

    //根据地址获取本地摄像头参数
    static void getCameraPara(const VideoCore &videoCore, QString &videoUrl, QString &bufferSize, int &frameRate);
    //根据地址获取桌面录屏参数
    static void getDesktopPara(const VideoCore &videoCore, QString &videoUrl, QString &bufferSize, int &frameRate, int &offsetX, int &offsetY, QString &encodeScale);
    //根据地址获取其他参数
    static void getNormalPara(QString &videoUrl, QString &transport, EncodeType &encodeType, VideoFormat &videoFormat, int &encodeVideoFps, float &encodeVideoRatio, QString &encodeVideoScale);

    //对参数进行矫正
    static VideoType initPara(WidgetPara &widgetPara, VideoPara &videoPara);

    //创建视频采集类
    static VideoThread *newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode);
    //对采集线程设置参数
    static void initVideoThread(VideoThread *videoThread, const VideoPara &videoPara);

    //根据地址取出本地设备或者桌面的参数以及其他处理(用于单独的解码线程初始化使用/推流只需要用解码线程不需要视频控件)
    static void initVideoPara(VideoThread *videoThread, const QString &videoUrl);
    static void initVideoPara(VideoThread *videoThread, const QString &videoUrl, QString &encodeVideoScale);
};

#endif // VIDEOHELPER_H
