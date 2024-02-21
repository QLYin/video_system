#include "ffmpeghelper.h"
#include "ffmpegutil.h"
#include "ffmpegsave.h"
#include "qmutex.h"
#include "qstringlist.h"

QString FFmpegHelper::getVersion()
{
    return QString("%1").arg(FFMPEG_VERSION);
}

QString FFmpegHelper::getError(int errnum)
{
    //常见错误码 https://blog.csdn.net/sz76211822/article/details/52371966
    char errbuf[1024] = { 0 };
    av_strerror(errnum, errbuf, sizeof(errbuf));
    return QString("(%1) %2").arg(errnum).arg(errbuf);
}

void FFmpegHelper::initLib()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit = false;
    if (!isInit) {
        //注册库中所有可用的文件格式和解码器
#if (FFMPEG_VERSION_MAJOR < 4)
        av_register_all();
#endif
        //注册滤镜特效库相关(色调/模糊/水平翻转/裁剪/加方框/叠加文字等功能)
#if (FFMPEG_VERSION_MAJOR < 5)
        avfilter_register_all();
#endif
        //注册所有设备相关支持
#ifdef ffmpegdevice
        avdevice_register_all();
#endif
        //初始化网络流格相关(使用网络流时必须先执行)
        avformat_network_init();

        //设置日志级别
        //如果不想看到烦人的打印信息可以设置成 AV_LOG_QUIET 表示不打印日志
        //有时候发现使用不正常比如打开了没法播放视频则需要打开日志看下报错提示
        av_log_set_level(AV_LOG_QUIET);

        isInit = true;
        qDebug() << TIMEMS << QString("初始化库 -> 类型: %1 版本: %2").arg("ffmpeg").arg(getVersion());

        //FFmpegUtil::debugCodec();
        //FFmpegUtil::showDevice();
        //FFmpegUtil::showDeviceOption("USB Video Device");
    }
}

void FFmpegHelper::initRtspFast(AVFormatContext *formatCtx)
{
    //接口内部读取的最大数据量(从源文件中读取的最大字节数)
    //默认值5000000导致这里卡很久最耗时(可以调小来加快打开速度)
    formatCtx->probesize = 50000;
    //从文件中读取的最大时长(单位为 AV_TIME_BASE units)
    formatCtx->max_analyze_duration = 5 * AV_TIME_BASE;
    //内部读取的数据包不放入缓冲区
    //formatCtx->flags |= AVFMT_FLAG_NOBUFFER;
    //设置解码错误验证过滤花屏
    //formatCtx->error_recognition |= AV_EF_EXPLODE;
}

void FFmpegHelper::initOption(AVDictionary **options, int caching, const QString &transport)
{
    //设置缓存大小(单位kb/默认32768/1024000=1MB)
    if (caching >= 1024000 && caching <= 102400000) {
        av_dict_set(options, "buffer_size", QString::number(caching).toUtf8().constData(), 0);
    }

    //设置超时断开连接时间(单位微秒/3000000表示3秒)
    av_dict_set(options, "stimeout", "3000000", 0);
    //设置最大时延(单位微秒/1000000表示1秒)
    av_dict_set(options, "max_delay", "1000000", 0);
    //自动开启线程数
    av_dict_set(options, "threads", "auto", 0);

    //通信协议采用tcp还是udp(udp优点是无连接/在网线拔掉以后十几秒钟重新插上还能继续接收/缺点是网络不好的情况下会丢包花屏)
    if (transport != "auto") {
        av_dict_set(options, "rtsp_transport", transport.toUtf8().constData(), 0);
    }

    //开启无缓存(rtmp等视频流不建议开启)
    //av_dict_set(options, "fflags", "nobuffer", 0);
    //av_dict_set(options, "fflags", "discardcorrupt", 0);

    //有些视频网站根据这个头部消息过滤不让ffmpeg访问需要模拟成其他的
    av_dict_set(options, "user_agent", "Mozilla", 0);
}

