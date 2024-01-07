#ifndef ABSTRACTVIDEOTHREAD_H
#define ABSTRACTVIDEOTHREAD_H

#include "widgethead.h"
class SaveVideo;
class SaveAudio;
class FFmpegSave;

class AbstractVideoThread : public QThread
{
    Q_OBJECT
public:
    //打印线程消息类别(0-不打印 1-完整地址 2-IP地址)
    static int debugInfo;
    //通用函数取出本地设备带分辨率和帧率等信息    
    static void checkDeviceUrl(const QString &url, QString &deviceName, QString &resolution, int &frameRate);
    static void checkDeviceUrl(const QString &url, QString &deviceName, QString &resolution, int &frameRate, int &offsetX, int &offsetY, QString &encodeScale);

    explicit AbstractVideoThread(QObject *parent = 0);
    ~AbstractVideoThread();

protected:
    //设置视频控件尺寸
    virtual void setGeometry();
    //初始化滤镜
    virtual void initFilter();    

protected:
    //数据锁
    QMutex mutex;
    //停止线程标志位
    volatile bool stopped;
    //打开是否成功
    volatile bool isOk;
    //暂停采集标志位
    volatile bool isPause;
    //开始截图标志位
    volatile bool isSnap;
    //正在录制标志位
    volatile bool isRecord;

    //设备地址
    QString ip;
    //唯一标识
    QString flag;

    //错误计数
    int errorCount;
    //最后的播放时间
    QDateTime lastTime;
    //计时器用来统计用时
    QElapsedTimer timer;
    //句柄窗体
    QWidget *hwndWidget;

    //视频显示模式
    VideoMode videoMode;
    //视频分辨率字符串
    QString videoSize;
    //视频画面宽度
    int videoWidth;
    //视频画面高度
    int videoHeight;
    //视频画面帧率
    qreal frameRate;
    //视频旋转角度
    int rotate;

    //音频采样率
    int sampleRate;
    //音频通道数
    int channelCount;
    //音频品质
    int profile;
    //只有音频
    bool onlyAudio;

    //明亮度
    int brightness;
    //对比度
    int contrast;
    //色彩度
    int hue;
    //饱和度
    int saturation;

    //截图文件名称
    QString snapName;
    //录制文件名称
    QString fileName;

    //查找人脸区域
    bool findFaceRect;
    //查找相似度最高人脸
    bool findFaceOne;

    //保存视频文件类型
    SaveVideoType saveVideoType;
    //保存音频文件类型
    SaveAudioType saveAudioType;

    //保存视频到文件
    SaveVideo *saveVideo;
    //保存音频到文件
    SaveAudio *saveAudio;
    //保存音视频到文件
    FFmpegSave *saveFile;

    //标签信息集合
    QList<OsdInfo> listOsd;
    //图形信息集合
    QList<GraphInfo> listGraph;

public:
    bool getIsOk() const;
    bool getIsPause() const;
    bool getIsSnap() const;
    bool getIsRecord() const;

public:
    //主动更新时间(一般事件回调中设置)
    void updateTime();
    //获取计时器
    QElapsedTimer *getTimer();

    //获取和设置视频显示模式
    VideoMode getVideoMode() const;
    void setVideoMode(const VideoMode &videoMode);

    //获取和设置画面尺寸 支持自动转换 640*480 640x480
    int getVideoWidth() const;
    int getVideoHeight() const;
    void setVideoSize(const QString &videoSize);

    //校验尺寸是否发生变化
    void checkVideoSize(int width, int height);
    //将子窗体插入到句柄控件布局
    void addWidget(QWidget *widget);

    //获取和设置采集帧率
    qreal getFrameRate() const;
    void setFrameRate(qreal frameRate);

    //获取和设置旋转角度
    int getRotate() const;
    void setRotate(int rotate);

    //目录不存在则新建
    void checkPath(const QString &fileName);

    //获取和设置截图文件名称
    QString getSnapName() const;
    void setSnapName(const QString &snapName);

    //获取和设置保存文件名称
    QString getFileName() const;
    void setFileName(const QString &fileName);

    //设置查找人脸区域
    void setFindFaceRect(bool findFaceRect);
    //设置查找相似度最高人脸
    void setFindFaceOne(bool findFaceOne);

    //获取和设置保存视频文件类型
    SaveVideoType getSaveVideoType() const;
    void setSaveVideoType(const SaveVideoType &saveVideoType);

    //获取和设置保存音频文件类型
    SaveAudioType getSaveAudioType() const;
    void setSaveAudioType(const SaveAudioType &saveAudioType);

public slots:
    //设置唯一标识(用来打印输出方便区分)
    void setFlag(const QString &flag);
    //统一格式打印信息
    void debug(const QString &head, const QString &msg, const QString &url);

    //主动设置图片(一般事件回调中设置)
    void setImage(const QImage &image);
    //主动设置rgb图片数据(一般事件回调中设置)
    void setRgb(int width, int height, quint8 *dataRGB, int type);
    //主动设置yuv图片数据(一般事件回调中设置)
    void setYuv(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV);

public slots:
    //开始播放
    virtual void play();
    //停止播放
    virtual void stop();

    //暂停播放
    virtual void pause();
    //继续播放
    virtual void next();

    //抓拍截图
    virtual void snap(const QString &snapName = QString());
    //截图完成
    virtual void snapFinsh(const QImage &image);

    //开始录制
    virtual void recordStart(const QString &fileName);
    //暂停录制
    virtual void recordPause();
    //停止录制
    virtual void recordStop();

    //写入视频数据到文件
    void writeVideoData(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV);
    //写入音频数据到文件
    void writeAudioData(const char *data, qint64 len);
    void writeAudioData(const QByteArray &data);

    //设置标签信息集合
    virtual void setOsdInfo(const QList<OsdInfo> &listOsd);
    //设置图形信息集合
    virtual void setGraphInfo(const QList<GraphInfo> &listGraph);

signals:
    //播放成功
    void receivePlayStart(int time);
    //播放结束
    void receivePlayFinsh();
    //播放失败
    void receivePlayError(int error);

    //收到一张图片
    void receiveImage(const QImage &image, int time);
    //抓拍一张图片
    void snapImage(const QImage &image, const QString &snapName);

    //视频尺寸变化
    void receiveSizeChanged();
    //录制状态变化
    void recorderStateChanged(const RecorderState &state, const QString &file);

    //收到一帧rgb视频数据
    void receiveFrame(int width, int height, quint8 *dataRGB, int type);
    //收到一帧yuv视频数据
    void receiveFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);
    //收到一帧nv12视频数据
    void receiveFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV);

    //音量大小
    void receiveVolume(int volume);
    //静音状态
    void receiveMuted(bool muted);
    //音频数据振幅
    void receiveLevel(qreal leftLevel, qreal rightLevel);

    //视频实时码率
    void receiveKbps(qreal kbps, int frameRate);
};

#endif // ABSTRACTVIDEOTHREAD_H
