#include "abstractvideothread.h"
#include "widgethelper.h"
#include "urlhelper.h"
#ifdef videosave
#include "savevideo.h"
#include "saveaudio.h"
#endif
#ifdef ffmpeg
#include "ffmpegsave.h"
#endif

int AbstractVideoThread::debugInfo = 2;

void AbstractVideoThread::checkDeviceUrl(const QString &url, QString &deviceName, QString &resolution, int &frameRate)
{
    int offsetX, offsetY;
    QString encodeScale = "camera";
    AbstractVideoThread::checkDeviceUrl(url, deviceName, resolution, frameRate, offsetX, offsetY, encodeScale);
}

void AbstractVideoThread::checkDeviceUrl(const QString &url, QString &deviceName, QString &resolution, int &frameRate, int &offsetX, int &offsetY, QString &encodeScale)
{
    //无论是否带分隔符第一个约定是设备名称
    QStringList list = url.split("|");
    int count = list.count();
    deviceName = list.at(0);

    //默认不指定屏幕索引
    int screenIndex = -1;
    //用一个无用的参数作为是否是本地摄像头的标志位
    bool isCamera = (encodeScale == "camera");

    //带分隔符说明还指定了分辨率或帧率
    if (count > 1) {
        QStringList sizes = WidgetHelper::getSizes(list.at(1));
        if (sizes.count() == 2) {
            int width = sizes.at(0).toInt();
            int height = sizes.at(1).toInt();
            resolution = QString("%1x%2").arg(width).arg(height);
        } else {
            resolution = "0x0";
        }

        //第三个参数是帧率
        if (count >= 3) {
            frameRate = list.at(2).toInt();
        }

        //桌面采集还需要取出其他几个参数
        if (!isCamera) {
            //XY坐标偏移值
            if (count >= 5) {
                offsetX = list.at(3).toInt();
                offsetY = list.at(4).toInt();
            }

            //屏幕索引
            if (count >= 6) {
                screenIndex = list.at(5).toInt();
            }

            //视频缩放
            if (count >= 7) {
                encodeScale = list.at(6);
            }

            //约定屏幕索引255则取用户自定义的区域
            if (screenIndex != 255) {
                WidgetHelper::checkRect(screenIndex, resolution, offsetX, offsetY);
            }
        }
    }

    //没有设置分辨率则重新处理
    if (resolution == "0x0") {
        if (isCamera) {
            resolution = "640x480";
        } else {
            WidgetHelper::checkRect(screenIndex, resolution, offsetX, offsetY);
        }
    }
}

AbstractVideoThread::AbstractVideoThread(QObject *parent) : QThread(parent)
{
    //注册数据类型
    qRegisterMetaType<RecorderState>("RecorderState");
    qRegisterMetaType<QList<int> >("QList<int>");

    stopped = false;
    isOk = false;
    isPause = false;
    isSnap = false;
    isRecord = false;

    errorCount = 0;
    lastTime = QDateTime::currentDateTime();
    hwndWidget = (QWidget *)parent;

    videoMode = VideoMode_Hwnd;
    videoSize = "0x0";
    videoWidth = 0;
    videoHeight = 0;
    rotate = -1;

    sampleRate = 8000;
    channelCount = 1;
    profile = 1;
    onlyAudio = false;

    brightness = 0;
    contrast = 0;
    hue = 0;
    saturation = 0;

    snapName = "";
    fileName = "";

    findFaceRect = false;
    findFaceOne = false;

    saveVideoType = SaveVideoType_None;
    saveAudioType = SaveAudioType_None;

#ifdef videosave
    saveVideo = new SaveVideo(this);
    saveAudio = new SaveAudio(this);
#endif
#ifdef ffmpeg
    saveFile = new FFmpegSave(this);
#endif
}

AbstractVideoThread::~AbstractVideoThread()
{
    this->stop();
}

void AbstractVideoThread::setGeometry()
{

}

void AbstractVideoThread::initFilter()
{

}

bool AbstractVideoThread::getIsOk() const
{
    return this->isOk;
}

bool AbstractVideoThread::getIsPause() const
{
    return this->isPause;
}

bool AbstractVideoThread::getIsSnap() const
{
    return this->isSnap;
}

bool AbstractVideoThread::getIsRecord() const
{
    return this->isRecord;
}

void AbstractVideoThread::updateTime()
{
    lastTime = QDateTime::currentDateTime();
}

QElapsedTimer *AbstractVideoThread::getTimer()
{
    return &timer;
}

VideoMode AbstractVideoThread::getVideoMode() const
{
    return this->videoMode;
}

void AbstractVideoThread::setVideoMode(const VideoMode &videoMode)
{
    this->videoMode = videoMode;
}

