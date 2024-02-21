#include "ffmpegutil.h"
#include "widgethead.h"
#include "qelapsedtimer.h"
#include "qstringlist.h"

//http://ruiy.leanote.com/post/c-使用ffmpeg
void FFmpegUtil::debugCodec()
{
    //输出所有支持的解码器名称
    QStringList listEncoderVideo, listEncoderAudio, listEncoderOther;
    QStringList listDecoderVideo, listDecoderAudio, listDecoderOther;
#if (FFMPEG_VERSION_MAJOR < 4)
    AVCodec *coder = av_codec_next(NULL);
    while (coder != NULL) {
#else
    void *opaque = NULL;
    const AVCodec *coder;
    while ((coder = av_codec_iterate(&opaque)) != NULL) {
#endif
        QString name = QString("%1").arg(coder->name);
        if (av_codec_is_encoder(coder)) {
            if (coder->type == AVMEDIA_TYPE_VIDEO) {
                listEncoderVideo << name;
            } else if (coder->type == AVMEDIA_TYPE_AUDIO) {
                listEncoderAudio << name;
            } else {
                listEncoderOther << name;
            }
        } else if (av_codec_is_decoder(coder)) {
            if (coder->type == AVMEDIA_TYPE_VIDEO) {
                listDecoderVideo << name;
            } else if (coder->type == AVMEDIA_TYPE_AUDIO) {
                listDecoderAudio << name;
            } else {
                listDecoderOther << name;
            }
        }
#if (FFMPEG_VERSION_MAJOR < 4)
        coder = coder->next;
#endif
    }

    qDebug() << TIMEMS << QString("视频编码 -> %1").arg(listEncoderVideo.join("/"));
    qDebug() << TIMEMS << QString("音频编码 -> %1").arg(listEncoderAudio.join("/"));
    qDebug() << TIMEMS << QString("其他编码 -> %1").arg(listEncoderOther.join("/"));
    qDebug() << TIMEMS << QString("视频解码 -> %1").arg(listDecoderVideo.join("/"));
    qDebug() << TIMEMS << QString("音频解码 -> %1").arg(listDecoderAudio.join("/"));
    qDebug() << TIMEMS << QString("其他解码 -> %1").arg(listDecoderOther.join("/"));

    //输出支持的协议
    QStringList listProtocolsIn, listProtocolsOut;
    struct URLProtocol *protocol = NULL;
    struct URLProtocol **protocol2 = &protocol;

    avio_enum_protocols((void **)protocol2, 0);
    while ((*protocol2)) {
        listProtocolsIn << avio_enum_protocols((void **)protocol2, 0);
    }

    protocol = NULL;
    avio_enum_protocols((void **)protocol2, 1);
    while ((*protocol2)) {
        listProtocolsOut << avio_enum_protocols((void **)protocol2, 1);
    }

    //硬解类型名称
    QStringList listHardware = getHardware();
    qDebug() << TIMEMS << QString("硬解类型 -> %1").arg(listHardware.join("/"));
    qDebug() << TIMEMS << QString("输入协议 -> %1").arg(listProtocolsIn.join("/"));
    qDebug() << TIMEMS << QString("输出协议 -> %1").arg(listProtocolsOut.join("/"));
    qDebug() << TIMEMS << QString("编译参数 -> %1").arg(avformat_configuration());
}

QStringList FFmpegUtil::getHardware()
{
    //硬解类型名称 ffmpeg -hwaccels
    QStringList listHardware;
    //其实ffmpeg2/ffmpeg3也有但是不完整一般建议3以上
#if (FFMPEG_VERSION_MAJOR > 3)
    enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
        listHardware << av_hwdevice_get_type_name(type);
    }
#endif
    return listHardware;
}

void FFmpegUtil::debugPara(AVCodecContext *codecCtx)
{
    QStringList list;
    list << QString("width: %1").arg(codecCtx->width);
    list << QString("height: %1").arg(codecCtx->height);
    list << QString("bit_rate: %1").arg(codecCtx->bit_rate);
    list << QString("level: %1").arg(codecCtx->level);
    list << QString("gop_size: %1").arg(codecCtx->gop_size);
    list << QString("max_b_frames: %1").arg(codecCtx->max_b_frames);
    list << QString("pix_fmt: %1").arg(FFmpegHelper::getPixFormatString(codecCtx->pix_fmt));
    list << QString("sample_fmt: %1").arg(FFmpegHelper::getSampleFormatString(codecCtx->sample_fmt));
    list << QString("codec_id: %1").arg(codecCtx->codec_id);
    list << QString("profile: %1").arg(codecCtx->profile);
    list << QString("qmin: %1").arg(codecCtx->qmin);
    list << QString("qmax: %1").arg(codecCtx->qmax);
    list << QString("me_range: %1").arg(codecCtx->me_range);
    list << QString("max_qdiff: %1").arg(codecCtx->max_qdiff);
    list << QString("qcompress: %1").arg(codecCtx->qcompress);
    qDebug() << TIMEMS << QString("参数信息 -> %1").arg(list.join("  "));
}

