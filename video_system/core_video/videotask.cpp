#include "videotask.h"
#include "videohelper.h"

QScopedPointer<VideoTask> VideoTask::self;
VideoTask *VideoTask::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new VideoTask);
        }
    }

    return self.data();
}

VideoTask::VideoTask(QObject *parent) : QThread(parent)
{
    stopped = false;
}

VideoTask::~VideoTask()
{
    this->stop();
}

void VideoTask::run()
{
    while (!stopped) {
        if (times.count() > 0) {
            mutex.lock();
            QDateTime time = times.first();
            QString type = types.first();
            QString para = paras.first();
            mutex.unlock();

            QDateTime now = QDateTime::currentDateTime();
            int offset = time.msecsTo(now);
            if (offset >= 500) {
                //qDebug() << TIMEMS << offset << type << para;
                if (type == "renameFile") {
                    VideoHelper::renameFile(para);
                } else if (type == "resetCursor") {
                    //必须在主线程执行
                    QMetaObject::invokeMethod(this, "resetCursor");
                }

                //执行完成任务后从队列中移除
                mutex.lock();
                times.removeFirst();
                types.removeFirst();
                paras.removeFirst();
                mutex.unlock();
            }
        }

        msleep(100);
    }

    stopped = false;
}

void VideoTask::resetCursor()
{    
    VideoHelper::resetCursor();
}

void VideoTask::append(const QString &type, const QString &para)
{
    //复位光标只需要一次
    if (type == "resetCursor" && types.contains("resetCursor")) {
        return;
    }

    mutex.lock();
    times << QDateTime::currentDateTime();
    types << type;
    paras << para;
    mutex.unlock();
}

void VideoTask::stop()
{
    //处于运行状态才可以停止
    if (this->isRunning()) {
        stopped = true;
        this->wait();
    }
}
