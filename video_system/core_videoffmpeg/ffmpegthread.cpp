#include "ffmpegthread.h"
#include "ffmpeghelper.h"
#include "ffmpegutil.h"
#include "ffmpegfilter.h"
#include "ffmpegsync.h"
#include "ffmpegsave.h"
#include "videohelper.h"

#ifdef audiox
#include "../core_audio/audioplayer.h"
#else
#include "../core_videobase/audioplayer.h"
#endif

FFmpegThread::FFmpegThread(QObject *parent) : VideoThread(parent)
{
    //执行初始化
    FFmpegHelper::initLib();

    changePosition = false;
    pushPreview = true;
    needClear = false;
    keyFrame = true;

    speed = 1.0;
    videoIndex = -1;
    audioIndex = -1;

    keyPacketIndex = 0;
    keyPacketCount = 0;
    realPacketSize = 0;
    realPacketCount = 0;

    videoFirstPts = 0;
    audioFirstPts = 0;

    packet = NULL;
    videoFrame = NULL;
    audioFrame = NULL;

    yuvFrame = NULL;
    imageFrame = NULL;
    tempFrame = NULL;

    yuvData = NULL;
    imageData = NULL;
    pcmData = NULL;

    yuvSwsCtx = NULL;
    imageSwsCtx = NULL;
    pcmSwrCtx = NULL;

    options = NULL;
    ifmt = NULL;
    formatCtx = NULL;

    videoCodec = NULL;
    audioCodec = NULL;

    videoCodecCtx = NULL;
    audioCodecCtx = NULL;

    saveVideoType = SaveVideoType_Mp4;
    saveAudioType = SaveAudioType_None;

    //初始化音频解码同步线程(本线程释放的时候也会自动释放)
    useSync = true;
    audioSync = new FFmpegSync(0, this);
    connect(audioSync, SIGNAL(receivePosition(qint64)), this, SIGNAL(receivePosition(qint64)));

    //初始化视频解码同步线程(本线程释放的时候也会自动释放)
    videoSync = new FFmpegSync(1, this);
    connect(videoSync, SIGNAL(receivePosition(qint64)), this, SIGNAL(receivePosition(qint64)));

    //实例化恢复暂停定时器(单次定时器)
    timerPause = new QTimer(this);
    connect(timerPause, SIGNAL(timeout()), this, SLOT(pause()));
    timerPause->setInterval(100);
    timerPause->setSingleShot(true);

    //实例化切换进度定时器(单次定时器)
    timerPosition = new QTimer(this);
    connect(timerPosition, SIGNAL(timeout()), this, SLOT(setPosition()));
    timerPosition->setInterval(200);
    timerPosition->setSingleShot(true);

    //实例化音频播放类
    audioPlayer = new AudioPlayer;
    connect(audioPlayer, SIGNAL(receiveVolume(int)), this, SIGNAL(receiveVolume(int)));
    connect(audioPlayer, SIGNAL(receiveMuted(bool)), this, SIGNAL(receiveMuted(bool)));
    connect(audioPlayer, SIGNAL(receiveLevel(qreal, qreal)), this, SIGNAL(receiveLevel(qreal, qreal)));

    //裁剪标志位和最后结束裁剪时间
    isCrop = false;
    cropTime = QDateTime::currentDateTime().addDays(-1);
    filterCount = 0;
}

FFmpegThread::~FFmpegThread()
{
    //释放音频播放类
    audioPlayer->deleteLater();
}

void FFmpegThread::run()
{
    while (!stopped) {
        if (!isOk) {
            //记住开始解码的时间用于用音视频同步等
            startTime = av_gettime();
            this->closeVideo();
            this->openVideo();
            this->checkOpen();
            continue;
        }

        //暂停及切换进度期间继续更新时间防止超时
        if (isPause || changePosition) {
            this->updateTime();
            this->readAndClear();
            msleep(1);
            continue;
        }

        //解码队列中帧数过多暂停读取(倍速越大数量也大)
        int max = speed * 100;
        if (useSync && (videoSync->getPacketCount() >= max || audioSync->getPacketCount() >= max)) {
            this->updateTime();
            msleep(1);
            continue;
        }

        //读取一帧(通过标志位控制回调那边做超时判断)
        tryRead = true;
        int result = av_read_frame(formatCtx, packet);
        tryRead = false;
        if (result >= 0) {
            //错误计数清零以及更新最后的解码时间
            errorCount = 0;
            this->updateTime();

            //判断当前包是视频还是音频
            int index = packet->stream_index;
            //QString msg = QString("time: %1 pts: %2 dts: %3 pos: %4").arg(qint64(FFmpegHelper::getPtsTime(formatCtx, packet) / 1000)).arg(packet->pts).arg(packet->dts).arg(position);
            if (index == videoIndex) {
                decodeVideo0(packet);
                //debug(0, "视频数据", msg);
            } else if (index == audioIndex) {
                decodeAudio0(packet);
                //debug(0, "音频数据", msg);
            }
        } else if (getIsFile()) {
            //如果是视频文件则判断是否文件结束
            if (result == AVERROR_EOF || result == AVERROR_EXIT) {
                //当同步队列中的数量为0(表示解码处理完成)才需要跳出
                if (videoSync->getPacketCount() == 0 && audioSync->getPacketCount() == 0) {
                    if (playRepeat) {
                        this->replay();
                    } else {
                        this->reopen();
                        continue;
                    }
                }
            }
        } else {
            //下面这种情况在视频流断开后出现(一般3秒钟来一次)
            errorCount++;
            debug(result, "错误计数", QString("次数: %1").arg(errorCount));
            if (errorCount >= 3) {
                this->reopen();
                continue;
            }
        }

        av_packet_unref(packet);
        //倍速播放使劲的解码不需要休息(同步线程那边休息了下就行)
        //发现有些环境下播放卡/很可能是下面msleep(1)引起的/注释掉就行/这些环境下可能休眠了20ms/非常离谱颠覆认知
        //增加缓存大小100MB用于去掉这个延迟/方便测试用
        if (speed <= 1 && caching != 102400000) {
            msleep(1);
        }
    }

    //关闭视频
    this->closeVideo();
    debug(0, "线程结束", "");
}

void FFmpegThread::readAndClear()
{
    //视频流要继续读取不然会一直累积
    if (!getIsFile() && videoType != VideoType_Desktop /*&& videoType != VideoType_Camera*/) {
        av_read_frame(formatCtx, packet);
        av_packet_unref(packet);
    }
}

void FFmpegThread::replay()
{
    //加个时间限制防止频繁执行
    QDateTime now = QDateTime::currentDateTime();
    if (qAbs(now.msecsTo(lastTime)) < 300) {
        return;
    }

    lastTime = now;
    videoSync->reset();
    audioSync->reset();
    //有些文件前面几帧缺失导致播放定位失败
    this->position = (videoFirstPts > 0 ? 1000 : 0);
    this->setPosition();
    debug(0, "循环播放", "");
}