void FFmpegHelper::initOption(AVDictionary **options, const QString &bufferSize, int frameRate)
{
    //设置分辨率(约定10x10分辨率表示所有屏幕/多屏幕下会合并到一个屏幕采集)
    if (bufferSize != "0x0" && bufferSize != "10x10") {
        av_dict_set(options, "video_size", bufferSize.toUtf8().constData(), 0);
    }

    //设置帧率
    if (frameRate > 0) {
        av_dict_set(options, "framerate", QString::number(frameRate).toUtf8().constData(), 0);
    }

    //设置输入格式(具体要看设置是否支持)
    //后面改成了统一转成标准yuv420所以不用设置也没有任何影响
    //av_dict_set(options, "input_format", "mjpeg", 0);
}

void FFmpegHelper::initOption(AVDictionary **options, int offsetX, int offsetY, QString &videoUrl)
{
    //av_dict_set(options, "draw_mouse", "0", 0);
    //av_dict_set(options, "show_region", "1", 0);

#ifdef Q_OS_WIN
    //windows上通过设置offset_x/offset_y来设置偏移值
    av_dict_set(options, "offset_x", QString::number(offsetX).toUtf8().constData(), 0);
    av_dict_set(options, "offset_y", QString::number(offsetY).toUtf8().constData(), 0);
#else
    //linux系统通过设备名称带上偏移值传入
    videoUrl += QString("+%1,%2").arg(offsetX).arg(offsetY);
#endif
}

QString FFmpegHelper::getFormatString(int format, bool video)
{
    if (format < 0) {
        return "none";
    }

    QString name;
    if (video) {
        name = av_get_pix_fmt_name((AVPixelFormat)format);
    } else {
        name = av_get_sample_fmt_name((AVSampleFormat)format);
    }
    return name;
}

QString FFmpegHelper::getPixFormatString(int format)
{
    return getFormatString(format, true);
}

QString FFmpegHelper::getSampleFormatString(int format)
{
    return getFormatString(format, false);
}

qint64 FFmpegHelper::getPts(AVPacket *packet)
{
    //有些文件(比如asf文件)取不到pts需要矫正
    qint64 pts = 0;
    if (packet->dts == AV_NOPTS_VALUE && packet->pts && packet->pts != AV_NOPTS_VALUE) {
        pts = packet->pts;
    } else if (packet->dts != AV_NOPTS_VALUE) {
        pts = packet->dts;
    }
    return pts;
}

double FFmpegHelper::getPtsTime(AVFormatContext *formatCtx, AVPacket *packet)
{
    AVStream *stream = formatCtx->streams[packet->stream_index];
    qint64 pts = getPts(packet);
    //qDebug() << TIMEMS << pts << packet->pos << packet->duration;
    //double time = pts * av_q2d(stream->time_base) * 1000;
    double time = pts * 1.0 * av_q2d(stream->time_base) * AV_TIME_BASE;
    //double time = pts * 1.0 * stream->time_base.num / stream->time_base.den * AV_TIME_BASE;
    return time;
}

double FFmpegHelper::getDurationTime(AVFormatContext *formatCtx, AVPacket *packet)
{
    AVStream *stream = formatCtx->streams[packet->stream_index];
    double time = packet->duration * av_q2d(stream->time_base);
    return time;
}

qint64 FFmpegHelper::getDelayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime)
{
    AVRational time_base = formatCtx->streams[packet->stream_index]->time_base;
    AVRational time_base_q = {1, AV_TIME_BASE};//AV_TIME_BASE_Q
    qint64 pts = getPts(packet);
    qint64 pts_time = av_rescale_q(pts, time_base, time_base_q);
    qint64 now_time = av_gettime() - startTime;
    qint64 offset_time = pts_time - now_time;
    return offset_time;
}

void FFmpegHelper::delayTime(AVFormatContext *formatCtx, AVPacket *packet, qint64 startTime)
{
    qint64 offset_time = getDelayTime(formatCtx, packet, startTime);
    //qDebug() << TIMEMS << offset_time << packet->pts << packet->dts;
    if (offset_time > 0 && offset_time < 1 * 1000 * 1000) {
        av_usleep(offset_time);
    }
}

AVPixelFormat FFmpegHelper::hw_pix_fmt = AV_PIX_FMT_NONE;
AVPixelFormat FFmpegHelper::get_hw_format(AVCodecContext *ctx, const AVPixelFormat *pix_fmts)
{
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt) {
            return *p;
        }
    }

    return AV_PIX_FMT_NONE;
}

