#ifndef VIDEOTASK_H
#define VIDEOTASK_H

#include "videohead.h"

class VideoTask : public QThread
{
    Q_OBJECT
public:
    static VideoTask *Instance();
    explicit VideoTask(QObject *parent = 0);
    ~VideoTask();

protected:
    void run();

private:
    //单例对象
    static QScopedPointer<VideoTask> self;
    //数据锁
    QMutex mutex;
    //停止线程标志位
    volatile bool stopped;

    //任务时间/任务类型/任务参数
    QList<QDateTime> times;
    QList<QString> types;
    QList<QString> paras;

private slots:
    void resetCursor();

public slots:
    //插入任务
    void append(const QString &type, const QString &para);
    //停止线程
    void stop();

signals:
    //重命名完成
    void renameFinsh(const QString &fileName);
};

#endif // VIDEOTASK_H