void FFmpegThread::reopen()
{
    //释放数据并判断是否需要重连
    av_packet_unref(packet);
    this->stop2();
    msleep(100);
}

void FFmpegThread::checkKeyPacketCount(AVPacket *packet)
{
    keyPacketIndex++;
    keyPacketCount++;
    if (packet->flags & AV_PKT_FLAG_KEY) {
        debug(0, "关键帧", QString("间隔: %1 当前: %2").arg(keyPacketCount).arg(keyPacketIndex - 1));
        keyPacketCount = 0;
    }
}

void FFmpegThread::checkRealPacketSize(AVPacket *packet, int maxCount)
{
    realPacketCount++;
    realPacketSize += packet->size;
    if (realPacketCount == maxCount) {
        qreal kbps = (qreal)realPacketSize / 100;
        //debug(0, "实时码率", QString("大小: %1 kbps").arg(QString::number(kbps, 'f', 0)));
        emit receiveKbps(kbps, maxCount);
        realPacketSize = 0;
        realPacketCount = 0;
    }
}

bool FFmpegThread::checkFrameSize(AVFrame *frame)
{
    //有些视频流会动态改变分辨率需要重新打开(主动设置过旋转角度的不用处理)
    //执行了裁剪滤镜也会触发变化
    int width = frame->width;
    int height = frame->height;
    if (width > 0 && height > 0 && (videoWidth != width || videoHeight != height)) {
        //先要根据旋转角度调整尺寸
        int w = width;
        int h = height;
        VideoHelper::rotateSize(rotate, w, h);
        if (isCrop) {
            this->checkVideoSize(w, h);
        } else {
            //此时是取消裁剪后触发的分辨率变化
            qint64 offset = cropTime.msecsTo(QDateTime::currentDateTime());
            if (offset < 1000) {
                this->checkVideoSize(w, h);
            } else if (rotate <= 0 && videoType == VideoType_Rtsp && hardware == "none") {
                debug(0, "尺寸变化", QString("变化: %1x%2 -> %3x%4").arg(videoWidth).arg(videoHeight).arg(width).arg(height));
                isOk = false;
                return false;
            }
        }
    }

    return true;
}

bool FFmpegThread::scaleAndSaveVideo(bool &needScale, AVFrame *frame)
{
    if (!checkFrameSize(frame)) {
        return false;
    }

    //不是yuv420则先要转换(本地摄像头格式yuyv422/还有些视频文件是各种各样的格式)
    AVPixelFormat format = videoCodecCtx->pix_fmt;
    //很多摄像头视频流是yuvj420也不需要转换可以直接用(硬解码模式下格式是NV12所以needScale永远为真)
    needScale = (format != AV_PIX_FMT_YUV420P && format != AV_PIX_FMT_YUVJ420P);

    //非录像阶段硬解码模式下不需要转换
    if (!isRecord && hardware != "none") {
        needScale = false;
    }

    //在录像阶段硬解码模式下非文件开启了全部转码才需要转换
    if (isRecord && !getIsFile() && encodeType != EncodeType_All && hardware != "none") {
        needScale = false;
    }

    //非录像阶段必须处于截图和绘制模式下才需要转换
    if (!isRecord && (isSnap || videoMode == VideoMode_Painter)) {
        needScale = false;
    }

    //将非yuv420格式转换到yuv420格式
    if (needScale) {
        int result = sws_scale(yuvSwsCtx, (const quint8 **)frame->data, frame->linesize, 0, videoHeight, yuvFrame->data, yuvFrame->linesize);
        if (result < 0) {
            debug(result, "转换失败", QString("格式: %1").arg(FFmpegHelper::getPixFormatString(format)));
            return false;
        }
    }

    //转换过的用转换后的数据
    this->writeFile(needScale ? yuvFrame : frame, true);
    return true;
}

void FFmpegThread::checkAndShowVideo(bool needScale, AVFrame *frame)
{
    //截图和绘制都转成图片
    if (isSnap || videoMode == VideoMode_Painter) {
        //启动计时
        timer.restart();
        //将数据转成图片
        int result = sws_scale(imageSwsCtx, (const quint8 **)frame->data, frame->linesize, 0, videoHeight, imageFrame->data, imageFrame->linesize);
        if (result < 0) {
            return;
        }

        QImage image((quint8 *)imageData, videoWidth, videoHeight, QImage::Format_RGB888);
        if (image.isNull()) {
            return;
        }

        //如果有旋转角度先要旋转
        VideoHelper::rotateImage(rotate, image);
        if (isSnap) {
            isSnap = false;
            //裁剪期间应用了裁剪滤镜对应的截图有问题
            if (isCrop) {
                int width, height;
                FFmpegHelper::getResolution(formatCtx->streams[videoIndex], width, height);
                if (width != videoWidth || height != videoHeight) {
                    return;
                }
            }

            image.save(this->snapName, "jpg");
            QMetaObject::invokeMethod(this, "snapFinsh");
            //带旋转角度的视频抓图后需要重新初始化滤镜
            if (rotate > 0) {
                videoFilter.init = false;
            }
        } else {
            emit receiveImage(image, timer.elapsed());
        }
    } else {
        //可能有tile滤镜导致产生空帧
        if (frame->width == 0) {
            return;
        }
        //qDebug() << TIMEMS << videoWidth << videoHeight << frame->width << frame->height << frame->linesize[0] << frame->linesize[1] << frame->linesize[2];
        if (hardware == "none") {
            if (needScale) {
                emit receiveFrame(videoWidth, videoHeight, yuvFrame->data[0], yuvFrame->data[1], yuvFrame->data[2], videoWidth, videoWidth / 2, videoWidth / 2);
                this->writeVideoData(videoWidth, videoHeight, yuvFrame->data[0], yuvFrame->data[1], yuvFrame->data[2]);
            } else {
                emit receiveFrame(frame->width, frame->height, frame->data[0], frame->data[1], frame->data[2], frame->linesize[0], frame->linesize[1], frame->linesize[2]);
                this->writeVideoData(frame->linesize[0], frame->height, frame->data[0], frame->data[1], frame->data[2]);
            }
        } else {
            emit receiveFrame(videoWidth, videoHeight, frame->data[0], frame->data[1], frame->linesize[0], frame->linesize[1]);
        }
    }
}

void FFmpegThread::decodeVideo0(AVPacket *packet)
{
    //checkKeyPacketCount(packet);
    if (countKbps) {
        checkRealPacketSize(packet, frameRate);
    }

    //有些首帧开始的时间不是0而是一个指定的时间所以需要减去
    if (videoFirstPts > 5000) {
        packet->pts -= videoFirstPts;
        packet->dts = packet->pts;
    }

    if (useSync) {
        //加入到队列交给解码同步线程处理
        videoSync->append(FFmpegHelper::creatPacket(packet));
    } else {
        //直接当前线程解码
        decodeVideo1(packet);
        if (decodeType != DecodeType_Fastest) {
            FFmpegHelper::delayTime(formatCtx, packet, startTime);
        }
    }
}

