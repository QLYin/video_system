#ifndef VIDEOMANAGE_H
#define VIDEOMANAGE_H

#include "videowidgetx.h"

class VideoManage : public QThread
{
    Q_OBJECT
public:
    static VideoManage *Instance();
    explicit VideoManage(QObject *parent = 0);
    ~VideoManage();

protected:
    void run();

private:
    //单例对象
    static QScopedPointer<VideoManage> self;
    //数据锁
    QMutex mutex;
    //停止线程标志位
    volatile bool stopped;

    //当前打开索引
    int openIndex;
    //逐个打开间隔
    int openInterval;
    //不允许录像标志位
    bool saveVideo;

    //录像文件存储目录
    QString savePath;
    //最后开启录像的时间
    QDateTime lastRecordTime;

    //视频地址集合
    QStringList videoUrls;
    //视频控件集合
    QList<VideoWidget *> videoWidgets;

    //1. 每个通道每周可以对应不同的录像计划
    //2. 每天24小时拆分成48个小段/每30分钟可选录制与不录制
    //3. 用户在通道悬浮条一旦单击过录制按钮则该通道立即改成手动录制机制
    //4. 定时检测一旦到了30分钟附近则先停止录制然后判断将需要录制的开启录制
    QList<QStringList> recordTimes;

public:
    //获取所有视频控件
    QList<VideoWidget *> getVideoWidgets();
    //根据播放地址找到对应视频控件
    VideoWidget *getVideoWidget(const QString &rtspMain, const QString &rtspSub);

private slots:
    //逐个打开视频
    void openVideo();
    //设置默认目录
    void initPath();

    //获取对应星期的录像计划索引
    int getRecordWeek(const QDateTime &dateTime);
    //执行录像计划
    void doRecord(VideoWidget *videoWidget, const QDateTime &dateTime);
    //播放成功开启录像
    void receivePlayStart(int time);
    //检查录像计划
    void checkRecord();

public slots:
    //设置视频地址集合
    void setVideoUrls(const QStringList &videoUrls);
    //设置视频控件集合
    void setVideoWidgets(QList<VideoWidget *> videoWidgets);
    //设置录像计划
    void setRecordTimes(const QList<QStringList> &recordTimes);

    //设置打开间隔
    void setOpenInterval(int openInterval);
    //设置存储录像
    void setSaveVideo(bool saveVideo);
    //设置存储目录
    void setSavePath(const QString &savePath);
    //停止服务
    void stop();

signals:
    //全部打开完成
    void openFinsh();
};

#endif // VIDEOMANAGE_H