static void logCallback(void *ptr, int level, const char *fmt, va_list vl)
{
    char buf[1024];
    vsprintf(buf, fmt, vl);
    QString line = buf;
    line = line.trimmed();
    line.replace("\r", "");
    line.replace("\n", "");
    qDebug() << TIMEMS << line;
}

void FFmpegUtil::showDevice()
{
    showDevice(Device_Flag);
}

//ffmpeg -list_devices true -f dshow -i dummy
void FFmpegUtil::showDevice(const char *flag)
{
    //启用日志回调接收输出信息
    av_log_set_callback(logCallback);

    AVFormatContext *ctx = avformat_alloc_context();
    AVInputFormatx *fmt = av_find_input_format(flag);

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "list_devices", "true", 0);

    if (flag == "vfwcap") {
        avformat_open_input(&ctx, "list", fmt, NULL);
    } else if (flag == "dshow") {
        avformat_open_input(&ctx, "dummy", fmt, &opts);
    } else {
        avformat_open_input(&ctx, "", fmt, &opts);
    }

    //释放资源
    av_dict_free(&opts);
    avformat_close_input(&ctx);

    //重新设置日志回调以便恢复原样
    av_log_set_callback(av_log_default_callback);
}

void FFmpegUtil::showDeviceOption(const QString &device)
{
    showDeviceOption(Device_Flag, device);
}

void FFmpegUtil::showDeviceOption(const char *flag, const QString &device)
{
    //启用日志回调接收输出信息
    av_log_set_callback(logCallback);

    AVFormatContext *ctx = avformat_alloc_context();
    AVInputFormatx *fmt = av_find_input_format(flag);

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "list_options", "true", 0);

    QByteArray url = device.toUtf8();
    if (flag == "dshow") {
        url = QString("video=%1").arg(device).toUtf8();
    }

    avformat_open_input(&ctx, url.data(), fmt, &opts);

    //释放资源
    av_dict_free(&opts);
    avformat_close_input(&ctx);

    //重新设置日志回调以便恢复原样
    av_log_set_callback(av_log_default_callback);
}

#ifdef ffmpegdevice
QStringList FFmpegUtil::getInputDevices(bool video)
{
    FFmpegHelper::initLib();

    //测试发现从ffmpeg5开始才能获取到值
    QStringList names;
    AVDeviceInfoList *devices = NULL;
    AVInputFormatx *fmt = NULL;
    fmt = av_find_input_format(Device_Flag);
    if (fmt) {
        if (avdevice_list_input_sources(fmt, NULL, NULL, &devices) >= 0) {
            names = getDeviceNames(devices, video);
        }
    }

    return names;
}

QStringList FFmpegUtil::getDeviceNames(bool input, bool video)
{
    FFmpegHelper::initLib();

    QStringList names;
    AVDeviceInfoList *devices = NULL;
    if (input) {
        AVInputFormatx *fmt = NULL;
        do {
            names.clear();
            fmt = (video ? av_input_video_device_next(fmt) : av_input_audio_device_next(fmt));
            if (fmt) {
                if (avdevice_list_input_sources(fmt, NULL, NULL, &devices) >= 0) {
                    names = getDeviceNames(devices, video);
                }
                qDebug() << fmt->name << names;
            }
        } while (fmt);
    } else {
        AVOutputFormatx *fmt = NULL;
        do {
            names.clear();
            fmt = (video ? av_output_video_device_next(fmt) : av_output_audio_device_next(fmt));
            if (fmt) {
                if (avdevice_list_output_sinks(fmt, NULL, NULL, &devices) >= 0) {
                    names = getDeviceNames(devices, video);
                }
                qDebug() << fmt->name << names;
            }
        } while (fmt);
    }

    return names;
}

QStringList FFmpegUtil::getDeviceNames(AVDeviceInfoList *devices, bool video)
{
    QStringList names;
    int count = devices->nb_devices;
    for (int i = 0; i < count; ++i) {
        AVDeviceInfo *device = devices->devices[i];
#if (FFMPEG_VERSION_MAJOR > 4)
        if (device->nb_media_types > 0) {
            AVMediaType type = *device->media_types;
            if ((video && type != AVMEDIA_TYPE_VIDEO) || (!video && type != AVMEDIA_TYPE_AUDIO)) {
                continue;
            }
        }
#endif
        //在win上设备名传描述符/linux是设备名
#ifdef Q_OS_WIN
        names << device->device_description;
#else
        names << device->device_name;
#endif
    }

    //释放设备列表
    avdevice_free_list_devices(&devices);
    return names;
}
#endif