void FFmpegThread::decodeVideo1(AVPacket *packet)
{
    this->writeFile(packet, true);
    //未编码推流不用继续(可以节约大量CPU/预览还需要继续解码)
    //未编码的才存在缓存数据需要清空
    if (!saveFile->getEncodeVideo()) {
        if (needClear) {
            keyFrame = false;
            this->clearBuffer(true);
            return;
        }
        if (!keyFrame) {
            keyFrame = (packet->flags & AV_PKT_FLAG_KEY);
        }
        if (!keyFrame) {
            return;
        }
    }

    if (saveFile->getIsOk() && !saveFile->getEncodeVideo() && !pushPreview) {
        return;
    }

    if (hardware == "none") {
        FFmpegHelper::decode(this, videoCodecCtx, packet, videoFrame, true);
    } else {
        FFmpegHelper::decode(this, videoCodecCtx, packet, tempFrame, videoFrame);
    }
}

void FFmpegThread::decodeVideo2(AVPacket *packet)
{
    //判断帧类型 I B P
#if 0
    int type = (hardware == "none" ? videoFrame->pict_type : tempFrame->pict_type);
    //qDebug() << TIMEMS << type;
    if (type == 3) {
        qDebug() << TIMEMS << type;
        //return;
    }
#endif

    //如果需要重新初始化则先初始化滤镜(带旋转角度的抓图也需要重新处理)
    if (!videoFilter.init || (isSnap && rotate > 0)) {
        filterCount = (!videoFilter.init ? 2 : 0);
        this->initFilter();
    }

    //刚刚初始化滤镜完成后的几帧暂时不处理(防止绘制那边数据冲突导致崩溃)
    if (filterCount > 0) {
        filterCount--;
        //电子放大期间需要及时处理不需要暂停
        if (!isCrop) {
            return;
        }
    }

    //先处理滤镜(软解码需要数据中转一下/硬解码源头就已经中转)
    AVFrame *frame = videoFrame;
    if (videoFilter.isOk) {
        if (av_buffersrc_add_frame(videoFilter.filterSrcCtx, videoFrame) >= 0) {
            if (hardware == "none") {
                frame = tempFrame;
                av_frame_unref(tempFrame);
                av_buffersink_get_frame(videoFilter.filterSinkCtx, tempFrame);
            } else {
                av_buffersink_get_frame(videoFilter.filterSinkCtx, videoFrame);
            }
        }
    }

    bool needScale = false;
    if (!scaleAndSaveVideo(needScale, frame)) {
        return;
    }

    this->checkAndShowVideo(needScale, frame);

    //开启了逐帧播放则每次播放完一帧就暂停
    if (playStep) {
        this->pause();
    }
}

void FFmpegThread::decodeAudio0(AVPacket *packet)
{
    //如果没有开启则不用继续
    if (!decodeAudio) {
        return;
    }

    if (audioFirstPts > AV_TIME_BASE) {
        packet->pts -= audioFirstPts;
        packet->dts = packet->pts;
    }

    if (useSync) {
        //加入到队列交给解码同步线程处理
        audioSync->append(FFmpegHelper::creatPacket(packet));
    } else {
        //直接当前线程解码
        decodeAudio1(packet);
        if (decodeType != DecodeType_Fastest) {
            FFmpegHelper::delayTime(formatCtx, packet, startTime);
        }
    }
}

void FFmpegThread::decodeAudio1(AVPacket *packet)
{
    //限制超过一定倍速不解码音频数据
    if (speed >= 5) {
        //return;
    }

    this->writeFile(packet, false);
    //未编码推流不用继续(可以节约大量CPU/预览还需要继续解码)
    if (saveFile->getIsOk() && !saveFile->getEncodeAudio() && !pushPreview) {
        return;
    }

    FFmpegHelper::decode(this, audioCodecCtx, packet, audioFrame, false);
    this->writeFile(audioFrame, false);
}

void FFmpegThread::decodeAudio2(AVPacket *packet)
{
    int channel = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    int len = av_samples_get_buffer_size(NULL, channel, audioFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);
    int result = swr_convert(pcmSwrCtx, &pcmData, len, (const quint8 **)audioFrame->data, audioFrame->nb_samples);
    if (result >= 0) {
        //播放声音数据
        char *data = (char *)pcmData;
        if (playAudio && !timerPause->isActive()) {
            //audioPlayer->playAudioData(data, len);
            QMetaObject::invokeMethod(audioPlayer, "playAudioData", Q_ARG(const char *, data), Q_ARG(qint64, len));
        }

        //保存音频文件
        if (saveAudioType == SaveAudioType_Pcm || saveAudioType == SaveAudioType_Wav) {
            this->writeAudioData(data, len);
        } else if (saveAudioType == SaveAudioType_Aac) {
            this->writeAudioData((char *)packet->data, packet->size);
        }
    }
}

void FFmpegThread::writeFile(AVPacket *packet, bool video)
{
    if (saveFile->getIsOk()) {
        if (video) {
            if (!saveFile->getEncodeVideo()) {
                saveFile->writePacket(packet, videoIndex);
            }
        } else {
            if (!saveFile->getEncodeAudio()) {
                saveFile->writePacket(packet, audioIndex);
            }
        }
    }
}

void FFmpegThread::writeFile(AVFrame *frame, bool video)
{
    if (saveFile->getIsOk()) {
        //裁剪滤镜不支持录像存储
        if (isCrop) {
            if (!saveFile->getIsPause()) {
                saveFile->pause();
            }
            return;
        } else {
            if (isRecord && saveFile->getIsPause()) {
                saveFile->pause();
            }
        }

        if (video) {
            if (saveFile->getEncodeVideo()) {
                saveFile->writeFrame(frame, videoIndex);
            }
        } else {
            if (saveFile->getEncodeAudio()) {
                saveFile->writeFrame(frame, audioIndex);
            }
        }
    }
}

void FFmpegThread::initOption()
{
    //增加rtp/sdp支持(后面发现不要加)
    if (videoUrl.endsWith(".sdp")) {
        //av_dict_set(&options, "protocol_whitelist", "file,rtp,udp", 0);
    }

    //设置缓存大小/通信协议
    FFmpegHelper::initOption(&options, caching, transport);
    //设置分辨率/帧率/桌面采集偏移值等参数
    if (videoType == VideoType_Camera || videoType == VideoType_Desktop) {
        FFmpegHelper::initOption(&options, bufferSize, frameRate);
        if (videoType == VideoType_Desktop) {
            int offsetX = this->property("offsetX").toInt();
            int offsetY = this->property("offsetY").toInt();
            FFmpegHelper::initOption(&options, offsetX, offsetY, videoUrl);
        }
    }

    //如果设置了秘钥则需要启用解密
    if (!secretKey.isEmpty()) {
        QByteArray cryptoKey = this->property("cryptoKey").toByteArray();
        av_dict_set(&options, "decryption_key", cryptoKey.constData(), 0);
    }
}

