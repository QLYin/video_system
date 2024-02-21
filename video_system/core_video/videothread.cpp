#include "videothread.h"
#include "videohelper.h"
#include "videotask.h"

QList<VideoThread *> VideoThread::videoThreads;
VideoThread *VideoThread::getVideoThread(const WidgetPara &widgetPara, const VideoPara &videoPara)
{
    //句柄模式不能共用
    if (!widgetPara.sharedData || widgetPara.videoMode == VideoMode_Hwnd) {
        return NULL;
    }

    VideoThread *thread = NULL;
    foreach (VideoThread *videoThread, videoThreads) {
        //桌面采集永远不共享
        if (VideoHelper::getVideoType(videoPara.videoUrl) == VideoType_Desktop) {
            break;
        }

        //解析内核和视频地址一致才能唯一决定一个采集线程
        if (videoThread->getVideoCore() == videoPara.videoCore && videoThread->getVideoUrl() == videoPara.videoUrl) {
            //限定纯音频不用共享
            if (!videoThread->getOnlyAudio()) {
                videoThread->refCount++;
                thread = videoThread;
            }
            break;
        }
    }

    return thread;
}

VideoThread::VideoThread(QObject *parent) : AbstractVideoThread(parent)
{
    refCount = 0;
    audioTrack = -1;
    videoTrack = -1;
    duration = 0;
    position = 0;

    firstDuration = -1;
    playCount = 0;
    rotate = -1;

    videoCore = VideoCore_None;
    videoUrl = "";
    videoType = VideoType_Other;

    videoWidth = 0;
    videoHeight = 0;
    bufferSize = "0x0";

    decodeType = DecodeType_Fast;
    hardware = "none";
    transport = "auto";
    caching = 0;

    audioLevel = false;
    decodeAudio = true;
    playAudio = true;
    playRepeat = false;
    playStep = false;
    countKbps = false;
    secretKey = "";
    audioDevice = "";

    encodeType = EncodeType_Auto;
    videoFormat = VideoFormat_Auto;
    encodeVideoFps = 0;
    encodeVideoRatio = 1;
    encodeVideoScale = "1";

    openSleepTime = 3000;
    readTimeout = 0;
    connectTimeout = 500;

    //这里要过滤下只使用了解码线程而没有对应视频控件的时候
    videoWidget = (QWidget *)parent;
    if (videoWidget) {
        while (!videoWidget->inherits("VideoWidget")) {
            videoWidget = (QWidget *)videoWidget->parent();
            if (!videoWidget) {
                break;
            }
        }
    }

    //也有可能有父控件但是不是视频控件
    if (videoWidget && !videoWidget->inherits("VideoWidget")) {
        videoWidget = 0;
    }

    //启动任务处理线程做一些额外的处理
    VideoTask::Instance()->start();
}

void VideoThread::checkOpen()
{
    //特意每次做个小延时每次都去判断标志位等可以大大加快关闭速度
    int count = 0;
    int maxCount = openSleepTime / 100;
    while (!stopped) {
        msleep(100);
        count++;
        //测试下来正常情况下基本上等待一次后 isOk=true
        if (count == maxCount || isOk) {
            break;
        }
    }
}

void VideoThread::run()
{
    while (!stopped) {
        if (!isOk) {
            this->closeVideo();
            if (videoMode == VideoMode_Hwnd) {
                QMetaObject::invokeMethod(this, "openVideo");
            } else {
                this->openVideo();
            }

            this->checkOpen();
            continue;
        }

        if (videoCore == VideoCore_Vlc) {
            if (videoWidth <= 0 && !getOnlyAudio()) {
                //视频文件需要尝试读取媒体信息多次保证能够读取到(一般视频流需要多次才能读取到)
                this->readMediaInfo();
            }
        } else if (videoCore == VideoCore_Mdk) {
            //实时读取播放进度
            this->getPosition();
        } else if (videoCore == VideoCore_HaiKang) {
            if (videoType == VideoType_FileLocal) {
                //本地文件需要这里实时读取播放进度
                this->getPosition();
            } else if (isOk && videoMode == VideoMode_Hwnd) {
                //句柄模式下视频流如果打开正常了则sdk内部处理重连
                this->updateTime();
            }
        } else if (videoCore == VideoCore_EasyPlayer) {
            if (videoWidth <= 0) {
                this->readMediaInfo();
            }

            this->getDuration();
            this->getPosition();
        }

        //启用了自动重连则通过判断最后的消息时间(超时则重新打开)
        if (readTimeout > 0) {
            qint64 offset = lastTime.msecsTo(QDateTime::currentDateTime());
            if (offset >= readTimeout) {
                isOk = false;
                debug("超时重连", "");
                continue;
            }
        }

        msleep(100);
    }

    //关闭视频
    this->closeVideo();

    //文件名为空才说明真正处理完可以彻底结束线程(否则一直等因为有可能文件还没保存完成)
    while (!fileName.isEmpty()) {
        debug("等待完成", "");
        msleep(5);
    }

    debug("线程结束", "");
}

