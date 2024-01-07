#include "ffmpegsavesimple.h"

//用法示例(保存文件/推流)
#if 0
FFmpegSaveSimple *f = new FFmpegSaveSimple(this);
f->setUrl("f:/mp4/push/1.mp4", "f:/1.mp4");
f->setUrl("f:/mp4/push/1.mp4", "rtmp://127.0.0.1/stream");
f->start();
#endif

FFmpegSaveSimple::FFmpegSaveSimple(QObject *parent) : QThread(parent)
{
    stopped = false;
    audioIndex = -1;
    videoIndex = -1;

    formatCtxIn = NULL;
    formatCtxOut = NULL;

    //初始化ffmpeg的库
    FFmpegHelper::initLib();
}

FFmpegSaveSimple::~FFmpegSaveSimple()
{
    this->stop();
    this->close();
}

void FFmpegSaveSimple::run()
{
    if (!this->open()) {
        this->close();
        return;
    }

    int ret;
    AVPacket packet;
    qint64 videoCount = 0;
    qint64 startTime = av_gettime();

    while (!stopped) {
        //读取一帧
        ret = av_read_frame(formatCtxIn, &packet);
        int index = packet.stream_index;
        if (ret < 0) {
            if (ret == AVERROR_EOF || ret == AVERROR_EXIT) {
                debug(ret, "文件结束");
                break;
            } else {
                debug(ret, "读取出错");
                continue;
            }
        }

        //取出输入输出流的时间基
        AVStream *streamIn = formatCtxIn->streams[index];
        AVStream *streamOut = formatCtxOut->streams[index];
        AVRational timeBaseIn = streamIn->time_base;
        AVRational timeBaseOut = streamOut->time_base;

        if (index == videoIndex) {
            videoCount++;
        }

        //纠正有些文件比如h264格式的没有pts
        if (packet.pts == AV_NOPTS_VALUE) {
            qreal fps = av_q2d(formatCtxIn->streams[videoIndex]->r_frame_rate);
            FFmpegHelper::rescalePacket(&packet, timeBaseIn, videoCount, fps);
        }

        //延时(防止数据太快给缓存造成太大压力)
        if (index == videoIndex) {
            AVRational timeBase = {1, AV_TIME_BASE};
            int64_t ptsTime = av_rescale_q(packet.dts, timeBaseIn, timeBase);
            int64_t nowTime = av_gettime() - startTime;
            if (ptsTime > nowTime) {
                av_usleep(ptsTime - nowTime);
            }
        }

        //重新调整时间基准
        FFmpegHelper::rescalePacket(&packet, timeBaseIn, timeBaseOut);

        qDebug() << TIMEMS << "发送一帧" << videoCount << packet.flags << packet.pts << packet.dts;
        //ret = av_write_frame(formatCtxOut, &packet);
        ret = av_interleaved_write_frame(formatCtxOut, &packet);
        if (ret < 0) {
            debug(ret, "写数据包");
            break;
        }

        av_packet_unref(&packet);
    }

    //写文件尾
    av_write_trailer(formatCtxOut);
    this->close();
}

bool FFmpegSaveSimple::open()
{
    if (urlIn.isEmpty() || urlOut.isEmpty()) {
        return false;
    }

    int ret = -1;
    if ((ret = avformat_open_input(&formatCtxIn, urlIn.toUtf8().constData(), 0, NULL)) < 0) {
        debug(ret, "打开输入");
        return false;
    }
    if ((ret = avformat_find_stream_info(formatCtxIn, 0)) < 0) {
        debug(ret, "无流信息");
        return false;
    }

    videoIndex = av_find_best_stream(formatCtxIn, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        debug(ret, "无视频流");
        return false;
    }

    //既可以是保存到文件也可以是推流(对应格式要区分)
    const char *format = "mp4";
    if (urlOut.startsWith("rtmp://")) {
        format = "flv";
    } else if (urlOut.startsWith("rtsp://")) {
        format = "rtsp";
    }

    QByteArray urlData = urlOut.toUtf8();
    const char *url = urlData.constData();
    ret = avformat_alloc_output_context2(&formatCtxOut, NULL, format, url);
    if (!formatCtxOut) {
        debug(ret, "创建输出");
        return false;
    }

    //根据输入流创建输出流
    for (int i = 0; i < formatCtxIn->nb_streams; i++) {
        AVStream *streamIn = formatCtxIn->streams[i];
        AVStream *streamOut = avformat_new_stream(formatCtxOut, NULL);
        if (!streamOut) {
            return false;
        }

        //复制各种参数
        ret = FFmpegHelper::copyContext(streamIn, streamOut);
        if (ret < 0) {
            debug(ret, "复制参数");
            return false;
        }        
    }

    //打开输出
    if (!(formatCtxOut->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&formatCtxOut->pb, url, AVIO_FLAG_WRITE);
        if (ret < 0) {
            debug(ret, "打开输出");
            return false;
        }
    }

    //写文件头
    ret = avformat_write_header(formatCtxOut, NULL);
    if (ret < 0) {
        debug(ret, "写文件头");
        return false;
    }

    debug(0, QString("打开成功: %1").arg(urlIn));
    if (format != "mp4") {
        debug(0, QString("开始推流: %1").arg(urlOut));
    } else {
        debug(0, QString("开始录像: %1").arg(urlOut));
    }

    return true;
}

void FFmpegSaveSimple::close()
{
    stopped = false;
    if (formatCtxOut) {
        debug(0, QString("关闭成功: %1").arg(urlIn));
    }

    if (formatCtxIn) {
        avformat_close_input(&formatCtxIn);
        formatCtxIn = NULL;
    }

    if (formatCtxOut && !(formatCtxOut->oformat->flags & AVFMT_NOFILE)) {
        avio_close(formatCtxOut->pb);
    }

    if (formatCtxOut) {
        avformat_free_context(formatCtxOut);
        formatCtxOut = NULL;
    }
}

void FFmpegSaveSimple::debug(int ret, const QString &msg)
{
    QString text = (ret < 0 ? QString("%1 错误: %2").arg(msg).arg(FFmpegHelper::getError(ret)) : msg);
    qDebug() << TIMEMS << text;
}

void FFmpegSaveSimple::setUrl(const QString &urlIn, const QString &urlOut)
{
    this->urlIn = urlIn;
    this->urlOut = urlOut;
}

void FFmpegSaveSimple::stop()
{
    this->stopped = true;
    this->wait();
}