bool FFmpegThread::initInput()
{
    //本地摄像头/桌面录屏/linux系统可以打开cheese程序查看本地摄像头(如果是在虚拟机中需要设置usb选项3.1)
    if (videoType == VideoType_Camera) {
        ifmt = av_find_input_format(Device_Flag);
    } else if (videoType == VideoType_Desktop) {
        ifmt = av_find_input_format(Desktop_Flag);
    }

    //实例化格式处理上下文
    formatCtx = avformat_alloc_context();
    //设置超时回调(有些不存在的地址或者网络不好的情况下要卡很久)
    formatCtx->interrupt_callback.callback = FFmpegHelper::openAndReadCallBack;
    formatCtx->interrupt_callback.opaque = this;

    //打开输入(通过标志位控制回调那边做超时判断)
    //其他地方调用 formatCtx->url formatCtx->filename 可以拿到设置的地址(两个变量值一样)
    tryOpen = true;
    QString url = VideoHelper::getRightUrl(videoType, videoUrl);
    int result = avformat_open_input(&formatCtx, url.toUtf8().constData(), ifmt, &options);
    tryOpen = false;
    if (result < 0) {
        debug(result, "打开地址", "");
        emit receivePlayError(VideoError_Open);
        return false;
    }

    //根据自己项目需要开启下面部分代码加快视频流打开速度
    //开启后由于值太小可能会出现部分视频流获取不到分辨率
    if (decodeType == DecodeType_Fastest && videoType == VideoType_Rtsp) {
        FFmpegHelper::initRtspFast(formatCtx);
    }

    //获取流信息
    result = avformat_find_stream_info(formatCtx, NULL);
    if (result < 0) {
        debug(result, "找流失败", "");
        return false;
    }

    //解码格式
    formatName = formatCtx->iformat->name;
    //某些格式比如视频流不做音视频同步(响应速度快)
    if (formatName == "rtsp" || videoUrl.endsWith(".sdp")) {
        useSync = false;
    }

    //设置了最快速度则不启用音视频同步(质量优先需要同步)
    if (decodeType == DecodeType_Fastest) {
        useSync = false;
    } else if (decodeType == DecodeType_Full) {
        useSync = true;
    }

    //有些格式不支持硬解码
    static QStringList formats;
    if (formats.count() == 0) {
        formats << "rm" << "avi" << "webm";
    }

    foreach (QString format, formats) {
        if (formatName.contains(format)) {
            hardware = "none";
            break;
        }
    }

    //本地设备采集不需要同步
    if (videoType == VideoType_Camera || videoType == VideoType_Desktop) {
        useSync = false;
    }

    //过低版本不支持硬解码
#if (FFMPEG_VERSION_MAJOR < 3)
    hardware = "none";
#endif

    //安卓上默认提供的库不带硬解码(需要自行编译带硬解的库)
#ifdef Q_OS_ANDROID
    hardware = "none";
#endif

    //获取文件时长(这里获取到的是秒)
    double length = (double)formatCtx->duration / AV_TIME_BASE;
    //如果是本地文件而且没有时长则用最原始方法读取时长
    //有部分设备导出的视频文件读取出来时长不对也可以用此方法读取
    if (videoType == VideoType_FileLocal && duration <= 0) {
        if (this->property("getDurationByFrame").toBool()) {
            length = FFmpegUtil::getDuration(videoUrl);
        }
    }

    duration = length * 1000;
    this->checkVideoType();

    //有时候网络地址也可能是纯音频
    if (videoType == VideoType_FileWeb) {
        onlyAudio = VideoHelper::getOnlyAudio(videoUrl, formatName);
    }

    //文件必须要音视频同步(手动指定了最快速度用来测试最快解码/也不需要同步)
    if (getIsFile() && !this->property("fastest").toBool()) {
        useSync = true;
        //发送文件时长信号
        emit receiveDuration(duration > 0 ? duration : 0);
    }

    //获取音视频轨道信息(一般有一个音频或者一个视频/ts节目文件可能有多个)
    FFmpegHelper::getTracks(formatCtx, audioTracks, videoTracks);
    emit receiveTrack(audioTracks, videoTracks);
    QString msg = QString("格式: %1 时长: %2 秒 加速: %3").arg(formatName).arg(duration / 1000).arg(hardware);
    debug(0, "媒体信息", msg);
    return true;
}

bool FFmpegThread::initVideo()
{
    //找到视频流索引
    videoIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        //有些没有视频流所以这里不用返回
        videoIndex = -1;
        debug(0, "无视频流", "");
    } else {
        //如果手动指定了轨道则取指定的(节目流有多个轨道可以指定某个)
        if (videoTrack >= 0 && videoTracks.contains(videoTrack)) {
            videoIndex = videoTrack;
        }

        //取出流获取对应的信息创建解码器
        int result = -1;
        AVStream *videoStream = formatCtx->streams[videoIndex];

        //如果主动设置过旋转角度则将旋转信息设置到流信息中以便保存那边也应用(不需要保存也旋转可以注释)
        if (rotate != -1) {
            FFmpegHelper::setRotate(videoStream, rotate);
        }

        //先获取旋转角度(如果有旋转角度则不能用硬件加速)
        this->getRotate();
        if (rotate != 0) {
            hardware = "none";
        }

        //查找视频解码器(如果上面av_find_best_stream第五个参数传了则这里不需要)
        AVCodecID codecID = FFmpegHelper::getCodecId(videoStream);
        if (codecID == AV_CODEC_ID_NONE) {
            debug(result, "无视解码", "");
            return false;
        }

        //获取默认的解码器
        videoCodec = avcodec_find_decoder(codecID);
        videoCodecName = videoCodec->name;

        //自动纠正硬解码失败情况下转成软解码
#ifdef Q_OS_WIN
        //264/265才能去启用系统层以外的硬解码
        if (hardware != "none" && hardware != "dxva2" && hardware != "d3d11va") {
            if (videoCodecName != "h264" && videoCodecName != "hevc") {
                hardware = "none";
            }
        }

        //指定硬解码器名称 h264_qsv/h264_cuvid/h264_vaapi hevc_qsv/hevc_cuvid/hevc_vaapi
        if (hardware != "none" && hardware != "dxva2" && hardware != "d3d11va") {
            QString name = QString("%1_%2").arg(videoCodecName).arg(hardware);
            videoCodec = avcodec_find_decoder_by_name(name.toUtf8().constData());
            //如果硬解码器分配失败则立即切换到软解码
            if (!videoCodec) {
                hardware = "none";
                videoCodec = avcodec_find_decoder(codecID);
            }
        }
#endif

        //创建视频解码器上下文
        videoCodecCtx = avcodec_alloc_context3(NULL);
        if (!videoCodecCtx) {
            debug(result, "创建视解", "");
            return false;
        }

        result = FFmpegHelper::copyContext(videoCodecCtx, videoStream, false);
        if (result < 0) {
            debug(result, "视频参数", "");
            return false;
        }

        //初始化硬件加速(也可以叫硬解码/如果当前格式不支持硬解则立即切换到软解码)
        if (hardware != "none" && !initHardware()) {
            hardware = "none";
            videoCodec = avcodec_find_decoder(codecID);
        }

        if (!videoCodec) {
            return false;
        }

        //设置低延迟和加速解码等参数(设置max_lowres的话很可能画面采用最小的分辨率)
        if (!getIsFile()) {
            //videoCodecCtx->lowres = videoCodec->max_lowres;
            videoCodecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
            videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            videoCodecCtx->flags2 |= AV_CODEC_FLAG2_FAST;
        }

        //打开视频解码器
        result = avcodec_open2(videoCodecCtx, videoCodec, NULL);
        if (result < 0) {
            debug(result, "打开视解", "");
            return false;
        }

        if (videoCodecCtx->pix_fmt == AV_PIX_FMT_NONE) {
            debug(0, "格式为空", "");
            return false;
        }

        //获取分辨率大小
        FFmpegHelper::getResolution(videoStream, videoWidth, videoHeight);
        //如果没有获取到宽高则返回
        if (videoWidth == 0 || videoHeight == 0) {
            debug(0, "无分辨率", "");
            return false;
        }

        //记录首帧开始时间和解码器名称
        videoFirstPts = videoStream->start_time;
        videoCodecName = videoCodec->name;
        frameRate = FFmpegHelper::getFrameRate(videoStream, formatName);
        QString msg = QString("索引: %1 解码: %2 帧率: %3 宽高: %4x%5 角度: %6").arg(videoIndex).arg(videoCodecName).arg(frameRate).arg(videoWidth).arg(videoHeight).arg(rotate);
        debug(0, "视频信息", msg);
    }

    return true;
}