AVPixelFormat FFmpegHelper::find_fmt_by_hw_type(const AVHWDeviceType &type, const AVCodec *codec)
{
    enum AVPixelFormat fmt;
#if (FFMPEG_VERSION_MAJOR < 4)
    switch (type) {
        case AV_HWDEVICE_TYPE_VAAPI:
            fmt = AV_PIX_FMT_VAAPI;
            break;
        case AV_HWDEVICE_TYPE_DXVA2:
            fmt = AV_PIX_FMT_DXVA2_VLD;
            break;
#if (FFMPEG_VERSION_MAJOR > 2)
        case AV_HWDEVICE_TYPE_D3D11VA:
            fmt = AV_PIX_FMT_D3D11;
            break;
#endif
        case AV_HWDEVICE_TYPE_VDPAU:
            fmt = AV_PIX_FMT_VDPAU;
            break;
        case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
            fmt = AV_PIX_FMT_VIDEOTOOLBOX;
            break;
        default:
            fmt = AV_PIX_FMT_NONE;
            break;
    }
#else
    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
        if (!config) {
            break;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == type) {
            fmt = config->pix_fmt;
            break;
        }
    }
#endif
    return fmt;
}

int FFmpegHelper::decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frameSrc, AVFrame *frameDst)
{
    int result = -1;
#ifdef videoffmpeg
    QString flag = "硬解出错";
#if (FFMPEG_VERSION_MAJOR > 2)
    result = avcodec_send_packet(avctx, packet);
    if (result < 0) {
        thread->debug(result, flag, "avcodec_send_packet");
        return result;
    }

    while (result >= 0) {
        result = avcodec_receive_frame(avctx, frameSrc);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_frame");
            break;
        }

        //将数据从GPU拷贝到CPU(这一步也比较耗CPU/最佳做法就是直接底层绘制显卡数据/目前不会)
        //result = av_hwframe_map(frameDst, frameSrc, 0);
        result = av_hwframe_transfer_data(frameDst, frameSrc, 0);
        if (result < 0) {
            av_frame_unref(frameDst);
            av_frame_unref(frameSrc);
            thread->debug(result, flag, "av_hwframe_transfer_data");
            return result;
        }
        goto end;
    }
#endif
    return result;

end:
    //调用线程处理解码后的数据
    thread->decodeVideo2(packet);
#endif
    return result;
}

int FFmpegHelper::decode(FFmpegThread *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video)
{
    int result = -1;
#ifdef videoffmpeg
    QString flag = video ? "视频解码" : "音频解码";
#if (FFMPEG_VERSION_MAJOR < 3)
    if (video) {
        avcodec_decode_video2(avctx, frame, &result, packet);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_decode_video2");
            return result;
        }
    } else {
        avcodec_decode_audio4(avctx, frame, &result, packet);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_decode_audio4");
            return result;
        }
    }
    goto end;
#else
    result = avcodec_send_packet(avctx, packet);
    //有些国标ts文件会是其他几种结果但是能正常解码
    if (result < 0 && (result != AVERROR(EAGAIN)) && (result != AVERROR_EOF)) {
        //if (result < 0) {
        thread->debug(result, flag, "avcodec_send_packet");
        return result;
    }

    result = 0;
    while (result >= 0) {
        result = avcodec_receive_frame(avctx, frame);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            //thread->debug(result, flag, "avcodec_receive_frame");
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_frame");
            break;
        }
        goto end;
    }
#endif
    return result;

end:
    //调用线程处理解码后的数据
    if (video) {
        thread->decodeVideo2(packet);
    } else {
        thread->decodeAudio2(packet);
    }
#endif
    return result;
}

int FFmpegHelper::encode(FFmpegSave *thread, AVCodecContext *avctx, AVPacket *packet, AVFrame *frame, bool video)
{
    int result = -1;
    QString flag = video ? "视频编码" : "音频编码";
#if (FFMPEG_VERSION_MAJOR < 3)
    if (video) {
        avcodec_encode_video2(avctx, packet, frame, &result);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_encode_video2");
            return result;
        }
    } else {
        avcodec_encode_audio2(avctx, packet, frame, &result);
        if (result < 0) {
            thread->debug(result, flag, "avcodec_encode_audio2");
            return result;
        }
    }
    goto end;