int AbstractVideoThread::getVideoWidth() const
{
    return this->videoWidth;
}

int AbstractVideoThread::getVideoHeight() const
{
    return this->videoHeight;
}

void AbstractVideoThread::setVideoSize(const QString &videoSize)
{
    this->videoSize = videoSize;
    QStringList sizes = WidgetHelper::getSizes(videoSize);
    if (sizes.count() == 2) {
        videoWidth = sizes.at(0).toInt();
        videoHeight = sizes.at(1).toInt();
    }
}

void AbstractVideoThread::checkVideoSize(int width, int height)
{
    if (width > 0 && height > 0 && videoWidth > 0 && videoHeight > 0) {
        if (videoWidth != width || videoHeight != height) {
            videoWidth = width;
            videoHeight = height;
            QMetaObject::invokeMethod(this, "receiveSizeChanged");
        }
    }
}

void AbstractVideoThread::addWidget(QWidget *widget)
{
    //有时候可能只是线程在用/没有设置窗体展示/比如只需要后台解码的情况
    if (hwndWidget && hwndWidget->layout()) {
        hwndWidget->layout()->addWidget(widget);
    }

    QMetaObject::invokeMethod(widget, "show");
}

qreal AbstractVideoThread::getFrameRate() const
{
    return this->frameRate;
}

void AbstractVideoThread::setFrameRate(qreal frameRate)
{
    this->frameRate = frameRate;
}

int AbstractVideoThread::getRotate() const
{
    return this->rotate;
}

void AbstractVideoThread::setRotate(int rotate)
{
    this->rotate = rotate;
}

void AbstractVideoThread::checkPath(const QString &fileName)
{
    //补全完整路径
    QString path = QFileInfo(fileName).path();
    if (path.startsWith("./")) {
        path.replace(".", "");
        path = qApp->applicationDirPath() + path;
    }

    //目录不存在则新建
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }
}

QString AbstractVideoThread::getSnapName() const
{
    return this->snapName;
}

void AbstractVideoThread::setSnapName(const QString &snapName)
{
    this->snapName = snapName;
    this->checkPath(snapName);
}

QString AbstractVideoThread::getFileName() const
{
    return this->fileName;
}

void AbstractVideoThread::setFileName(const QString &fileName)
{
    this->fileName = fileName;
    this->checkPath(fileName);
}

void AbstractVideoThread::setFindFaceRect(bool findFaceRect)
{
    this->findFaceRect = findFaceRect;
}

void AbstractVideoThread::setFindFaceOne(bool findFaceOne)
{
    this->findFaceOne = findFaceOne;
}

SaveVideoType AbstractVideoThread::getSaveVideoType() const
{
    return this->saveVideoType;
}

void AbstractVideoThread::setSaveVideoType(const SaveVideoType &saveVideoType)
{
    this->saveVideoType = saveVideoType;
}

SaveAudioType AbstractVideoThread::getSaveAudioType() const
{
    return this->saveAudioType;
}

void AbstractVideoThread::setSaveAudioType(const SaveAudioType &saveAudioType)
{
    this->saveAudioType = saveAudioType;
}

void AbstractVideoThread::setFlag(const QString &flag)
{
    this->flag = flag;
#ifdef videosave
    saveVideo->setFlag(flag);
    saveAudio->setFlag(flag);
#endif
#ifdef ffmpeg
    saveFile->setFlag(flag);
#endif
}

void AbstractVideoThread::debug(const QString &head, const QString &msg, const QString &url)
{
    if (debugInfo == 0) {
        return;
    }

    //如果设置了唯一标识则放在打印前面
    QString text = head;
    if (!flag.isEmpty()) {
        text = QString("标识[%1] %2").arg(flag).arg(text);
    }

    QString addr = url;
    if (debugInfo == 2) {
        //为空的时候获取一次即可
        if (ip.isEmpty()) {
            //本地文件则取末尾
            if (QFile(url).exists() && !url.contains("/dev/")) {
                ip = url.split("/").last();
            } else {
                ip = UrlHelper::getUrlIP(url);
            }
        }
        addr = ip;
    }

    if (msg.isEmpty()) {
        qDebug() << TIMEMS << QString("%1 -> 地址: %2").arg(text).arg(addr);
    } else {
        qDebug() << TIMEMS << QString("%1 -> %2 地址: %3").arg(text).arg(msg).arg(addr);
    }
}

void AbstractVideoThread::setImage(const QImage &image)
{
    this->updateTime();
    emit receiveImage(image, 0);
}

void AbstractVideoThread::setRgb(int width, int height, quint8 *dataRGB, int type)
{
    this->updateTime();
    emit receiveFrame(width, height, dataRGB, type);
}

