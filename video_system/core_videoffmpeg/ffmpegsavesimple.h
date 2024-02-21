#ifndef FFMPEGSAVESIMPLE_H
#define FFMPEGSAVESIMPLE_H

#include "qthread.h"
#include "ffmpeghelper.h"

class FFmpegSaveSimple : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegSaveSimple(QObject *parent = 0);
    ~FFmpegSaveSimple();

protected:
    void run();

private:
    //停止线程标志位
    volatile bool stopped;

    //输入地址
    QString urlIn;
    //输出地址(可以是保存到本地或者推流)
    QString urlOut;

    //音视频索引
    int audioIndex;
    int videoIndex;

    //输入格式上下文
    AVFormatContext *formatCtxIn;
    //输出格式上下文
    AVFormatContext *formatCtxOut;

private slots:
    //打开
    bool open();
    //关闭
    void close();

public slots:
    //打印信息
    void debug(int ret, const QString &msg);
    //设置地址
    void setUrl(const QString &urlIn, const QString &urlOut);
    //停止线程
    void stop();
};

#endif // FFMPEGSAVESIMPLE_H
