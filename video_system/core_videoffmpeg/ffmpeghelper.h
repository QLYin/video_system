#ifndef FFMPEGHELPER_H
#define FFMPEGHELPER_H

#ifdef videoffmpeg
#include "ffmpegthread.h"
#else
class FFmpegThread;
#include "ffmpeginclude.h"
#endif
class FFmpegSave;

class FFmpegHelper
{
public:
    //获取版本
    static QString getVersion();
    //将枚举值错误代号转成字符串
    static QString getError(int errnum);
    //初始化库(一个软件中只需要初始化一次就行)
    static void initLib();

    //初始化各种参数
    static void initRtspFast(AVFormatContext *formatCtx);
    static void initOption(AVDictionary **options, int caching, const QString &transport);
    static void initOption(AVDictionary **options, const QString &bufferSize, int frameRate);
    static void initOption(AVDictionary **options, int offsetX, int offsetY, QString &videoUrl);

    //格式枚举值转字符串
    static QString getFormatString(int format, bool video);
    static QString getPixFormatString(int format);
    static QString getSampleFormatString(int format);

    //获取pts值(带矫正)
    static qint64 getPts(AVPacket *packet);
    //播放时刻值(单位秒)
    static double getPtsTime(AVFormatContext *formatCtx, AVPacket *packet);
    //播放时长值(单位秒)
    static double getDurationTime(AVFormatContext *formatCtx, AVPacket *packet);
    //延时时间值(单位微秒)
    static qint64 getDelayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime);
    //根据时间差延时
    static void delayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime);

    //硬解码格式
    static enum AVPixelFormat hw_pix_fmt;
    //获取硬解码格式回调函数
    static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const AVPixelFormat *pix_fmts);
    //根据硬解码类型找到对应的硬解码格式
    static enum AVPixelFormat find_fmt_by_hw_type(const AVHWDeviceType &type, const AVCodec *codec);

    //通用硬解码(音频没有硬解码)
    static int decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frameSrc, AVFrame *frameDst);
    //通用软解码(支持音频视频)
    static int decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video);
    //通用软编码(支持音频视频)
    static int encode(FFmpegSave *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video);

    //获取视频帧率
    static qreal getFrameRate(AVStream *stream, const QString &formatName = QString());

    //获取和设置旋转角度
    static int getRotate(AVStream *stream);
    static void setRotate(AVStream *stream, int rotate);

    //获取解码器枚举值和名称
    static AVCodecID getCodecId(AVStream *stream);
    static QString getCodecName(AVStream *stream);

    //根据分辨率获取码率
    static qint64 getBitRate(int width, int height);
    //获取码率
    static qint64 getBitRate(AVStream *stream);

    //获取格式
    static int getFormat(AVStream *stream);
    //获取类型
    static AVMediaType getMediaType(AVStream *stream);

    //获取分辨率
    static void getResolution(AVStream *stream, int &width, int &height);
    //获取采样等
    static void getAudioInfo(AVStream *stream, int &sampleRate, int &channelCount, int &profile);
    //获取流信息
    static void getStreamInfo(AVStream *stream, int &id, int &type, int &format, qint64 &bitrate, int &width, int &height, int &sampleRate, int &channelCount, int &profile);

    //获取视音频轨道
    static void getTracks(AVFormatContext *formatCtx, QList<int> &audioTracks, QList<int> &videoTracks);

    //拷贝上下文参数
    static int copyContext(AVStream *streamIn, AVStream *streamOut);
    static int copyContext(AVCodecContext *avctx, AVStream *stream, bool from);

    //转换时间基准
    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, qint64 &count, qreal fps);
    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut);
    static void rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut, qint64 &duration);

    //生成一个数据包对象
    static AVPacket *creatPacket(AVPacket *packet);

    //释放数据帧数据包
    static void freeFrame(AVFrame *frame);
    static void freePacket(AVPacket *packet);

    //超时回调(包括打开超时和读取超时/采集线程使用)
    static int openAndReadCallBack(void *ctx);
};

#endif // FFMPEGHELPER_H