bool FFmpegThread::initHardware()
{
#if (FFMPEG_VERSION_MAJOR > 2)
    //根据名称自动寻找硬解码
    enum AVHWDeviceType type;
    QByteArray hwData = hardware.toUtf8();
    const char *hwName = hwData.constData();
#ifdef __arm__
    //发现嵌入式上低版本的库没有av_hwdevice_find_type_by_name函数
#if (FFMPEG_VERSION_MAJOR < 4)
    return false;
#else
    type = av_hwdevice_find_type_by_name(hwName);
#endif
#else
    type = av_hwdevice_find_type_by_name(hwName);
#endif

    //找到对应的硬解码格式
    debug(0, "硬件加速", QString("名称: %1 数值: %2").arg(hardware).arg(type));
    FFmpegHelper::hw_pix_fmt = FFmpegHelper::find_fmt_by_hw_type(type, videoCodec);
    if (FFmpegHelper::hw_pix_fmt == -1) {
        debug(0, "加速失败", "");
        return false;
    }

    int result = -1;
    //创建硬解码设备
    AVBufferRef *hw_device_ref;
    result = av_hwdevice_ctx_create(&hw_device_ref, type, NULL, NULL, 0);
    if (result < 0) {
        debug(result, "创建硬解", "av_hwdevice_ctx_create");
        return false;
    }

    //解码器格式赋值为硬解码
    videoCodecCtx->get_format = FFmpegHelper::get_hw_format;
    videoCodecCtx->hw_device_ctx = av_buffer_ref(hw_device_ref);
    av_buffer_unref(&hw_device_ref);
    debug(result, "初始硬解", QString("成功: %1").arg(hardware));
    return true;
#else
    return false;
#endif
}

bool FFmpegThread::initAudio()
{
    //找到音频流索引
    audioIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIndex < 0) {
        //有些没有音频流所以这里不用返回
        audioIndex = -1;
        debug(0, "无音频流", "");
    } else {
        //如果手动指定了轨道则取指定的(节目流有多个轨道可以指定某个)
        if (audioTrack >= 0 && audioTracks.contains(audioTrack)) {
            audioIndex = audioTrack;
        }

        //取出流获取对应的信息创建解码器
        int result = -1;
        AVStream *audioStream = formatCtx->streams[audioIndex];

        //查找音频解码器(如果上面av_find_best_stream第五个参数传了则这里不需要)
        AVCodecID codecID = FFmpegHelper::getCodecId(audioStream);
        if (codecID == AV_CODEC_ID_NONE) {
            debug(result, "无音解码", "");
            return false;
        }

        audioCodec = avcodec_find_decoder(codecID);
        //audioCodec = avcodec_find_decoder_by_name("aac");
        if (!audioCodec) {
            return false;
        }

        //创建音频解码器上下文
        audioCodecCtx = avcodec_alloc_context3(audioCodec);
        if (!audioCodecCtx) {
            debug(result, "创建音解", "");
            return false;
        }

        result = FFmpegHelper::copyContext(audioCodecCtx, audioStream, false);
        if (result < 0) {
            debug(result, "音频参数", "");
            return false;
        }

        //打开音频解码器
        result = avcodec_open2(audioCodecCtx, audioCodec, NULL);
        if (result < 0) {
            debug(result, "打开音解", "");
            return false;
        }

        //记录首帧开始时间和解码器名称
        audioFirstPts = audioStream->start_time;
        audioCodecName = audioCodec->name;

        //获取音频参数
        profile = audioCodecCtx->profile;
        sampleRate = audioCodecCtx->sample_rate;
        channelCount = 2;//audioCodecCtx->channels;//发现有些地址居然有6个声道
        int sampleSize = 2;//av_get_bytes_per_sample(audioCodecCtx->sample_fmt) / 2;

        //音频采样转换
        qint64 channelOut = AV_CH_LAYOUT_STEREO;
        qint64 channelIn = av_get_default_channel_layout(audioCodecCtx->channels);
        pcmSwrCtx = swr_alloc_set_opts(NULL, channelOut, AV_SAMPLE_FMT_S16, sampleRate, channelIn, audioCodecCtx->sample_fmt, sampleRate, 0, 0);

        //分配音频数据内存 192000 这个值是看ffplay代码中的
        if (swr_init(pcmSwrCtx) >= 0) {
            quint64 byte = (192000 * 3) / 2;
            pcmData = (quint8 *)av_malloc(byte * sizeof(quint8));
            if (!pcmData) {
                av_free(pcmData);
                return false;
            }
        } else {
            return false;
        }

        qint64 bitrate = FFmpegHelper::getBitRate(audioStream);
        QString msg = QString("索引: %1 解码: %2 比特: %3 声道: %4 采样: %5 品质: %6").arg(audioIndex).arg(audioCodecName).arg(bitrate).arg(channelCount).arg(sampleRate).arg(profile);
        debug(0, "音频信息", msg);
    }

    return true;
}

