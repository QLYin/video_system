#include "abstractsavethread.h"

bool AbstractSaveThread::debugInfo = true;
bool AbstractSaveThread::directSave = true;
AbstractSaveThread::AbstractSaveThread(QObject *parent) : QThread(parent)
{
    stopped = false;
    isOk = false;
    isPause = false;
    errorCount = 0;

    saveMode = SaveMode_File;
    saveVideoType = SaveVideoType_None;
    saveAudioType = SaveAudioType_None;

    videoWidth = 0;
    videoHeight = 0;
    frameRate = 25;
    rotate = 0;

    sampleRate = 0;
    channelCount = 1;
    profile = 1;
}

AbstractSaveThread::~AbstractSaveThread()
{
    this->stop();
}

void AbstractSaveThread::checkOpen()
{
    //特意每次做个小延时每次都去判断标志位等可以大大加快关闭速度
    int count = 0;
    int maxCount = 3000 / 100;
    while (!stopped) {
        msleep(100);
        count++;
        //测试下来正常情况下基本上等待一次后 isOk=true
        if (count == maxCount || isOk) {
            break;
        }
    }
}

void AbstractSaveThread::run()
{
    while (!stopped) {
        if (!isOk) {
            this->close();
            this->open(fileName);
            this->checkOpen();
            continue;
        }

        this->save();
        msleep(1);
    }

    stopped = false;
    isOk = false;
    isPause = false;
    errorCount = 0;
}

bool AbstractSaveThread::getIsOk()
{
    return this->isOk;
}

bool AbstractSaveThread::getIsPause()
{
    return this->isPause;
}

QString AbstractSaveThread::getFileName()
{
    return this->fileName;
}

void AbstractSaveThread::deleteFile(const QString &fileName)
{
    //如果文件大小太小则删除(mp4文件至少要求48字节)
    QFile file(fileName);
    if (file.exists() && file.size() <= 48) {
        file.remove();
        debug("失败删除", "");
    }
}

QString AbstractSaveThread::getFlag()
{
    return this->flag;
}

void AbstractSaveThread::setFlag(const QString &flag)
{
    this->flag = flag;
}

SaveMode AbstractSaveThread::getSaveMode()
{
    return this->saveMode;
}

void AbstractSaveThread::setSaveMode(SaveMode saveMode)
{
    this->saveMode = saveMode;
}

void AbstractSaveThread::debug(const QString &head, const QString &msg)
{
    if (!debugInfo) {
        return;
    }

    //如果设置了唯一标识则放在打印前面
    QString text = head;
    if (!flag.isEmpty()) {
        text = QString("标识[%1] %2").arg(flag).arg(text);
    }

    QString name = (saveMode == SaveMode_File ? "文件" : "地址");
    if (msg.isEmpty()) {
        qDebug() << TIMEMS << QString("%1 -> %2: %3").arg(text).arg(name).arg(fileName);
    } else {
        qDebug() << TIMEMS << QString("%1 -> %2 %3: %4").arg(text).arg(msg).arg(name).arg(fileName);
    }
}

bool AbstractSaveThread::init()
{
    return true;
}

void AbstractSaveThread::save()
{

}

void AbstractSaveThread::close()
{

}

void AbstractSaveThread::open(const QString &fileName)
{
    //已经打开不用继续防止重复打开
    if (isOk) {
        return;
    }

    this->fileName = fileName;
    if (fileName.startsWith("rtmp://")) {
        saveMode = SaveMode_Rtmp;
    } else if (fileName.startsWith("rtsp://")) {
        saveMode = SaveMode_Rtsp;
    }

    //调整视频文件拓展名(裸流情况还要要区分264/265)
    if (saveVideoType == SaveVideoType_Yuv) {
        this->fileName.replace("mp4", "yuv");
    } else if (saveVideoType == SaveVideoType_Stream) {
        QString videoCodecName = this->property("videoCodecName").toString();
        this->fileName.replace("mp4", videoCodecName == "hevc" ? "h265" : "h264");
    }

    //调整音频文件拓展名
    if (saveAudioType == SaveAudioType_Pcm || saveAudioType == SaveAudioType_Wav) {
        this->fileName.replace("mp4", "pcm");
    } else if (saveAudioType == SaveAudioType_Aac) {
        this->fileName.replace("mp4", "aac");
    }

    //特殊音频格式
    QString url = this->property("url").toString();
    if (url.endsWith(".mp3")) {
        this->fileName.replace("mp4", "mp3");
    }

    //先初始化
    if (!this->init()) {
        return;
    }

    //视频保存到MP4用的是内部接口打开文件
    file.setFileName(this->fileName);
    if (saveVideoType != SaveVideoType_Mp4) {
        isOk = file.open(QFile::WriteOnly);
    } else {
        isOk = true;
    }

    //启动线程
    this->start();
    emit receiveSaveStart();

    //打印对应的信息
    QString text = (saveMode == SaveMode_File ? "录制" : "推流");
    debug(QString("开始%1").arg(text), "");
    if (videoWidth > 0) {
        debug("视频编码", QString("宽度: %1 高度: %2 帧率: %3 角度: %4").arg(videoWidth).arg(videoHeight).arg(frameRate).arg(rotate));
    }
    if (sampleRate > 0) {
        debug("音频编码", QString("采样: %1 通道: %2 品质: %3").arg(sampleRate).arg(channelCount).arg(profile));
    }
}

void AbstractSaveThread::pause()
{
    if (!isOk) {
        return;
    }

    isPause = !isPause;
    QString text = (saveMode == SaveMode_File ? "录制" : "推流");
    debug(isPause ? QString("暂停%1").arg(text) : QString("继续%1").arg(text), "");
}

void AbstractSaveThread::stop()
{
    if (!isOk) {
        return;
    }

    //处于运行状态才可以停止
    if (this->isRunning()) {
        stopped = true;
        isPause = false;
        this->wait();
    }

    //关闭文件
    isOk = false;
    QString text = (saveMode == SaveMode_File ? "录制" : "推流");
    debug(QString("结束%1").arg(text), "");
    if (file.isOpen()) {
        file.close();
    }

    //关闭释放并清理文件
    this->close();
    this->deleteFile(fileName);
    emit receiveSaveFinsh();
}
