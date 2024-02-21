#include "videomanage.h"
#include "abstractsavethread.h"
#include "abstractvideothread.h"

QScopedPointer<VideoManage> VideoManage::self;
VideoManage *VideoManage::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new VideoManage);
        }
    }

    return self.data();
}

VideoManage::VideoManage(QObject *parent) : QThread(parent)
{
    stopped = false;
    openIndex = -1;
    openInterval = 100;
    saveVideo = true;
    savePath = "./video_normal";
    this->initPath();

    //由于解码线程中的打印信息比较多(可以临时关闭打印信息方便查看其他打印信息)
    //AbstractSaveThread::debugInfo = false;
    //AbstractVideoThread::debugInfo = false;
}

VideoManage::~VideoManage()
{
    this->stop();
}

void VideoManage::run()
{
    //校验下数量是否一致
    int count1 = videoUrls.count();
    int count2 = videoWidgets.count();
    int count3 = recordTimes.count();
    if (count1 == 0 || count2 == 0 || count3 == 0 || count1 != count2 || count2 != count3) {
        stopped = false;
        return;
    }

    openIndex = -1;
    //默认时间搞个早期时间保证每次都是正确计算
    lastRecordTime = QDateTime::fromString("1970-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss");
    while (!stopped) {
        this->openVideo();
        this->checkRecord();
        msleep(1);
    }

    stopped = false;
}

QList<VideoWidget *> VideoManage::getVideoWidgets()
{
    return this->videoWidgets;
}

VideoWidget *VideoManage::getVideoWidget(const QString &rtspMain, const QString &rtspSub)
{
    foreach (VideoWidget *videoWidget, videoWidgets) {
        QString url = videoWidget->getVideoPara().videoUrl;
        if (url == rtspMain || url == rtspSub) {
            return videoWidget;
        }
    }
    return 0;
}

void VideoManage::openVideo()
{
    openIndex++;
    if (openIndex < videoWidgets.count()) {
        //判断是否到末尾发送打开完成信号
        if (openIndex == videoWidgets.count() - 1) {
            emit openFinsh();
            //qDebug() << TIMEMS << "全部打开完成";
        }

        //立即跳过地址为空的
        QString url = videoUrls.at(openIndex);
        if (url.isEmpty()) {
            return;
        }

        //异步执行打开
        VideoWidget *videoWidget = videoWidgets.at(openIndex);
        QMetaObject::invokeMethod(videoWidget, "open", Q_ARG(QString, url));
        msleep(openInterval);
    }
}

void VideoManage::initPath()
{
    //默认录像文件和截图文件存放路径
    QString path = savePath + "/" + QDATE;
    VideoWidget::recordPath = path;
    VideoWidget::snapPath = path;
    VideoWidget::snapPath.replace("video_normal", "image_normal");
}

int VideoManage::getRecordWeek(const QDateTime &dateTime)
{
    //取出星期几
    int recordWeek = 0;
    QString week = dateTime.toString("dddd");
    if (week == "星期一") {
        recordWeek = 0;
    } else if (week == "星期二") {
        recordWeek = 1;
    } else if (week == "星期三") {
        recordWeek = 2;
    } else if (week == "星期四") {
        recordWeek = 3;
    } else if (week == "星期五") {
        recordWeek = 4;
    } else if (week == "星期六") {
        recordWeek = 5;
    } else {
        recordWeek = 6;
    }

    return recordWeek;
}

void VideoManage::doRecord(VideoWidget *videoWidget, const QDateTime &dateTime)
{
    //运行阶段才能开启录像
    if (!videoWidget->getIsRunning() || !saveVideo) {
        return;
    }

    //取出星期几
    int recordWeek = getRecordWeek(dateTime);
    //取出当前时间对应录像计划一天中所在的格子
    int hour = dateTime.time().hour();
    int min = dateTime.time().minute();
    int recordIndex = (hour * 2) + (min < 30 ? 0 : 1);
    //qDebug() << TIMEMS << "录像计划" << videoWidget->getWidgetPara().videoFlag << recordWeek << recordIndex;

    //设置录像文件格式
    videoWidget->getVideoThread()->setSaveVideoType(SaveVideoType_Mp4);
    videoWidget->getVideoThread()->setSaveAudioType(SaveAudioType_None);

    //从录像计划表中找出对应的计划
    int index = videoWidgets.indexOf(videoWidget);
    QStringList recordTime = recordTimes.at(index);
    QString time = recordTime.at(recordWeek);
    QStringList list = time.split(",");
    bool needRecord = (list.at(recordIndex) == "1");
    if (needRecord) {
        QString path = savePath + "/" + QDATE;
        QString fileName = QString("%1/ch%2_%3.mp4").arg(path).arg(index + 1, 2, 10, QChar('0')).arg(STRDATETIMEMS);
        videoWidget->recordStart(fileName);
    }
}

void VideoManage::receivePlayStart(int time)
{
    lastRecordTime = QDateTime::currentDateTime();
    VideoWidget *videoWidget = (VideoWidget *)sender();
    this->doRecord(videoWidget, lastRecordTime);
}

void VideoManage::checkRecord()
{
    //如果还在打开阶段则不用继续
    if (openIndex < videoWidgets.count()) {
        return;
    }

    //计算离最后一次开启录像的时间差值(防止频繁执行)
    QDateTime now = QDateTime::currentDateTime();
    int offset = lastRecordTime.secsTo(now);
    if (offset > 60) {
        //无论是否需要录像到整半点就处理一下
        int min = now.time().minute();
        int sec = now.time().second();
        //这里设定一个目标秒数可以用于校准时间精确到秒(偏差几秒就填几/默认无偏差就是0)
        int target = 0;
        if ((min == 0 || min == 30) && sec >= target) {
            lastRecordTime = now;
            //先对所有的通道停止录像再去判断是否需要录像再开启录像
            foreach (VideoWidget *videoWidget, videoWidgets) {
                videoWidget->recordStop();
                this->doRecord(videoWidget, now);
            }
        }

        this->initPath();
    }

    msleep(1000);
}

void VideoManage::setVideoUrls(const QStringList &videoUrls)
{
    this->videoUrls = videoUrls;
}

void VideoManage::setVideoWidgets(QList<VideoWidget *> videoWidgets)
{
    this->videoWidgets = videoWidgets;
    //关联打开成功信号开启录像(末尾参数用来过滤只关联一次)
    foreach (VideoWidget *videoWidget, videoWidgets) {
        connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)), Qt::UniqueConnection);
    }
}

void VideoManage::setRecordTimes(const QList<QStringList> &recordTimes)
{
    this->recordTimes = recordTimes;
}

void VideoManage::setOpenInterval(int openInterval)
{
    if (openInterval >= 0 && openInterval < 3000) {
        this->openInterval = openInterval;
    }
}

void VideoManage::setSaveVideo(bool saveVideo)
{
    this->saveVideo = saveVideo;
}

void VideoManage::setSavePath(const QString &savePath)
{
    this->savePath = savePath;
}

void VideoManage::stop()
{
    //关闭所有视频
    foreach (VideoWidget *videoWidget, videoWidgets) {
        videoWidget->stop();
    }

    //处于运行状态才可以停止
    if (this->isRunning()) {
        stopped = true;
        this->wait();
    }
}