void FFmpegThread::initData()
{
    //分配数据包内存
    packet = FFmpegHelper::creatPacket(NULL);
    //音频流索引存在则分配音频帧内存
    if (audioIndex >= 0) {
        audioFrame = av_frame_alloc();
    }

    //视频流索引存在才需要分配视频帧内存以及其他相关设置
    if (videoIndex >= 0) {
        videoFrame = av_frame_alloc();
        yuvFrame = av_frame_alloc();
        imageFrame = av_frame_alloc();
        tempFrame = av_frame_alloc();

        //设置属性以便该帧对象正常
        yuvFrame->format = AV_PIX_FMT_YUV420P;
        yuvFrame->width = videoWidth;
        yuvFrame->height = videoHeight;

        //定义及获取像素格式
        AVPixelFormat srcFormat = AV_PIX_FMT_YUV420P;
        //重新设置源图片格式
        if (hardware == "none") {
            //通过解码器获取解码格式
            srcFormat = videoCodecCtx->pix_fmt;
        } else {
            //硬件加速需要指定格式
            srcFormat = AV_PIX_FMT_NV12;
        }

        //默认速度优先的解码采用的SWS_FAST_BILINEAR参数(可能会丢失部分图片数据)
        int flags = SWS_FAST_BILINEAR;
        if (decodeType == DecodeType_Fast) {
            flags = SWS_FAST_BILINEAR;
        } else if (decodeType == DecodeType_Full) {
            flags = SWS_BICUBIC;
        } else if (decodeType == DecodeType_Even) {
            flags = SWS_BILINEAR;
        }

        //部分视频文件需要内存对齐否则画面可能不正确(比如分辨率426x240)
        //部分视频文件字节对齐要求不能大于4(比如分辨率1480x1080)
        //部分视频文件在硬解码情况下必须16字节对齐才能正常保存(比如分辨率426x240)
        //如果在部分系统环境中发现画面斜了或者条纹状可以考虑修改这里的对齐值
        int align = (hardware == "none" ? 4 : 16);

        //分配视频帧数据(转yuv420)
        int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, videoWidth, videoHeight, align);
        yuvData = (quint8 *)av_malloc(yuvSize * sizeof(quint8));
        av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, yuvData, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, align);
        //视频图像转换(转yuv420)
        yuvSwsCtx = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, AV_PIX_FMT_YUV420P, flags, NULL, NULL, NULL);

        //分配视频帧数据(转rgb)
        align = 4;
        int imageSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, videoWidth, videoHeight, align);
        imageData = (quint8 *)av_malloc(imageSize * sizeof(quint8));
        av_image_fill_arrays(imageFrame->data, imageFrame->linesize, imageData, AV_PIX_FMT_RGB24, videoWidth, videoHeight, align);
        //视频图像转换(转rgb)
        imageSwsCtx = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, AV_PIX_FMT_RGB24, flags, NULL, NULL, NULL);

        QString srcFormatString = FFmpegHelper::getPixFormatString(srcFormat);
        QString dstFormatString = FFmpegHelper::getPixFormatString(videoMode == VideoMode_Painter ? AV_PIX_FMT_RGB24 : AV_PIX_FMT_YUV420P);
        QString msg = QString("源头: %1 目标: %2 硬解: %3").arg(srcFormatString).arg(dstFormatString).arg(hardware);
        debug(0, "格式信息", msg);
    }
}

void FFmpegThread::checkPara()
{
    //有时候上面获取到了封面作为一路视频流
    if (onlyAudio) {
        videoIndex = -1;
    }

    //部分设备比如海康不支持48000采样率(大华的居然都支持)
    if (videoType == VideoType_Rtsp && sampleRate >= 48000) {
        decodeAudio = false;
        audioIndex = -1;
    }

    //非本地摄像头设备没有音频则不用录制音频
    if (audioIndex < 0 && videoType != VideoType_Camera) {
        saveAudioType = SaveAudioType_None;
    }

    //本地摄像头强制采用wav格式
    if (videoType == VideoType_Camera) {
        if (saveAudioType > 1) {
            saveAudioType = SaveAudioType_Wav;
        }
#ifndef Q_OS_WIN
        saveAudioType = SaveAudioType_None;
#endif
    }

    //设置了保存声音并且比特=0则强制改成wav格式
    if (saveAudioType == SaveAudioType_Aac && formatCtx->bit_rate == 0) {
        saveAudioType = SaveAudioType_Wav;
    }

    //如果解码不是aac且设置了保存为aac则强制改成保存为wav
    //有些文件就算音频解码是aac然后用aac保存输出可能也有错(只需要改成wav格式100%正确)
    if (audioCodecName != "aac" && saveAudioType == SaveAudioType_Aac) {
        saveAudioType = SaveAudioType_Wav;
    }
}

void FFmpegThread::initAudioPlayer()
{
    //设置是否需要计算音频振幅
    audioPlayer->setAudioLevel(audioLevel);
    if (videoType == VideoType_Camera) {
        //本地摄像头需要打开本地音频输入
        QMetaObject::invokeMethod(audioPlayer, "openAudioInput", Q_ARG(int, 8000), Q_ARG(int, 1), Q_ARG(int, 16));
        this->openAudioOutput(800, 1, 16, 0);
        //如果设置过需要保存音频数据则关联音频数据输出到保存音频槽函数
        if (saveAudioType > 0) {
            connect(audioPlayer, SIGNAL(receiveInputData(QByteArray)), this, SLOT(writeAudioData(QByteArray)));
        }
    } else if (audioIndex >= 0) {
        this->openAudioOutput(sampleRate, channelCount, 16, 0);
    }
}

void FFmpegThread::openAudioOutput(int sampleRate, int channelCount, int sampleSize, int volume)
{
    QMetaObject::invokeMethod(audioPlayer, "openAudioOutput", Q_ARG(QString, audioDevice), Q_ARG(int, sampleRate), Q_ARG(int, channelCount), Q_ARG(int, sampleSize), Q_ARG(int, volume));
}

void FFmpegThread::initMetadata()
{
    if (!onlyAudio) {
        return;
    }

    //读取音频文件信息
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(formatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        debug(0, "专辑信息", QString("名称: %1 键值: %2").arg(tag->key).arg(tag->value));
    }

    //读取封面(纯音频在这里发送播放开始信号)
    emit receivePlayStart(timer.elapsed());
    if (formatCtx->iformat->read_header(formatCtx) >= 0) {
        for (int i = 0; i < formatCtx->nb_streams; ++i) {
            if (formatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                AVPacket pkt = formatCtx->streams[i]->attached_pic;
                QImage image = QImage::fromData((quint8 *)pkt.data, pkt.size);
                emit receiveImage(image, 0);
                break;
            }
        }
    }
}