bool VideoThread::eventFilter(QObject *watched, QEvent *event)
{
    //对应内核中会有 installEventFilter
    //qDebug() << TIMEMS << watched << event;
    if (videoCore == VideoCore_QMedia) {
        //视频源尺寸发生变化需要重新读取信息
        if (event->type() == QEvent::LayoutRequest) {
            this->setGeometry();
        }
    } else if (videoCore == VideoCore_EasyPlayer) {
        //父窗体尺寸变了需要重新设置显示视频控件的尺寸
        if (event->type() == QEvent::Resize) {
            this->setGeometry();
        }
    }

    return QThread::eventFilter(watched, event);
}

bool VideoThread::openVideo()
{
    return false;
}

void VideoThread::closeVideo()
{
    duration = 0;
    position = 0;

    firstDuration = -1;
    playCount = 0;

    videoWidth = 0;
    videoHeight = 0;

    errorCount = 0;
    snapName = "";
    fileName = "";

    stopped = false;
    isOk = false;
    isPause = false;
    isSnap = false;
    isRecord = false;
    debug("关闭线程", "");
    emit receivePlayFinsh();
}

void VideoThread::replay()
{
    //如果设置了重复循环播放则快到了文件末尾重新设置位置即可
    if (this->getPlayRepeat() && (this->getDuration() - position) < 500) {
        QMetaObject::invokeMethod(this, "setPosition", Q_ARG(qint64, 0));
    }
}

void VideoThread::debug(const QString &head, const QString &msg)
{
    AbstractVideoThread::debug(head, msg, videoUrl);
}

void VideoThread::setDuration2(qint64 duration)
{
    this->duration = duration;
    //在mpv内核中如果连续多次时长不一致则判定为视频流而不是视频文件
    if (playCount < 2) {
        this->firstDuration = duration;
    }
}

void VideoThread::setPosition2(qint64 position)
{
    this->position = position;
    this->playCount++;
}

VideoCore VideoThread::getVideoCore() const
{
    return this->videoCore;
}

void VideoThread::setVideoCore(const VideoCore &videoCore)
{
    this->videoCore = videoCore;
}

QString VideoThread::getVideoUrl() const
{
    return this->videoUrl;
}

void VideoThread::setVideoUrl(const QString &videoUrl)
{
    //海康大华等厂家sdk只支持rtsp和mp4
#if 0
    if (videoCore == VideoCore_HaiKang || videoCore == VideoCore_DaHua) {
        if (!videoUrl.startsWith("rtsp://") && !videoUrl.endsWith(".mp4")) {
            this->videoUrl = "";
            return;
        }
    }
#endif

    this->ip = "";
    this->videoUrl = videoUrl;
    this->videoType = VideoHelper::getVideoType(videoUrl);
    this->onlyAudio = VideoHelper::getOnlyAudio(videoUrl);
}

bool VideoThread::getIsFile() const
{
    return (videoType == VideoType_FileLocal || videoType == VideoType_FileWeb);
}

bool VideoThread::getOnlyAudio() const
{
    return this->onlyAudio;
}

void VideoThread::checkVideoType()
{
    //视频文件 http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4
    //视频文件 http://vodcdn.video.taobao.com/oss/ali-video/bbae2fc164d9517913ed5a9117f8455b/video.m3u8
    //特别提示 有些可能带了 cache: 开头/这个是ffmpeg独有的写法
    QString url = videoUrl;
    url.replace("cache:", "");
    bool http = (url.startsWith("http://") || url.startsWith("https://"));
    bool web = (http || url.startsWith("rtsp://"));
    if (duration > 0) {
        if (videoCore == VideoCore_Vlc) {
            //经过测试发现实时视频流会发送个固定90000=90s时长的数据
            if (duration == 90000) {
                if (http) {
                    videoType = VideoType_Http;
                }
            } else {
                if (web) {
                    videoType = VideoType_FileWeb;
                }
            }
        } else if (videoCore == VideoCore_Mpv) {
            //经过测试发现实时视频流也会不断触发时长和播放进度变化
            //如果是单纯的视频文件(本地视频/网络视频)只会触发1-2次时长变化和播放进度变化
            if (playCount == 5) {
                if (duration == firstDuration) {
                    QMetaObject::invokeMethod(this, "receiveDuration", Q_ARG(qint64, duration));
                    if (web) {
                        videoType = VideoType_FileWeb;
                    }
                } else {
                    if (http) {
                        videoType = VideoType_Http;
                    }
                }
            }
        } else if (videoCore == VideoCore_Mdk) {
            //限定低于2s时长的为视频流
            if (duration < 2000) {
                if (http) {
                    videoType = VideoType_Http;
                }
            } else {
                if (web) {
                    videoType = VideoType_FileWeb;
                }
            }
        } else if (videoCore == VideoCore_EasyPlayer) {
            //经过测试发现实时视频流会发送个固定1s时长的数据
            if (duration == 1) {
                if (http) {
                    videoType = VideoType_Http;
                }
            } else {
                if (web) {
                    videoType = VideoType_FileWeb;
                } else if (QFile(videoUrl).exists()) {
                    videoType = VideoType_FileLocal;
                }
            }
        } else {
            if (web) {
                videoType = VideoType_FileWeb;
            }
        }
    } else {
        if (http) {
            videoType = VideoType_Http;
        } else if (getIsFile()) {
            //有一些裸流本地文件没有时长
            videoType = VideoType_Other;
        }
    }
}