void FFmpegUtil::rotateFrame(int rotate, AVFrame *frameSrc, AVFrame *frameDst)
{
    int n = 0;
    int pos = 0;
    int w = frameSrc->width;
    int h = frameSrc->height;
    int hw = w / 2;
    int hh = h / 2;

    //根据不同的旋转角度拷贝yuv分量
    if (rotate == 90) {
        n = 0;
        int size = w * h;
        for (int j = 0; j < w; j++) {
            pos = size;
            for (int i = h - 1; i >= 0; i--) {
                pos -= w;
                frameDst->data[0][n++] = frameSrc->data[0][pos + j];
            }
        }

        n = 0;
        int hsize = size / 4;
        for (int j = 0; j < hw; j++) {
            pos = hsize;
            for (int i = hh - 1; i >= 0; i--) {
                pos -= hw;
                frameDst->data[1][n] = frameSrc->data[1][ pos + j];
                frameDst->data[2][n] = frameSrc->data[2][ pos + j];
                n++;
            }
        }
    } else if (rotate == 180) {
        n = 0;
        pos = w * h;
        for (int i = 0; i < h; i++) {
            pos -= w;
            for (int j = 0; j < w; j++) {
                frameDst->data[0][n++] = frameSrc->data[0][pos + j];
            }
        }

        n = 0;
        pos = w * h / 4;
        for (int i = 0; i < hh; i++) {
            pos -= hw;
            for (int j = 0; j < hw; j++) {
                frameDst->data[1][n] = frameSrc->data[1][ pos + j];
                frameDst->data[2][n] = frameSrc->data[2][ pos + j];
                n++;
            }
        }
    } else if (rotate == 270) {
        n = 0;
        for (int i = w - 1; i >= 0; i--) {
            pos = 0;
            for (int j = 0; j < h; j++) {
                frameDst->data[0][n++] = frameSrc->data[0][pos + i];
                pos += w;
            }
        }

        n = 0;
        for (int i = hw - 1; i >= 0; i--) {
            pos = 0;
            for (int j = 0; j < hh; j++) {
                frameDst->data[1][n] = frameSrc->data[1][pos + i];
                frameDst->data[2][n] = frameSrc->data[2][pos + i];
                pos += hw;
                n++;
            }
        }
    }

    //设置尺寸
    if (rotate == 90 || rotate == 270) {
        frameDst->linesize[0] = h;
        frameDst->linesize[1] = h / 2;
        frameDst->linesize[2] = h / 2;
        frameDst->width = h;
        frameDst->height = w;
    } else {
        frameDst->linesize[0] = w;
        frameDst->linesize[1] = w / 2;
        frameDst->linesize[2] = w / 2;
        frameDst->width = w;
        frameDst->height = h;
    }

    //设置其他参数
    frameDst->pts = frameSrc->pts;
    frameDst->pkt_dts = frameSrc->pkt_dts;
    frameDst->format = frameSrc->format;
    frameDst->key_frame = frameSrc->key_frame;
}

qint64 FFmpegUtil::getDuration(const QString &fileName, quint64 *useTime)
{
    qint64 duration = 0;
    QElapsedTimer timer;
    timer.start();

    //打开文件
    AVFormatContext *formatCtx = NULL;
    avformat_open_input(&formatCtx, fileName.toUtf8().constData(), NULL, NULL);
    avformat_find_stream_info(formatCtx, NULL);
    int videoIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        return duration;
    }

    AVStream *videoStream = formatCtx->streams[videoIndex];
    double fps = av_q2d(videoStream->avg_frame_rate);
    double fps2 = av_q2d(videoStream->r_frame_rate);
    int frameCount = videoStream->nb_frames;

    //没有获取到帧数则从头读取到尾部统计
    if (frameCount <= 0) {
        AVPacket pkt;
        av_init_packet(&pkt);
        while (av_read_frame(formatCtx, &pkt) >= 0) {
            if (pkt.stream_index == videoIndex) {
                frameCount++;
            }
            av_packet_unref(&pkt);
        }
    }

    //计算总时长
    duration = (double)frameCount / fps;
    QString type = (videoStream->nb_frames == 0 ? "循环计算" : "文件信息");
    //qDebug() << TIMEMS << QString("文件时长: %1s 统计用时: %2ms 统计方式: %3").arg(duration).arg(timer.elapsed()).arg(type);
    if (useTime) {
        *useTime = timer.elapsed();
    }

    avformat_close_input(&formatCtx);
    avformat_free_context(formatCtx);
    return duration;
}

QString FFmpegUtil::dataToString(char *data, qint64 len)
{
    return dataToStringList(data, len).join(",");
}

QStringList FFmpegUtil::dataToStringList(char *data, qint64 len)
{
    QStringList list;
    for (int i = 0; i < len; ++i) {
        list << QString::number(data[i]);
    }
    return list;
}