void FFmpegThread::initFilter()
{
    if (videoWidget && !videoWidget->isVisible()) {
        return;
    }

    //很奇怪低分辨率的本地摄像头不支持滤镜(会变成左右对称的两个画面)
    if (videoIndex < 0 || videoCodecName == "rawvideo") {
        videoFilter.enable = false;
    }

    //抓图和绘制模式不用旋转
    int rotate = ((isSnap || videoMode == VideoMode_Painter) ? 0 : this->rotate);

    //初始化视频滤镜用于标签和图形绘制
    videoFilter.init = true;
    if (videoFilter.enable) {
        videoFilter.rotate = rotate;
        videoFilter.listOsd = listOsd;
        videoFilter.listGraph = listGraph;
        videoFilter.formatSrc = videoCodecCtx->pix_fmt;
        videoFilter.formatIn = (hardware == "none" ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_NV12);
        videoFilter.formatOut = (hardware == "none" ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_NV12);
        mutex.lock();
        FFmpegFilter::initFilter(this, formatCtx->streams[videoIndex], videoCodecCtx, videoFilter);
        mutex.unlock();
    }
}

void FFmpegThread::resetFilter()
{
    if (videoFilter.enable && isOk) {
        videoFilter.init = false;
    }
}

bool FFmpegThread::openVideo()
{
    //先检查地址是否正常(文件是否存在或者网络地址是否可达)
    if (!VideoHelper::checkUrl(this, videoType, videoUrl, connectTimeout)) {
        return false;
    }

    //启动计时
    timer.start();

    //初始化参数
    this->initOption();

    //初始化输入
    if (!initInput()) {
        return false;
    }

    //初始化视频
    if (!initVideo()) {
        return false;
    }

    //初始化音频(有些设备的音频格式在ffmpeg中找不到对应的解码器/G.722.1)
    if (!initAudio()) {
        decodeAudio = false;
        audioIndex = -1;
        //音视频都失败的时候不用继续
        if (videoIndex < 0) {
            return false;
        }
    }

    //分配各种内存
    this->initData();
    //检查各种参数
    this->checkPara();
    //初始化音频播放
    this->initAudioPlayer();
    //获取专辑信息
    this->initMetadata();
    //初始化滤镜
    this->initFilter();

    //启动音视频同步线程
    audioSync->start();
    videoSync->start();

    //有视频索引要触发界面通知打开完成(音频的在下面处理)
    this->openFinsh(videoIndex >= 0);
    return isOk;
}

void FFmpegThread::closeVideo()
{
    speed = 1.0;
    videoIndex = -1;
    audioIndex = -1;
    changePosition = false;

    keyPacketIndex = 0;
    keyPacketCount = 0;
    realPacketSize = 0;
    realPacketCount = 0;

    videoFirstPts = 0;
    audioFirstPts = 0;

    //停止解码同步线程
    audioSync->stop();
    videoSync->stop();

    //先停止录制
    recordStop();
    //搞个标志位判断是否需要调用父类的释放(可以防止重复调用)
    bool needClose = (onlyAudio ? audioCodecCtx : videoCodecCtx);

    if (packet) {
        FFmpegHelper::freePacket(packet);
        packet = NULL;
    }

    if (yuvSwsCtx) {
        sws_freeContext(yuvSwsCtx);
        yuvSwsCtx = NULL;
    }

    if (imageSwsCtx) {
        sws_freeContext(imageSwsCtx);
        imageSwsCtx = NULL;
    }

    if (pcmSwrCtx) {
        swr_free(&pcmSwrCtx);
        pcmSwrCtx = NULL;
    }

    if (videoFrame) {
        FFmpegHelper::freeFrame(videoFrame);
        videoFrame = NULL;
    }

    if (audioFrame) {
        FFmpegHelper::freeFrame(audioFrame);
        audioFrame = NULL;
    }

    if (yuvFrame) {
        FFmpegHelper::freeFrame(yuvFrame);
        yuvFrame = NULL;
    }

    if (imageFrame) {
        FFmpegHelper::freeFrame(imageFrame);
        imageFrame = NULL;
    }

    if (tempFrame) {
        FFmpegHelper::freeFrame(tempFrame);
        tempFrame = NULL;
    }

    if (yuvData) {
        av_free(yuvData);
        yuvData = NULL;
    }

    if (imageData) {
        av_free(imageData);
        imageData = NULL;
    }

    if (pcmData) {
        av_free(pcmData);
        pcmData = NULL;
    }

    this->clearBuffer(true);

    //videoCodec会跟着自动释放
    if (videoCodecCtx) {
        avcodec_free_context(&videoCodecCtx);
        videoCodec = NULL;
        videoCodecCtx = NULL;
    }

    //audioCodec会跟着自动释放
    if (audioCodecCtx) {
        avcodec_free_context(&audioCodecCtx);
        audioCodec = NULL;
        audioCodecCtx = NULL;
    }

    if (options) {
        av_dict_free(&options);
        options = NULL;
    }

    if (formatCtx) {
        avformat_close_input(&formatCtx);
        avformat_free_context(formatCtx);
        formatCtx = NULL;
    }

    //释放滤镜相关
    mutex.lock();
    FFmpegFilter::freeFilter(videoFilter);
    mutex.unlock();

    if (needClose) {
        VideoThread::closeVideo();
    }
}

qint64 FFmpegThread::getStartTime()
{
    return this->startTime;
}

QDateTime FFmpegThread::getLastTime()
{
    return this->lastTime;
}

FFmpegSave *FFmpegThread::getSaveFile()
{
    return this->saveFile;
}

AVStream *FFmpegThread::getVideoStream()
{
    if (videoIndex >= 0) {
        return formatCtx->streams[videoIndex];
    } else {
        return NULL;
    }
}

AVStream *FFmpegThread::getAudioStream()
{
    if (audioIndex >= 0) {
        return formatCtx->streams[audioIndex];
    } else {
        return NULL;
    }
}

bool FFmpegThread::getPushPreview()
{
    return this->pushPreview;
}

void FFmpegThread::setPushPreview(bool pushPreview)
{
    this->pushPreview = pushPreview;
}

bool FFmpegThread::getTryOpen()
{
    return this->tryOpen;
}

bool FFmpegThread::getTryRead()
{
    return this->tryRead;
}

bool FFmpegThread::getTryStop()
{
    return this->stopped;
}

void FFmpegThread::clearBuffer(bool direct)
{
    if (direct) {
        needClear = false;
        if (videoCodecCtx) {
            avcodec_flush_buffers(videoCodecCtx);
        }
        if (audioCodecCtx) {
            avcodec_flush_buffers(audioCodecCtx);
        }
    } else {
        needClear = true;
    }
}

void FFmpegThread::clearAndReset()
{
    audioSync->clear();
    videoSync->clear();
    audioSync->reset();
    videoSync->reset();
}

int FFmpegThread::getRotate()
{
    //不是默认值说明已经获取过旋转角度不用再去获取
    if (rotate != -1) {
        return rotate;
    }

    rotate = 0;
    if (videoIndex >= 0) {
        rotate = FFmpegHelper::getRotate(formatCtx->streams[videoIndex]);
        if (rotate == 90 || rotate == 270) {
            emit receiveSizeChanged();
        }
    }

    return rotate;
}