VideoType VideoThread::getVideoType() const
{
    return this->videoType;
}

QString VideoThread::getBufferSize() const
{
    return this->bufferSize;
}

void VideoThread::setBufferSize(const QString &bufferSize)
{
    this->bufferSize = bufferSize;
}

DecodeType VideoThread::getDecodeType() const
{
    return this->decodeType;
}

void VideoThread::setDecodeType(const DecodeType &decodeType)
{
    this->decodeType = decodeType;
}

QString VideoThread::getHardware() const
{
    return this->hardware;
}

void VideoThread::setHardware(const QString &hardware)
{
    this->hardware = hardware;
}

QString VideoThread::getTransport() const
{
    return this->transport;
}

void VideoThread::setTransport(const QString &transport)
{
    this->transport = transport;
}

int VideoThread::getCaching() const
{
    return this->caching;
}

void VideoThread::setCaching(int caching)
{
    this->caching = caching;
}

bool VideoThread::getAudioLevel() const
{
    return this->audioLevel;
}

void VideoThread::setAudioLevel(bool audioLevel)
{
    this->audioLevel = audioLevel;
}

bool VideoThread::getDecodeAudio() const
{
    return this->decodeAudio;
}

void VideoThread::setDecodeAudio(bool decodeAudio)
{
    this->decodeAudio = decodeAudio;
}

bool VideoThread::getPlayAudio() const
{
    return this->playAudio;
}

void VideoThread::setPlayAudio(bool playAudio)
{
    this->playAudio = playAudio;
}

bool VideoThread::getPlayRepeat() const
{
    return this->playRepeat;
}

void VideoThread::setPlayRepeat(bool playRepeat)
{
    this->playRepeat = playRepeat;
}

bool VideoThread::getCountKbps() const
{
    return this->countKbps;
}

void VideoThread::setCountKbps(bool countKbps)
{
    this->countKbps = countKbps;
}

QString VideoThread::getSecretKey() const
{
    return this->secretKey;
}

void VideoThread::setSecretKey(const QString &secretKey)
{
    this->secretKey = secretKey;
    //将原始秘钥(方便人类记忆理解)按照MD5加密转换成最终秘钥字符串(程序理解)
    QByteArray cryptoKey = QCryptographicHash::hash(secretKey.toUtf8(), QCryptographicHash::Md5).toHex();
    this->setProperty("cryptoKey", cryptoKey);
}

EncodeType VideoThread::getEncodeType() const
{
    return this->encodeType;
}

void VideoThread::setEncodeType(const EncodeType &encodeType)
{
    this->encodeType = encodeType;
}

VideoFormat VideoThread::getVideoFormat() const
{
    return this->videoFormat;
}

void VideoThread::setVideoFormat(const VideoFormat &videoFormat)
{
    this->videoFormat = videoFormat;
}

int VideoThread::getEncodeVideoFps() const
{
    return this->encodeVideoFps;
}

void VideoThread::setEncodeVideoFps(int encodeVideoFps)
{
    if (encodeVideoFps >= 1 && encodeVideoRatio <= 30) {
        this->encodeVideoFps = encodeVideoFps;
    }
}

float VideoThread::getEncodeVideoRatio() const
{
    return this->encodeVideoRatio;
}

void VideoThread::setEncodeVideoRatio(float encodeVideoRatio)
{
    if (encodeVideoRatio >= 0.1 && encodeVideoRatio < 1) {
        this->encodeVideoRatio = encodeVideoRatio;
    }
}

QString VideoThread::getEncodeVideoScale() const
{
    return this->encodeVideoScale;
}

void VideoThread::setEncodeVideoScale(const QString &encodeVideoScale)
{
    this->encodeVideoScale = encodeVideoScale;
}