#else
    result = avcodec_send_frame(avctx, frame);
    if (result < 0) {
        thread->debug(result, flag, "avcodec_send_frame");
        return result;
    }

    while (result >= 0) {
        result = avcodec_receive_packet(avctx, packet);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            //thread->debug(result, flag, "avcodec_receive_packet");
            break;
        } else if (result < 0) {
            thread->debug(result, flag, "avcodec_receive_packet");
            break;
        }
        goto end;
    }
#endif
    return result;

end:
    thread->writePacket2(packet, video);
    return result;
}

qreal FFmpegHelper::getFrameRate(AVStream *stream, const QString &formatName)
{
    //帧率优先取平均帧率
    double fps = av_q2d(stream->avg_frame_rate);
    double fps2 = av_q2d(stream->r_frame_rate);
    //qDebug() << TIMEMS << fps << fps2;
    if (fps <= 0 || fps > 30 || qIsNaN(fps)) {
        fps = fps2;
    }

    //有时候平均帧率是25而基本帧率是24.9127这种则取基本帧率
    if (qAbs(fps - fps2) < 5) {
        //fps = fps2;
    }

    //264/265裸流纠正下fps
    if (fps > 30 && (formatName == "h264" || formatName == "hevc")) {
        fps = 25;
    }

    return fps;
}

int FFmpegHelper::getRotate(AVStream *stream)
{
    int rotate = 0;
    //测试发现ffmpeg2不支持旋转滤镜
#if (FFMPEG_VERSION_MAJOR < 3)
    return rotate;
#endif

#if (FFMPEG_VERSION_MAJOR < 5)
    AVDictionaryEntry *tag = NULL;
    tag = av_dict_get(stream->metadata, "rotate", NULL, 0);
    if (tag) {
        rotate = atoi(tag->value);
    }
#else
    //从ffplay源码中找到的方法
    double theta = 0;
    quint8 *displaymatrix = av_stream_get_side_data(stream, AV_PKT_DATA_DISPLAYMATRIX, NULL);
    if (displaymatrix) {
        theta = -av_display_rotation_get((qint32 *) displaymatrix);
        theta -= 360 * floor(theta / 360 + 0.9 / 360);
        rotate = theta;
    }
#endif
    return rotate;
}

void FFmpegHelper::setRotate(AVStream *stream, int rotate)
{
#if (FFMPEG_VERSION_MAJOR < 5)
    av_dict_set(&stream->metadata, "rotate", QString::number(rotate).toUtf8().constData(), 0);
#else
    quint8 *sidedata = av_stream_new_side_data(stream, AV_PKT_DATA_DISPLAYMATRIX, sizeof(qint32) * 9);
    if (sidedata) {
        av_display_rotation_set((qint32 *)sidedata, rotate);
    }
#endif
}

AVCodecID FFmpegHelper::getCodecId(AVStream *stream)
{
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return AVCodecID(id);
}

QString FFmpegHelper::getCodecName(AVStream *stream)
{
    AVCodecID id = getCodecId(stream);
    return (id == AV_CODEC_ID_NONE ? "none" : avcodec_descriptor_get(id)->name);
}

qint64 FFmpegHelper::getBitRate(int width, int height)
{
    qint64 bitRate = 400;
    int size = width * height;
    if (size <= (640 * 360)) {
        bitRate = 400;
    } else if (size <= (960 * 540)) {
        bitRate = 900;
    } else if (size <= (1280 * 720)) {
        bitRate = 1500;
    } else if (size <= (1920 * 1080)) {
        bitRate = 3000;
    } else if (size <= (2560 * 1440)) {
        bitRate = 3500;
    } else if (size <= (3840 * 2160)) {
        bitRate = 6000;
    }

    return bitRate * 1000;
}

qint64 FFmpegHelper::getBitRate(AVStream *stream)
{
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return bitrate;
}

int FFmpegHelper::getFormat(AVStream *stream)
{
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return format;
}

