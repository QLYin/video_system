#ifndef FFMPEGTHREAD_H
#define FFMPEGTHREAD_H

#include "ffmpeginclude.h"
#include "ffmpegstruct.h"
#include "videothread.h"
class FFmpegSync;
class AudioPlayer;

class FFmpegThread : public VideoThread
{
    Q_OBJECT
public:
    //解码同步线程定义成友元类(这样可以直接使用主解码线程的变量)
    friend class FFmpegSync;
    explicit FFmpegThread(QObject *parent = 0);
    ~FFmpegThread();

protected:
    void run();

private:
    //正在切换进度
    volatile bool changePosition;
    //开启推流预览(推流用)
    volatile bool pushPreview;
    //清空缓存标志位(推流用)
    volatile bool needClear;
    //是否收到关键帧(推流用)
    volatile bool keyFrame;

    //播放速度
    double speed;
    //视频流索引
    int videoIndex;
    //音频流索引
    int audioIndex;

    //是否已经尝试打开过
    bool tryOpen;
    //是否已经尝试读取过
    bool tryRead;

    //统计多久一个关键帧
    qint64 keyPacketIndex;
    qint64 keyPacketCount;

    //实时码率(帧数总包大小)
    qint64 realPacketSize;
    qint64 realPacketCount;

    //视频首个包的 pts start_time
    qint64 videoFirstPts;
    //音频首个包的 pts start_time
    qint64 audioFirstPts;

    //解码开始时间
    qint64 startTime;
    //文件格式
    QString formatName;
    //视频解码器名称
    QString videoCodecName;
    //音频解码器名称
    QString audioCodecName;

    //待解码数据包
    AVPacket *packet;
    //视频帧对象
    AVFrame *videoFrame;
    //音频帧对象
    AVFrame *audioFrame;

    //视频帧对象(转yuv420)
    AVFrame *yuvFrame;
    //视频帧对象(转rgb)
    AVFrame *imageFrame;
    //视频帧对象(中转用)
    AVFrame *tempFrame;

    //解码后的视频数据(转yuv420)
    quint8 *yuvData;
    //解码后的视频数据(转rgb)
    quint8 *imageData;
    //解码后的音频数据(转pcm)
    quint8 *pcmData;

    //视频图像转换上下文(转yuv420)
    SwsContext *yuvSwsCtx;
    //视频图像转换上下文(转rgb)
    SwsContext *imageSwsCtx;
    //音频数据转换上下文(转pcm)
    SwrContext *pcmSwrCtx;

    //参数字典
    AVDictionary *options;
    //输入格式
    AVInputFormatx *ifmt;
    //描述一个多媒体文件的构成及其基本信息
    AVFormatContext *formatCtx;

    //视频解码器
    AVCodecx *videoCodec;
    //音频解码器
    AVCodecx *audioCodec;

    //视频解码器上下文
    AVCodecContext *videoCodecCtx;
    //音频解码器上下文
    AVCodecContext *audioCodecCtx;

    //启用音视频同步
    bool useSync;
    //视频解码同步线程
    FFmpegSync *videoSync;
    //音频解码同步线程
    FFmpegSync *audioSync;

    //恢复暂停定时器
    QTimer *timerPause;
    //切换进度定时器
    QTimer *timerPosition;

    //音频播放类
    AudioPlayer *audioPlayer;

    //裁剪滤镜标志位
    bool isCrop;
    //取消裁剪状态的时间
    QDateTime cropTime;

    //计数用于延迟应用新的滤镜
    int filterCount;
    //视频滤镜数据结构体
    FilterData videoFilter;    

private:
    //读取并清空(视频流暂停期间)
    void readAndClear();
    //重新播放(循环播放)
    void replay();
    //重新打开(重连打开)
    void reopen();

    //统计I帧(关键帧)间隔
    void checkKeyPacketCount(AVPacket *packet);
    //计算实时码率
    void checkRealPacketSize(AVPacket *packet, int maxCount);

    //动态监测尺寸变化
    bool checkFrameSize(AVFrame *frame);
    //转换和保存视频
    bool scaleAndSaveVideo(bool &needScale, AVFrame *frame);
    //处理和显示视频
    void checkAndShowVideo(bool needScale, AVFrame *frame);

public slots:
    //解码视频
    void decodeVideo0(AVPacket *packet);
    void decodeVideo1(AVPacket *packet);
    void decodeVideo2(AVPacket *packet);

    //解码音频
    void decodeAudio0(AVPacket *packet);
    void decodeAudio1(AVPacket *packet);
    void decodeAudio2(AVPacket *packet);

    //写入视音频数据到文件
    void writeFile(AVPacket *packet, bool video);
    void writeFile(AVFrame *frame, bool video);

private slots:
    //初始化参数
    void initOption();
    //初始化输入
    bool initInput();

    //初始化视频
    bool initVideo();
    //初始化硬解码
    bool initHardware();
    //初始化音频
    bool initAudio();

    //分配各种内存(packet/frame/data)
    void initData();
    //检查各种参数
    void checkPara();
    //初始化音频播放
    void initAudioPlayer();
    //打开音频输出
    void openAudioOutput(int sampleRate, int channelCount, int sampleSize, int volume);
    //初始化专辑
    void initMetadata();

    //初始化滤镜
    void initFilter();
    //重置滤镜
    void resetFilter();

    //打开视频
    bool openVideo();
    //关闭视频
    void closeVideo();

public slots:
    //获取初始化的时间
    qint64 getStartTime();
    //获取最后的活动时间
    QDateTime getLastTime();

    //获取视频保存类
    FFmpegSave *getSaveFile();
    //获取视频流对象
    AVStream *getVideoStream();
    //获取音频流对象
    AVStream *getAudioStream();

    //获取和设置开启推流预览
    bool getPushPreview();
    void setPushPreview(bool pushPreview);

    //获取是否已经尝试打开过
    bool getTryOpen();
    //获取是否已经尝试读取过
    bool getTryRead();
    //获取是否尝试停止线程
    bool getTryStop();

    //清空缓存数据
    void clearBuffer(bool direct);
    //清空复位同步线程
    void clearAndReset();

    //获取和设置旋转角度
    int getRotate();
    void setRotate(int rotate);

    //获取文件时长
    qint64 getDuration();

    //获取和设置播放位置
    qint64 getPosition();
    void setPosition(qint64 position);
    void setPosition();

    //获取和设置播放速度
    double getSpeed();
    void setSpeed(double speed);

    //获取和设置音量大小
    int getVolume();
    void setVolume(int volume);

    //获取和设置静音状态
    bool getMuted();
    void setMuted(bool muted);

    //切换声卡
    void setAudioDevice(const QString &audioDevice);

    //设置裁剪状态
    void setCrop(bool isCrop);
    //打印信息
    void debug(int result, const QString &head, const QString &msg);

public slots:
    //暂停播放
    void pause();
    //继续播放
    void next();

    //按帧播放
    void step(bool backward = false);

    //开始录制
    void recordStart(const QString &fileName);
    //暂停录制
    void recordPause();
    //停止录制
    void recordStop();

    //设置标签信息集合
    void setOsdInfo(const QList<OsdInfo> &listOsd);
    //设置图形信息集合
    void setGraphInfo(const QList<GraphInfo> &listGraph);
};

#endif // FFMPEGTHREAD_H
