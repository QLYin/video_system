#ifndef FFMPEGUTIL_H
#define FFMPEGUTIL_H

#include "ffmpeghelper.h"

class FFmpegUtil
{
public:
    //打印输出编码解码信息 https://blog.csdn.net/xu13879531489/article/details/80703465
    static void debugCodec();
    //查看硬解码 ffmpeg -hwaccels
    static QStringList getHardware();
    //打印编码器的相关参数
    static void debugPara(AVCodecContext *codecCtx);

    //打印设备列表和参数 type: vfwcap dshow v4l2 avfoundation
    static void showDevice();
    static void showDevice(const char *flag);
    static void showDeviceOption(const QString &device);
    static void showDeviceOption(const char *flag, const QString &device);

//获取输入输出设备名称集合
#ifdef ffmpegdevice
    static QStringList getInputDevices(bool video);
    static QStringList getDeviceNames(bool input, bool video);
    static QStringList getDeviceNames(AVDeviceInfoList *devices, bool video);
#endif

    //视频帧旋转
    static void rotateFrame(int rotate, AVFrame *frameSrc, AVFrame *frameDst);

    //获取指定文件时长
    static qint64 getDuration(const QString &fileName, quint64 *useTime = NULL);

    //数据转字符串
    static QString dataToString(char *data, qint64 len);
    static QStringList dataToStringList(char *data, qint64 len);
};

#endif // FFMPEGUTIL_H