void AbstractVideoThread::setYuv(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV)
{
    this->updateTime();
    emit receiveFrame(width, height, dataY, dataU, dataV, width, width / 2, width / 2);
}

void AbstractVideoThread::play()
{
    //没有运行才需要启动
    if (!this->isRunning()) {
        stopped = false;
        isPause = false;
        isSnap = false;
        this->start();
    }
}

void AbstractVideoThread::stop()
{
    //处于运行状态才可以停止
    if (this->isRunning()) {
        stopped = true;
        isPause = false;
        isSnap = false;
        this->wait();
    }
}

void AbstractVideoThread::pause()
{
    if (this->isRunning()) {
        isPause = true;
    }
}

void AbstractVideoThread::next()
{
    if (this->isRunning()) {
        isPause = false;
    }
}

void AbstractVideoThread::snap(const QString &snapName)
{
    if (this->isRunning()) {
        isSnap = true;
        this->setSnapName(snapName);
    }
}

void AbstractVideoThread::snapFinsh(const QImage &image)
{
    //如果填了截图文件名称则先保存图片
    if (!snapName.isEmpty() && !image.isNull()) {
        //取出拓展名根据拓展名保存格式
        QString suffix = snapName.split(".").last();
        image.save(snapName, suffix.toLatin1().constData());
    }

    //发送截图完成信号
    emit snapImage(image, snapName);
}

void AbstractVideoThread::recordStart(const QString &fileName)
{
#ifdef videosave
    this->setFileName(fileName);
    if (saveAudioType > 0) {
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveAudio->getIsPause()) {
            saveAudio->pause();
            emit recorderStateChanged(RecorderState_Recording, saveAudio->getFileName());
        } else {
            saveAudio->setPara(saveAudioType, sampleRate, channelCount, profile);
            saveAudio->open(fileName);
            if (saveAudio->getIsOk()) {
                emit recorderStateChanged(RecorderState_Recording, saveAudio->getFileName());
            }
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveVideo->getIsPause()) {
            isRecord = true;
            saveVideo->pause();
            emit recorderStateChanged(RecorderState_Recording, saveVideo->getFileName());
        } else {
            saveVideo->setPara(saveVideoType, videoWidth, videoHeight, frameRate);
            saveVideo->open(fileName);
            if (saveVideo->getIsOk()) {
                isRecord = true;
                emit recorderStateChanged(RecorderState_Recording, saveVideo->getFileName());
            }
        }
    }
#endif
}

void AbstractVideoThread::recordPause()
{
#ifdef videosave
    if (saveAudioType > 0) {
        if (saveAudio->getIsOk()) {
            saveAudio->pause();
            emit recorderStateChanged(RecorderState_Paused, saveAudio->getFileName());
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        if (saveVideo->getIsOk()) {
            isRecord = false;
            saveVideo->pause();
            emit recorderStateChanged(RecorderState_Paused, saveVideo->getFileName());
        }
    }
#endif
}

void AbstractVideoThread::recordStop()
{
#ifdef videosave
    if (saveAudioType > 0) {
        if (saveAudio->getIsOk()) {
            saveAudio->stop();
            emit recorderStateChanged(RecorderState_Stopped, saveAudio->getFileName());
        }
    }

    if (saveVideoType == SaveVideoType_Yuv && !onlyAudio) {
        if (saveVideo->getIsOk()) {
            isRecord = false;
            saveVideo->stop();
            emit recorderStateChanged(RecorderState_Stopped, saveVideo->getFileName());
        }
    }
#endif
}

void AbstractVideoThread::writeVideoData(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV)
{
#ifdef videosave
    if (saveVideoType == SaveVideoType_Yuv && isRecord) {
        saveVideo->setPara(SaveVideoType_Yuv, width, height, frameRate);
        saveVideo->write(dataY, dataU, dataV);
    }
#endif
}

void AbstractVideoThread::writeAudioData(const char *data, qint64 len)
{
#ifdef videosave
    if (saveAudioType > 0 && saveAudio->getIsOk()) {
        saveAudio->write(data, len);
    }
#endif
}

void AbstractVideoThread::writeAudioData(const QByteArray &data)
{
#ifdef videosave
    if (saveAudioType > 0 && saveAudio->getIsOk()) {
        this->writeAudioData(data.constData(), data.length());
    }
#endif
}

void AbstractVideoThread::setOsdInfo(const QList<OsdInfo> &listOsd)
{
    this->listOsd = listOsd;
    this->initFilter();
}

void AbstractVideoThread::setGraphInfo(const QList<GraphInfo> &listGraph)
{
    this->listGraph = listGraph;
    this->initFilter();
}