void FFmpegThread::setRotate(int rotate)
{
    if (rotate < 0) {
        return;
    }

    //只有变化了才需要触发
    bool changed = (this->rotate != rotate);
    this->rotate = rotate;
    if (isOk && rotate >= 0 && changed) {
        //旋转通过滤镜实现效果
        this->resetFilter();
        QMetaObject::invokeMethod(this, "receiveSizeChanged");
    }
}

qint64 FFmpegThread::getDuration()
{
    return duration;
}

qint64 FFmpegThread::getPosition()
{
    return position;
}

void FFmpegThread::setPosition(qint64 position)
{
    if (isOk && getIsFile()) {
        //设置切换进度标志位以便暂停解析
        changePosition = true;
        //用定时器设置有个好处避免重复频繁设置进度(频繁设置可能会崩溃)
        this->position = position;
        timerPosition->stop();
        timerPosition->start();
    }
}

void FFmpegThread::setPosition()
{
    if (!isOk) {
        return;
    }

    //清空同步线程缓存数据
    audioSync->clear();
    videoSync->clear();
    //清空解码缓存数据
    this->clearBuffer(true);

    //发过来的是毫秒而参数需要微秒(有些文件开始时间不是0所以需要加上该时间)
    //asf文件开始时间是一个很大的负数这里不需要加上
    qint64 timestamp = position * 1000;
    qint64 timestart = formatCtx->start_time;
    if (timestart > 0) {
        timestamp += timestart;
    }

    //AVSEEK_FLAG_BACKWARD=往前找最近的关键帧/AVSEEK_FLAG_ANY=直接定位到指定帧不管画面是否清晰
    av_seek_frame(formatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    //avformat_seek_file(formatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);

    //如果处于暂停阶段在等待新进度切换好以后需要再次暂停
    if (isPause) {
        timerPause->stop();
        timerPause->start();
    }

    //继续播放以便切换到当前新进度的位置
    this->next();
}

double FFmpegThread::getSpeed()
{
    return this->speed;
}

void FFmpegThread::setSpeed(double speed)
{
    //启用了音视频同步的h264裸流也需要设置
    if (isOk && this->speed != speed && (getIsFile() || (useSync && (formatName == "h264" || formatName == "hevc")))) {
        this->pause();
        this->speed = speed;
        this->next();
    }
}

int FFmpegThread::getVolume()
{
    return audioPlayer->getVolume();
}

void FFmpegThread::setVolume(int volume)
{
    audioPlayer->setVolume(volume);
}

bool FFmpegThread::getMuted()
{
    return audioPlayer->getMuted();
}

void FFmpegThread::setMuted(bool muted)
{
    audioPlayer->setMuted(muted);
}

void FFmpegThread::setAudioDevice(const QString &audioDevice)
{
    this->audioDevice = audioDevice;
    if (!audioPlayer || audioDevice.isEmpty()) {
        return;
    }

    //取出旧音量用于切换声卡后立即设置新的音量
    int volume = getVolume();
    if (videoType == VideoType_Camera) {
        this->openAudioOutput(8000, 1, 16, volume);
    } else if (audioIndex >= 0) {
        this->openAudioOutput(sampleRate, channelCount, 16, volume);
    }
}

void FFmpegThread::setCrop(bool isCrop)
{
    this->isCrop = isCrop;
    if (!isCrop) {
        cropTime = QDateTime::currentDateTime();
    }
}

void FFmpegThread::debug(int result, const QString &head, const QString &msg)
{
    if (result < 0) {
        QString text = (msg.isEmpty() ? "" : (" " + msg));
        VideoThread::debug(head, QString("错误: %1%2").arg(FFmpegHelper::getError(result)).arg(text));
    } else {
        VideoThread::debug(head, msg);
    }
}

void FFmpegThread::pause()
{
    if (this->isRunning()) {
        isPause = true;
    }
}

void FFmpegThread::next()
{
    if (this->isRunning()) {
        isPause = false;
        changePosition = false;
        //复位同步线程(不复位继续播放后会瞬间快速跳帧)
        audioSync->reset();
        videoSync->reset();
    }
}

void FFmpegThread::step(bool backward)
{
    if (getIsFile() && playStep) {
        this->next();
    }
}

void FFmpegThread::recordStart(const QString &fileName)
{
    AbstractVideoThread::recordStart(fileName);
    if (saveVideoType > 1) {
        this->setFileName(fileName);
        //处于暂停阶段则切换暂停标志位(暂停后再次恢复说明又重新开始录制)
        if (saveFile->getIsPause()) {
            isRecord = true;
            saveFile->pause();
            emit recorderStateChanged(RecorderState_Recording, fileName);
        } else {
            //根据不同的编码策略设置视频是否需要转码
            bool encodeVideo = false;
            if (encodeType == EncodeType_File) {
                if (getIsFile()) {
                    encodeVideo = true;
                }
            } else if (encodeType == EncodeType_All) {
                encodeVideo = true;
            }

            //部分厂家比如安迅士的需要重新编码保存才正常
            if (videoType == VideoType_Rtsp && videoUrl.contains("/axis-media/")) {
                encodeVideo = true;
            } else if (formatName == "mpjpeg") {
                encodeVideo = true;
            }

            saveFile->setEncode(encodeVideo, false, videoFormat, encodeVideoFps, encodeVideoRatio, encodeVideoScale);
            saveFile->setPara(videoType, saveVideoType, getVideoStream(), getAudioStream());
            saveFile->setProperty("url", videoUrl);

            //如果秘钥不为空则需要启用加密
            if (!secretKey.isEmpty()) {
                QByteArray cryptoKey = this->property("cryptoKey").toByteArray();
                saveFile->setProperty("cryptoKey", cryptoKey);
            }

            //保存裸流要区分264/265(比如保存裸流文件拓展名需要根据这个来替换)
            saveFile->setProperty("videoCodecName", videoCodecName);
            saveFile->open(fileName);
            if (saveFile->getIsOk()) {
                isRecord = true;
                emit recorderStateChanged(RecorderState_Recording, fileName);
            }
        }
    }
}

void FFmpegThread::recordPause()
{
    AbstractVideoThread::recordPause();
    if (saveVideoType > 1) {
        if (saveFile->getIsOk()) {
            isRecord = false;
            saveFile->pause();
            emit recorderStateChanged(RecorderState_Paused, fileName);
        }
    }
}

void FFmpegThread::recordStop()
{
    AbstractVideoThread::recordStop();
    if (saveVideoType > 1) {
        if (saveFile->getIsOk()) {
            isRecord = false;
            saveFile->stop();
            emit recorderStateChanged(RecorderState_Stopped, fileName);
        }
    }
}

void FFmpegThread::setOsdInfo(const QList<OsdInfo> &listOsd)
{
    this->listOsd = listOsd;
    this->resetFilter();
}

void FFmpegThread::setGraphInfo(const QList<GraphInfo> &listGraph)
{
    //硬解码或者采集桌面不支持图形滤镜
    if (hardware != "none" || videoType == VideoType_Desktop) {
        return;
    }

    this->listGraph = listGraph;
    this->resetFilter();
}
