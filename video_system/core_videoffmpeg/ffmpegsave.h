#ifndef FFMPEGSAVE_H
#define FFMPEGSAVE_H

#include "ffmpeginclude.h"
#include "abstractsavethread.h"

//1. 网上的保存计算都是在理想状态下进行的(打开后从第一帧开始保存直到结束)
//2. 中间如果切换了播放进度(不是连续的帧)
//3. 中间暂停了流采集或者暂停了录制(不是连续的时间)
//4. 同时支持录制到文件和推流到服务器
//5. 音视频同步输出(任意切换进度自动同步)
//6. 指定只存储视频或者音频
//7. 音视频流索引顺序是0/1和1/0
//8. 不编码存储和重新编码存储
class FFmpegSave : public AbstractSaveThread
{
    Q_OBJECT
public:
    explicit FFmpegSave(QObject *parent = 0);

private:    
    //视频文件类型
    int videoType;
    //编码视频数据
    bool encodeVideo;
    //编码音频数据
    bool encodeAudio;

    //视频编码格式/视频编码帧率/视频压缩比率/视频缩放尺寸
    int videoFormat;
    int encodeVideoFps;
    float encodeVideoRatio;
    QString encodeVideoScale;

    //是否需要发送数据包信号/仅仅发送数据包
    bool sendPacket;
    bool onlySendPacket;

    //视音频启用标志位
    bool needVideo;
    bool needAudio;

    //输入流视音频索引
    int videoIndexIn;
    int audioIndexIn;

    //输出流视音频索引
    int videoIndexOut;
    int audioIndexOut;

    //视音频解码器名称
    QString videoCodecName;
    QString audioCodecName;

    //记录是否已经收到了关键帧
    bool keyFrame;
    //视频包数量和音频时长
    qint64 videoCount;
    qint64 audioDuration;

    //描述一个多媒体文件的构成及其基本信息
    AVFormatContext *formatCtx;

    //输入视音频流对象(参数传入)
    AVStream *videoStreamIn;
    AVStream *audioStreamIn;

    //视音频编码上下文
    AVCodecContext *videoCodecCtx;
    AVCodecContext *audioCodecCtx;

    //转换后的视频数据(转size)
    quint8 *videoData;
    //转换后的音频数据(转aac)
    quint8 *audioData;

    //视频帧对象(转size)
    AVFrame *videoFrame;
    //音频帧对象(转aac)
    AVFrame *audioFrame;

    //视频图像转换上下文(转size)
    SwsContext *videoSwsCtx;
    //音频数据转换上下文(转aac)
    SwrContext *audioSwrCtx;

    //视音频编码临时包
    AVPacket *videoPacket;
    AVPacket *audioPacket;

    //数据包队列(直接写入)
    QList<AVPacket *> packets;
    //数据帧队列(等待编码)
    QList<AVFrame *> frames;

private:
    //获取媒体信息
    void getMediaInfo();
    //校验视音频是否需要编码
    void checkEncode();

    //初始化视频解码器
    bool initVideoCtx();
    //初始化音频解码器
    bool initAudioCtx();

    //初始化视音频流
    bool initStream();
    bool initVideoStream();
    bool initAudioStream();

private slots:
    //初始化
    bool init();
    //保存数据
    void save();
    //关闭释放
    void close();
    //复位数据
    void reset();

public:
    //获取输入视音频流索引
    int getVideoIndexIn();
    int getAudioIndexIn();

    //获取输出视音频流索引
    int getVideoIndexOut();
    int getAudioIndexOut();

    //获取输出视音频流对象
    AVStream *getVideoStream();
    AVStream *getAudioStream();

    //打印信息
    void debug(int result, const QString &head, const QString &msg);

    //获取和设置编码视音频数据及视频数据压缩比
    bool getEncodeVideo();
    bool getEncodeAudio();
    void setEncode(bool encodeVideo, bool encodeAudio, int videoFormat, int encodeVideoFps, float encodeVideoRatio, const QString &encodeVideoScale);

    //设置是否发送数据包
    void setSendPacket(bool sendPacket, bool onlySendPacket);

    //设置参数
    void setPara(int videoType, const SaveVideoType &saveVideoType, AVStream *videoStreamIn, AVStream *audioStreamIn);

    //写入数据包
    void writePacket2(AVPacket *packet);
    void writePacket2(AVPacket *packet, bool video);

    //校验索引
    bool checkIndex(int index);
    //写入视频包
    void writePacket(AVPacket *packet, int index);
    //写入视频帧
    void writeFrame(AVFrame *frame, int index);

signals:
    //收到数据包(一般用于多路存储或推流)
    void receivePacket(AVPacket *packet);
};

#endif // FFMPEGSAVE_H