AVMediaType FFmpegHelper::getMediaType(AVStream *stream)
{
    qint64 bitrate;
    int id, type, format, width, height, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
    return AVMediaType(type);
}

void FFmpegHelper::getResolution(AVStream *stream, int &width, int &height)
{
    qint64 bitrate;
    int id, type, format, sampleRate, channelCount, profile;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
}

void FFmpegHelper::getAudioInfo(AVStream *stream, int &sampleRate, int &channelCount, int &profile)
{
    qint64 bitrate;
    int id, type, format, width, height;
    getStreamInfo(stream, id, type, format, bitrate, width, height, sampleRate, channelCount, profile);
}

void FFmpegHelper::getStreamInfo(AVStream *stream, int &id, int &type, int &format, qint64 &bitrate, int &width, int &height, int &sampleRate, int &channelCount, int &profile)
{
#if (FFMPEG_VERSION_MAJOR < 3)
    type = stream->codec->codec_type;
    if (type == AVMEDIA_TYPE_VIDEO) {
        format = stream->codec->pix_fmt;
    } else if (type == AVMEDIA_TYPE_AUDIO) {
        format = stream->codec->sample_fmt;
    }

    id = stream->codec->codec_id;
    bitrate = stream->codec->bit_rate;
    width = stream->codec->width;
    height = stream->codec->height;
    sampleRate = stream->codec->sample_rate;
    channelCount = stream->codec->channels;
    profile = stream->codec->profile;
#else
    id = stream->codecpar->codec_id;
    type = stream->codecpar->codec_type;
    format = stream->codecpar->format;
    bitrate = stream->codecpar->bit_rate;
    width = stream->codecpar->width;
    height = stream->codecpar->height;
    sampleRate = stream->codecpar->sample_rate;
    channelCount = stream->codecpar->channels;
    profile = stream->codecpar->profile;
#endif
}

void FFmpegHelper::getTracks(AVFormatContext *formatCtx, QList<int> &audioTracks, QList<int> &videoTracks)
{
    //获取音视频轨道信息(一般有一个音频或者一个视频/ts节目文件可能有多个)
    audioTracks.clear();
    videoTracks.clear();
    int count = formatCtx->nb_streams;
    for (int i = 0; i < count; ++i) {
        AVMediaType type = FFmpegHelper::getMediaType(formatCtx->streams[i]);
        if (type == AVMEDIA_TYPE_AUDIO) {
            audioTracks << i;
        } else if (type == AVMEDIA_TYPE_VIDEO) {
            videoTracks << i;
        }
    }
}

int FFmpegHelper::copyContext(AVStream *streamIn, AVStream *streamOut)
{
    int result = -1;
    //设置 codec_tag = 0 这个很关键(不加保存的数据可能不正确)
#if (FFMPEG_VERSION_MAJOR < 3)
    result = avcodec_copy_context(streamOut->codec, streamIn->codec);
    streamOut->codec->codec_tag = 0;
#else
    result = avcodec_parameters_copy(streamOut->codecpar, streamIn->codecpar);
    streamOut->codecpar->codec_tag = 0;
#endif
    return result;
}

int FFmpegHelper::copyContext(AVCodecContext *avctx, AVStream *stream, bool from)
{
    int result = -1;
#if (FFMPEG_VERSION_MAJOR < 3)
    if (from) {
        result = avcodec_copy_context(stream->codec, avctx);
    } else {
        result = avcodec_copy_context(avctx, stream->codec);
    }
#else
    if (from) {
        result = avcodec_parameters_from_context(stream->codecpar, avctx);
    } else {
        result = avcodec_parameters_to_context(avctx, stream->codecpar);
    }
#endif
    return result;
}

void FFmpegHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, qint64 &count, qreal fps)
{
    count++;
    qreal duration = AV_TIME_BASE / fps;
    packet->pts = (count * duration) / (av_q2d(timeBaseIn) * AV_TIME_BASE);
    packet->dts = packet->pts;
    packet->duration = duration / (av_q2d(timeBaseIn) * AV_TIME_BASE);
}

void FFmpegHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut)
{
    //1. av_rescale_q里面调用的av_rescale_q_rnd
    //2. av_rescale_q_rnd里面调用的av_rescale_rnd
    //3. av_packet_rescale_ts对pts/dts/duration三者调用av_rescale_q
#if 1
    packet->pts = av_rescale_q_rnd(packet->pts, timeBaseIn, timeBaseOut, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->dts = av_rescale_q_rnd(packet->dts, timeBaseIn, timeBaseOut, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->duration = av_rescale_q(packet->duration, timeBaseIn, timeBaseOut);
#else
    av_packet_rescale_ts(packet, timeBaseIn, timeBaseOut);
#endif
    packet->pos = -1;
}

void FFmpegHelper::rescalePacket(AVPacket *packet, AVRational timeBaseIn, AVRational timeBaseOut, qint64 &duration)
{
    //qDebug() << TIMEMS << duration << packet->duration << timeBaseIn.den << timeBaseIn.num << timeBaseOut.den << timeBaseOut.num;
    packet->pts = av_rescale_q_rnd(duration, timeBaseIn, timeBaseOut, AV_ROUND_UP);
    packet->dts = packet->pts;//av_rescale_q_rnd(duration, timeBaseIn, timeBaseOut, AV_ROUND_UP);
    //packet->duration = av_rescale_q_rnd(packet->duration, timeBaseIn, timeBaseOut,AV_ROUND_UP);
    packet->pos = -1;
    duration += packet->duration;
}

AVPacket *FFmpegHelper::creatPacket(AVPacket *packet)
{
    AVPacket *pkt;
#if (FFMPEG_VERSION_MAJOR < 3)
    //旧方法(废弃使用)
    if (packet) {
        pkt = new AVPacket;
        av_init_packet(pkt);
        av_copy_packet(pkt, packet);
    } else {
        pkt = new AVPacket;
        av_new_packet(pkt, sizeof(AVPacket));
    }
#else
    //新方法(推荐使用)
    if (packet) {
        pkt = av_packet_clone(packet);
    } else {
        pkt = av_packet_alloc();
    }
#endif
    return pkt;
}

void FFmpegHelper::freeFrame(AVFrame *frame)
{
    if (frame) {
        av_frame_free(&frame);
        av_free(frame->data);
    }
}

void FFmpegHelper::freePacket(AVPacket *packet)
{
    if (packet) {
        av_packet_unref(packet);
        //测试发现linux上再次释放指针会出错
#ifndef Q_OS_WIN
        return;
#endif
#if (FFMPEG_VERSION_MAJOR < 3)
        av_freep(packet);
#else
        av_packet_free(&packet);
        av_free(packet);
#endif
    }
}

int FFmpegHelper::openAndReadCallBack(void *ctx)
{
#ifdef videoffmpeg
    FFmpegThread *thread = (FFmpegThread *)ctx;
    //先判断是否尝试停止线程(有时候不存在的地址反复打开关闭会卡主导致崩溃/多了这个判断可以立即停止)
    if (thread->getTryStop()) {
        thread->debug(0, "主动停止", "");
        return 1;
    }

    //打开超时判定和读取超时判定
    if (thread->getTryOpen()) {
        //时间差值=当前时间-开始解码的时间(单位微秒)
        qint64 offset = av_gettime() - thread->getStartTime();
        int timeout = thread->getConnectTimeout() * 1000;
        //没有设定对应值的话限定最小值
        timeout = (timeout <= 0 ? 3000000 : timeout);
        if (offset > timeout) {
            //thread->debug(0, "打开超时", QString("最大: %1 超时: %2").arg(timeout).arg(offset));
            return 1;
        }
    } else if (thread->getTryRead()) {
        //时间差值=当前时间-最后一次读取的时间(单位毫秒)
        QDateTime now = QDateTime::currentDateTime();
        qint64 offset = thread->getLastTime().msecsTo(now);
        int timeout = thread->getReadTimeout();
        //没有设定对应值的话限定最小值(rtmp值要大一点)
        timeout = (timeout <= 0 ? (thread->getVideoType() == VideoType_Rtmp ? 15000 : 3000) : timeout);
        if (offset > timeout) {
            //thread->debug(0, "读取超时", QString("最大: %1 超时: %2").arg(timeout).arg(offset));
            return 1;
        }
    }
#endif
    return 0;
}