int VideoThread::getOpenSleepTime() const
{
    return this->openSleepTime;
}

void VideoThread::setOpenSleepTime(int openSleepTime)
{
    this->openSleepTime = openSleepTime;
}

int VideoThread::getReadTimeout() const
{
    return this->readTimeout;
}

void VideoThread::setReadTimeout(int readTimeout)
{
    this->readTimeout = readTimeout;
}

int VideoThread::getConnectTimeout() const
{
    return this->connectTimeout;
}

void VideoThread::setConnectTimeout(int connectTimeout)
{
    this->connectTimeout = connectTimeout;
}

void VideoThread::readMediaInfo()
{

}

void VideoThread::setAudioTrack(int audioTrack)
{
    this->audioTrack = audioTrack;
}

void VideoThread::setVideoTrack(int videoTrack)
{
    this->videoTrack = videoTrack;
}

void VideoThread::setTrack(int audioTrack, int videoTrack)
{
    this->audioTrack = audioTrack;
    this->videoTrack = videoTrack;
    this->setAudioTrack(audioTrack);
    this->setVideoTrack(videoTrack);
}

void VideoThread::setTrack()
{
    //如果设置过了则立即切换轨道
    if (audioTrack >= 0 && audioTracks.contains(audioTrack)) {
        this->setAudioTrack(audioTrack);
    }
    if (videoTrack >= 0 && videoTracks.contains(videoTrack)) {
        this->setVideoTrack(videoTrack);
    }

    if (audioTracks.count() > 0 || videoTracks.count() > 0) {
        emit receiveTrack(audioTracks, videoTracks);
    }
}

int VideoThread::getRotate()
{
    return rotate;
}

void VideoThread::setRotate(int rotate)
{
    this->rotate = rotate;
}

void VideoThread::setAspect(double width, double height)
{

}

qint64 VideoThread::getDuration()
{
    return duration;
}

qint64 VideoThread::getPosition()
{
    return position;
}

void VideoThread::setPosition(qint64 position)
{
    emit receivePosition(position);
}

double VideoThread::getSpeed()
{
    return 1;
}

void VideoThread::setSpeed(double speed)
{

}

int VideoThread::getVolume()
{
    return 100;
}

void VideoThread::setVolume(int volume)
{
    emit receiveVolume(volume);
}

bool VideoThread::getMuted()
{
    return false;
}

void VideoThread::setMuted(bool muted)
{
    emit receiveMuted(muted);
}

bool VideoThread::getPlayStep() const
{
    return this->playStep;
}

void VideoThread::setPlayStep(bool playStep)
{
    if (this->playStep != playStep) {
        playStep ? this->pause() : this->next();
        this->playStep = playStep;
    }
}

QString VideoThread::getAudioDevice() const
{
    return this->audioDevice;
}

void VideoThread::setAudioDevice(const QString &audioDevice)
{
    this->audioDevice = audioDevice;
}

void VideoThread::stop2()
{
    if (readTimeout > 0 && isOk && isRunning()) {
        debug("重新打开", "");
        isOk = false;
        return;
    }

    //存在显示窗体则调用显示窗体的停止更彻底
    if (videoWidget) {
        QMetaObject::invokeMethod(videoWidget, "stop");
    } else {
        this->stop();
    }
}

void VideoThread::snapFinsh()
{
    //文件已经存在不用重新保存(png格式重新保存为jpg以减少体积)
    QImage image(snapName);
    if (snapName.endsWith(".png")) {
        QFile(snapName).remove();
        snapName = snapName.replace(".png", ".jpg");
        //vlc内核需要重新旋转
        if (videoCore == VideoCore_Vlc) {
            VideoHelper::rotateImage(rotate, image);
        }
        image.save(snapName, "jpg");
    }

    isSnap = false;
    emit snapImage(image, snapName);
}

void VideoThread::openFinsh(bool start)
{
    isOk = true;
    this->updateTime();
    emit recorderStateChanged(RecorderState_Stopped, fileName);
    int time = timer.elapsed();
    debug("打开成功", QString("用时: %1 毫秒").arg(time));

    //发送播放完成信号
    if (start) {
        emit receivePlayStart(time);
    }
}

void VideoThread::step(bool backward)
{
    //本身没有逐帧播放接口/按照帧率计算的间隔来跳转播放进度实现
    //如果子类实现了step函数则会执行子类的代码
    if (getIsFile() && playStep) {
        int offset = 1000 / 25;
        if (frameRate > 0) {
            offset = 1000 / frameRate;
        }

        qint64 position = this->getPosition();
        position = (backward ? (position - offset) : (position + offset));
        this->setPosition(position);
    }
}
