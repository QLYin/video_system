#ifndef FFMPEGSYNC_H
#define FFMPEGSYNC_H

#include <QThread>
#include <QMutex>
class FFmpegThread;
class AVPacket;

class FFmpegSync : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegSync(quint8 type, QObject *parent);
    ~FFmpegSync();

protected:
    void run();

private:
    //线程停止标志位
    volatile bool stopped;
    //数据锁
    QMutex mutex;

    //数据类型 0-音频 1-视频
    quint8 type;
    //解码主线程
    FFmpegThread *thread;
    //数据包队列
    QList<AVPacket *> packets;

    //当前帧显示时间
    double ptsTime;
    //已播放时间
    qint64 showTime;
    //缓冲时间
    qint64 bufferTime;
    //当前时间和开始时间的差值
    qint64 offsetTime;
    //解码开始时间
    qint64 startTime;

public:
    //停止线程
    void stop();
    //清除数据
    void clear();
    //复位时钟
    void reset();
    //添加数据包
    void append(AVPacket *packet);

    //获取队列中包的数量
    int getPacketCount();
    //计算pts时间
    bool checkPtsTime();
    //校验显示时间
    void checkShowTime();

signals:
    //文件播放进度
    void receivePosition(qint64 position);
};

#endif // FFMPEGSYNC_H
