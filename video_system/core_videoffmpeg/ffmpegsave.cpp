#include "ffmpegsave.h"
#include "ffmpeghelper.h"
#include "widgethelper.h"

FFmpegSave::FFmpegSave(QObject *parent) : AbstractSaveThread(parent)
{
    this->reset();

    formatCtx = NULL;
    videoStreamIn = NULL;
    audioStreamIn = NULL;
    videoCodecCtx = NULL;
    audioCodecCtx = NULL;

    videoData = NULL;
    audioData = NULL;
    videoFrame = NULL;
    audioFrame = NULL;
    videoSwsCtx = NULL;
    audioSwrCtx = NULL;

    videoPacket = NULL;
    audioPacket = NULL;
}

void FFmpegSave::getMediaInfo()
{
    //获取输入视频流的信息
    if (videoStreamIn) {
        videoIndexIn = videoStreamIn->index;
        this->videoCodecName = FFmpegHelper::getCodecName(videoStreamIn);
        FFmpegHelper::getResolution(videoStreamIn, videoWidth, videoHeight);
        frameRate = FFmpegHelper::getFrameRate(videoStreamIn);
        rotate = FFmpegHelper::getRotate(videoStreamIn);
        WidgetHelper::rotateSize(rotate, videoWidth, videoHeight);
    } else {
        needVideo = false;
        videoCodecName = "none";
    }

    //获取输入音频流的信息
    if (audioStreamIn) {
        audioIndexIn = audioStreamIn->index;
        this->audioCodecName = FFmpegHelper::getCodecName(audioStreamIn);
        FFmpegHelper::getAudioInfo(audioStreamIn, sampleRate, channelCount, profile);
    } else {
        needAudio = false;
        audioCodecName = "none";
    }
}

void FFmpegSave::checkEncode()
{
    //如果是保存264则不需要音频
    if (saveVideoType == SaveVideoType_Stream) {
        needAudio = false;
    }

    //格式支持说明 https://github.com/bluenviron/mediamtx
    //推流和录制要区分判断(推流更严格/主要限定在流媒体服务器端)
    bool notSupportVideo = false;
    bool notSupportAudio = false;
    if (saveMode == SaveMode_File) {
        //不需要转码的视音频格式(经过验证后的可以自行添加)
        static QStringList vnames, anames;
        if (vnames.count() == 0) {
            vnames << "h264" << "hevc";
            //部分本地摄像头必须强制转码
            if (videoType != 3) {
                vnames << "mjpeg";
            }
        }
        if (anames.count() == 0) {
            anames << "aac" << "mp2" << "mp3" << "ac3";
        }

        notSupportVideo = !vnames.contains(videoCodecName);
        notSupportAudio = !anames.contains(audioCodecName);
    } else {
        //RTMP目前只支持264/aac RTSP支持264/265/aac/G.711
        //具体需要根据实际需求进行调整/比如新版的ffmpeg6就支持265的rtmp/部分流媒体服务程序也支持264+pcm_mulaw等
        if (saveMode == SaveMode_Rtmp) {
            if (videoCodecName != "h264") {
                notSupportVideo = true;
            }
            if (audioCodecName != "aac") {
                notSupportAudio = true;
            }

            //ffmpeg6.1开始支持hevc的rtmp推流
            //#if FFMPEG_VERSION_MAJOR > 5
            if (videoCodecName == "hevc") {
                //notSupportVideo = false;
            }
            //#endif
        } else if (saveMode == SaveMode_Rtsp) {
            if (videoCodecName != "h264" && videoCodecName != "hevc" /*&& videoCodecName != "mjpeg"*/) {
                notSupportVideo = true;
            }
            if (audioCodecName != "aac" && audioCodecName != "pcm_mulaw" && audioCodecName != "pcm_alaw") {
                notSupportAudio = true;
            }
        }

        //特定格式过滤
        if (this->property("url").toString().endsWith(".m3u8")) {
            notSupportAudio = true;
        }
    }

    if (notSupportVideo) {
        debug(0, "格式检查", QString("警告: %1").arg(videoCodecName));
        encodeVideo = true;
    }

    if (notSupportAudio) {
        debug(0, "格式检查", QString("警告: %1").arg(audioCodecName));
        //encodeAudio = true;
        needAudio = false;
    }

    //可以选择只存储视频或者音频
    //this->needVideo = false;
    //this->needAudio = false;

    //索引不存在不支持编码(参数=0是为了方便打印线程那边依赖这个变量判断)
    if (!needVideo) {
        videoWidth = 0;
        encodeVideo = false;
    }
    if (!needAudio) {
        sampleRate = 0;
        encodeAudio = false;
    }

    //启用视频编码下需要纠正下帧率
    if (encodeVideo) {
        this->frameRate = frameRate > 30 ? 30 : frameRate;
    }

    //如果设置过帧率则最终按照设置的
    if (encodeVideoFps > 0) {
        this->frameRate = encodeVideoFps;
    }

    //打印编码信息
    QString msg1 = QString("%1/%2/%3").arg(needVideo ? "启用" : "禁用").arg(encodeVideo ? "编码" : "原始").arg(videoCodecName);
    QString msg2 = QString("%1/%2/%3").arg(needAudio ? "启用" : "禁用").arg(encodeAudio ? "编码" : "原始").arg(audioCodecName);
    debug(0, "编码信息", QString("视频: %1 音频: %2").arg(msg1).arg(msg2));
}

bool FFmpegSave::initVideoCtx()
{
    //没启用视频编码或者不需要视频则不继续
    if (!encodeVideo || !needVideo) {
        return true;
    }

    //查找视频编码器(自动处理的话如果源头是H265则采用HEVC作为编码器)
    AVCodecx *videoCodec;
    if (videoFormat == 0) {
        AVCodecID codecID = FFmpegHelper::getCodecId(videoStreamIn);
        if (codecID == AV_CODEC_ID_HEVC) {
            videoCodec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
        } else {
            videoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
        }
    } else if (videoFormat == 1) {
        videoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    } else if (videoFormat == 2) {
        videoCodec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    }

    //RTMP流媒体目前只支持H264
    if (fileName.startsWith("rtmp://")) {
        //#if FFMPEG_VERSION_MAJOR < 6
        videoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
        //#endif
    }

    if (!videoCodec) {
        debug(0, "视频编码", "avcodec_find_encoder");
        return false;
    }

    //创建视频编码器上下文
    videoCodecCtx = avcodec_alloc_context3(videoCodec);
    if (!videoCodecCtx) {
        debug(0, "视频编码", "avcodec_alloc_context3");
        return false;
    }

    //AVCodecContext结构体参数: https://blog.csdn.net/weixin_44517656/article/details/109707539
    //放大系数是为了小数位能够正确放大到整型
    int ratio = 1000000;
    videoCodecCtx->time_base.num = 1 * ratio;
    videoCodecCtx->time_base.den = frameRate * ratio;
    videoCodecCtx->framerate.num = frameRate * ratio;
    videoCodecCtx->framerate.den = 1 * ratio;

    //下面这种方式对编译器有版本要求(c++11)
    //videoCodecCtx->time_base = {1, frameRate};
    //videoCodecCtx->framerate = {frameRate, 1};

    //参数说明 https://blog.csdn.net/qq_40179458/article/details/110449653
    //大分辨率需要加上下面几个参数设置(否则在32位的库不能正常编码提示 Generic error in an external library)
    if ((videoWidth >= 3840 || videoHeight >= 2160)) {
        videoCodecCtx->qmin = 10;
        videoCodecCtx->qmax = 51;
        videoCodecCtx->me_range = 16;
        videoCodecCtx->max_qdiff = 4;
        videoCodecCtx->qcompress = 0.6;
    }

    //需要转换尺寸的启用目标尺寸
    int width = videoWidth;
    int height = videoHeight;
    if (encodeVideoScale != "1") {
        QStringList sizes = WidgetHelper::getSizes(encodeVideoScale);
        if (sizes.count() == 2) {
            width = sizes.at(0).toInt();
            height = sizes.at(1).toInt();
        } else {
            float scale = encodeVideoScale.toFloat();
            width = videoWidth * scale;
            height = videoHeight * scale;
        }
    }

    //初始化视频编码器参数(如果要文件体积小一些画质差一些可以降低码率)
    videoCodecCtx->bit_rate = FFmpegHelper::getBitRate(width, height) * encodeVideoRatio;
    videoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    videoCodecCtx->width = width;
    videoCodecCtx->height = height;
    videoCodecCtx->level = 50;
    //多少帧一个I帧(关键帧)
    videoCodecCtx->gop_size = frameRate;
    //去掉B帧只留下I帧和P帧
    videoCodecCtx->max_b_frames = 0;
    //videoCodecCtx->bit_rate_tolerance = 1;
    videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    videoCodecCtx->profile = FF_PROFILE_H264_MAIN;
    if (saveVideoType == SaveVideoType_Mp4) {
        videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        //videoCodecCtx->flags |= (AV_CODEC_FLAG_GLOBAL_HEADER | AV_CODEC_FLAG_LOW_DELAY);
    }

    //加速选项 https://www.jianshu.com/p/034f5b3e7f94
    //加载预设 https://blog.csdn.net/JineD/article/details/125304570
    //速度选项 ultrafast/superfast/veryfast/faster/fast/medium/slow/slower/veryslow/placebo
    //视觉优化 film/animation/grain/stillimage/psnr/ssim/fastdecode/zerolatency

    //设置零延迟(本地采集设备视频流保存如果不设置则播放的时候会越来越模糊)
    //测试发现有些文件需要开启才不会慢慢变模糊/有些开启后在部分系统环境会偶尔卡顿(webrtc下)/根据实际需求决定是否开启
    av_opt_set(videoCodecCtx->priv_data, "tune", "zerolatency", 0);

    //文件类型除外(保证文件的清晰度)
    if (videoType > 2) {
        av_opt_set(videoCodecCtx->priv_data, "preset", "ultrafast", 0);
        //av_opt_set(videoCodecCtx->priv_data, "x265-params", "qp=20", 0);
    }

    //打开视频编码器
    int result = avcodec_open2(videoCodecCtx, videoCodec, NULL);
    if (result < 0) {
        debug(result, "视频编码", "avcodec_open2");
        return false;
    }

    //创建编码用临时包
    videoPacket = FFmpegHelper::creatPacket(NULL);

    //设置了视频缩放则转换
    if (encodeVideoScale != "1") {
        videoFrame = av_frame_alloc();
        videoFrame->format = AV_PIX_FMT_YUV420P;
        videoFrame->width = width;
        videoFrame->height = height;

        int align = 1;
        int flags = SWS_BICUBIC;
        AVPixelFormat format = AV_PIX_FMT_YUV420P;
        int videoSize = av_image_get_buffer_size(format, width, height, align);
        videoData = (quint8 *)av_malloc(videoSize * sizeof(quint8));
        av_image_fill_arrays(videoFrame->data, videoFrame->linesize, videoData, format, width, height, align);
        videoSwsCtx = sws_getContext(videoWidth, videoHeight, format, width, height, format, flags, NULL, NULL, NULL);
    }

    debug(0, "视频编码", QString("编码: %1").arg(videoCodec->name));
    return true;
}

//https://blog.csdn.net/irainsa/article/details/129289254
bool FFmpegSave::initAudioCtx()
{
    //没启用音频编码或者不需要音频则不继续
    if (!encodeAudio || !needAudio) {
        return true;
    }

    AVCodecx *audioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!audioCodec) {
        debug(0, "音频编码", "avcodec_find_encoder");
        return false;
    }

    //创建音频编码器上下文
    audioCodecCtx = avcodec_alloc_context3(audioCodec);
    if (!audioCodecCtx) {
        debug(0, "音频编码", "avcodec_alloc_context3");
        return false;
    }

    //初始化音频编码器参数
    audioCodecCtx->bit_rate = FFmpegHelper::getBitRate(audioStreamIn);
    audioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    audioCodecCtx->sample_rate = sampleRate;
    audioCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    audioCodecCtx->channels = channelCount;
    //audioCodecCtx->profile = FF_PROFILE_AAC_MAIN;
    audioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audioCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    if (saveVideoType == SaveVideoType_Mp4) {
        audioCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    //打开音频编码器
    int result = avcodec_open2(audioCodecCtx, audioCodec, NULL);
    if (result < 0) {
        debug(result, "音频编码", "avcodec_open2");
        return false;
    }

    //创建编码用临时包
    audioPacket = FFmpegHelper::creatPacket(NULL);
    debug(0, "音频编码", QString("编码: %1").arg(audioCodec->name));
    return true;
}

bool FFmpegSave::initStream()
{
    AVDictionary *options = NULL;
    QByteArray fileData = fileName.toUtf8();
    const char *url = fileData.data();

    //既可以是保存到文件也可以是推流(对应格式要区分)
    const char *format = "mp4";
    if (videoIndexIn < 0 && audioCodecName == "mp3") {
        format = "mp3";
    }
    if (fileName.startsWith("rtmp://")) {
        format = "flv";
    } else if (fileName.startsWith("rtsp://")) {
        format = "rtsp";
        av_dict_set(&options, "stimeout", "3000000", 0);
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
    } else if (fileName.startsWith("udp://")) {
        format = "mpegts";
    }

    //如果存在秘钥则启用加密
    QByteArray cryptoKey = this->property("cryptoKey").toByteArray();
    if (!cryptoKey.isEmpty()) {
        av_dict_set(&options, "encryption_scheme", "cenc-aes-ctr", 0);
        av_dict_set(&options, "encryption_key", cryptoKey.constData(), 0);
        av_dict_set(&options, "encryption_kid", cryptoKey.constData(), 0);
    }

    //开辟一个格式上下文用来处理视频流输出(末尾url不填则rtsp推流失败)
    int result = avformat_alloc_output_context2(&formatCtx, NULL, format, url);
    if (result < 0) {
        debug(result, "创建格式", "");
        return false;
    }

    //创建输出视频流
    if (!this->initVideoStream()) {
        goto end;
    }

    //创建输出音频流
    if (!this->initAudioStream()) {
        goto end;
    }

    //打开输出文件
    if (!(formatCtx->oformat->flags & AVFMT_NOFILE)) {
        result = avio_open(&formatCtx->pb, url, AVIO_FLAG_WRITE);
        if (result < 0) {
            debug(result, "打开输出", "");
            goto end;
        }
    }

    //写入文件开始符
    result = avformat_write_header(formatCtx, &options);
    if (result < 0) {
        debug(result, "写文件头", "");
        goto end;
    }

    return true;

end:
    //关闭释放并清理文件
    this->close();
    this->deleteFile(fileName);
    return false;
}

bool FFmpegSave::initVideoStream()
{
    if (needVideo) {
        videoIndexOut = 0;
        AVStream *stream = avformat_new_stream(formatCtx, NULL);
        if (!stream) {
            return false;
        }

        //设置旋转角度(没有编码的数据是源头带有旋转角度的/编码后的是正常旋转好的)
        if (!encodeVideo) {
            FFmpegHelper::setRotate(stream, rotate);
        }

        //复制解码器上下文参数(不编码从源头流拷贝/编码从设置的编码器拷贝)
        int result = -1;
        if (encodeVideo) {
            stream->r_frame_rate = videoCodecCtx->framerate;
            result = FFmpegHelper::copyContext(videoCodecCtx, stream, true);
        } else {
            result = FFmpegHelper::copyContext(videoStreamIn, stream);
        }

        if (result < 0) {
            debug(result, "复制参数", "");
            return false;
        }
    }

    return true;
}

bool FFmpegSave::initAudioStream()
{
    if (needAudio) {
        audioIndexOut = (videoIndexOut == 0 ? 1 : 0);
        AVStream *stream = avformat_new_stream(formatCtx, NULL);
        if (!stream) {
            return false;
        }

        //复制解码器上下文参数(不编码从源头流拷贝/编码从设置的编码器拷贝)
        int result = -1;
        if (encodeAudio) {
            result = FFmpegHelper::copyContext(audioCodecCtx, stream, true);
        } else {
            result = FFmpegHelper::copyContext(audioStreamIn, stream);
        }

        if (result < 0) {
            debug(result, "复制参数", "");
            return false;
        }
    }

    return true;
}

bool FFmpegSave::init()
{
    //必须存在输入视音频流对象其中一个
    if (fileName.isEmpty() || (!videoStreamIn && !audioStreamIn)) {
        return false;
    }

    //检查推流地址是否正常
    if (saveMode != SaveMode_File && !WidgetHelper::checkUrl(fileName, 1000)) {
        debug(0, "地址不通", "");
        if (!this->isRunning()) {
            this->start();
        }
        return false;
    }

    //获取媒体信息及检查编码处理
    this->getMediaInfo();
    this->checkEncode();

    //ffmpeg2不支持重新编码的推流
#if (FFMPEG_VERSION_MAJOR < 3)
    if (saveMode != SaveMode_File && (encodeVideo || encodeAudio)) {
        return false;
    }
#endif

    //初始化对应视音频编码器
    if (!this->initVideoCtx()) {
        return false;
    }
    if (!this->initAudioCtx()) {
        return false;
    }

    //保存264数据直接写文件
    if (saveVideoType == SaveVideoType_Stream) {
        return true;
    }

    //初始化视音频流
    if (!this->initStream()) {
        return false;
    }

    debug(0, "索引信息", QString("视频: %1/%2 音频: %3/%4").arg(videoIndexIn).arg(videoIndexOut).arg(audioIndexIn).arg(audioIndexOut));
    return true;
}

void FFmpegSave::save()
{
    //从队列中取出数据处理
    //qDebug() << TIMEMS << packets.count() << frames.count();

    if (packets.count() > 0) {
        mutex.lock();
        AVPacket *packet = packets.takeFirst();
        mutex.unlock();

        this->writePacket2(packet, packet->stream_index == videoIndexIn);
        FFmpegHelper::freePacket(packet);
    }

    if (frames.count() > 0) {
        mutex.lock();
        AVFrame *frame = frames.takeFirst();
        mutex.unlock();

        if (frame->width > 0) {
            FFmpegHelper::encode(this, videoCodecCtx, videoPacket, frame, true);
        } else {
            FFmpegHelper::encode(this, audioCodecCtx, audioPacket, frame, false);
        }
        FFmpegHelper::freeFrame(frame);
    }
}

void FFmpegSave::close()
{
    //写入过开始符才能写入文件结束符(没有这个判断会报错)
    if ((audioDuration > 0 || videoCount > 0) && saveVideoType == SaveVideoType_Mp4 && formatCtx) {
        av_write_trailer(formatCtx);
    }

    //释放视频临时数据包
    if (videoPacket) {
        FFmpegHelper::freePacket(videoPacket);
        videoPacket = NULL;
    }

    //释放音频临时数据包
    if (audioPacket) {
        FFmpegHelper::freePacket(audioPacket);
        audioPacket = NULL;
    }

    //释放队列中的数据包
    foreach (AVPacket *packet, packets) {
        FFmpegHelper::freePacket(packet);
    }

    //释放队列中的数据帧
    foreach (AVFrame *frame, frames) {
        FFmpegHelper::freeFrame(frame);
    }

    //关闭文件流并释放对象
    if (formatCtx) {
        avio_close(formatCtx->pb);
        avformat_free_context(formatCtx);
        formatCtx = NULL;
    }

    //关闭视频编码器上下文并释放对象
    if (videoCodecCtx) {
        avcodec_free_context(&videoCodecCtx);
        videoCodecCtx = NULL;
    }

    //关闭视频编码器上下文并释放对象
    if (audioCodecCtx) {
        avcodec_free_context(&audioCodecCtx);
        audioCodecCtx = NULL;
    }

    //释放转换用的视频相关
    if (videoData) {
        av_free(videoData);
        FFmpegHelper::freeFrame(videoFrame);
        sws_freeContext(videoSwsCtx);
        videoData = NULL;
        videoFrame = NULL;
        videoSwsCtx = NULL;
    }

    //释放转换用的音频相关
    if (audioData) {
        av_free(audioData);
        FFmpegHelper::freeFrame(audioFrame);
        swr_free(&audioSwrCtx);
        audioData = NULL;
        audioFrame = NULL;
        audioSwrCtx = NULL;
    }

    //清空队列并复位
    packets.clear();
    frames.clear();
    this->reset();
    emit receiveSaveFinsh();
}

void FFmpegSave::reset()
{
    errorCount = 0;
    encodeVideo = false;
    encodeAudio = false;
    encodeVideoRatio = 1;
    encodeVideoScale = "1";

    sendPacket = false;
    onlySendPacket = false;

    needVideo = true;
    needAudio = true;
    videoIndexIn = -1;
    audioIndexIn = -1;
    videoIndexOut = -1;
    audioIndexOut = -1;

    keyFrame = false;
    videoCount = 0;
    audioDuration = 0;
}

int FFmpegSave::getVideoIndexIn()
{
    return this->videoIndexIn;
}

int FFmpegSave::getAudioIndexIn()
{
    return this->audioIndexIn;
}

int FFmpegSave::getVideoIndexOut()
{
    return this->videoIndexOut;
}

int FFmpegSave::getAudioIndexOut()
{
    return this->audioIndexOut;
}

AVStream *FFmpegSave::getVideoStream()
{
    if (videoIndexOut >= 0) {
        return formatCtx->streams[videoIndexOut];
    } else {
        return NULL;
    }
}

AVStream *FFmpegSave::getAudioStream()
{
    if (audioIndexOut >= 0) {
        return formatCtx->streams[audioIndexOut];
    } else {
        return NULL;
    }
}

void FFmpegSave::debug(int result, const QString &head, const QString &msg)
{
    if (result < 0) {
        QString text = (msg.isEmpty() ? "" : (" " + msg));
        AbstractSaveThread::debug(head, QString("错误: %1%2").arg(FFmpegHelper::getError(result)).arg(text));
    } else {
        AbstractSaveThread::debug(head, msg);
    }
}

bool FFmpegSave::getEncodeVideo()
{
    return this->encodeVideo;
}

bool FFmpegSave::getEncodeAudio()
{
    return this->encodeAudio;
}

void FFmpegSave::setEncode(bool encodeVideo, bool encodeAudio, int videoFormat, int encodeVideoFps, float encodeVideoRatio, const QString &encodeVideoScale)
{
    this->encodeVideo = encodeVideo;
    this->encodeAudio = encodeAudio;
    this->videoFormat = videoFormat;
    this->encodeVideoFps = encodeVideoFps;
    this->encodeVideoRatio = encodeVideoRatio;
    this->encodeVideoScale = encodeVideoScale;

    //启用了缩放或者设置了尺寸则启用视频编码
    if (encodeVideoScale != "1") {
        this->encodeVideo = true;
    }
}

void FFmpegSave::setSendPacket(bool sendPacket, bool onlySendPacket)
{
    this->sendPacket = sendPacket;
    this->onlySendPacket = onlySendPacket;
}

void FFmpegSave::setPara(int videoType, const SaveVideoType &saveVideoType, AVStream *videoStreamIn, AVStream *audioStreamIn)
{
    this->videoType = videoType;
    this->saveVideoType = saveVideoType;
    this->videoStreamIn = videoStreamIn;
    this->audioStreamIn = audioStreamIn;
}

void FFmpegSave::writePacket2(AVPacket *packet)
{
    //非音视频流不用处理
    int index = packet->stream_index;
    if (index != videoIndexOut && index != audioIndexOut) {
        return;
    }

    //转发数据包(可以设置仅仅转发数据包不用继续)
    if (sendPacket) {
        emit receivePacket(FFmpegHelper::creatPacket(packet));
        if (onlySendPacket) {
            return;
        }
    }

    //编码加入SPS/PPS https://blog.csdn.net/weixin_44520287/article/details/113435440
    if (saveVideoType == SaveVideoType_Stream) {
        //只需要写入视频数据
        if (index == videoIndexOut) {
            file.write((char *)packet->data, packet->size);
        }
    } else if (saveVideoType == SaveVideoType_Mp4) {
        //取出输入输出流的时间基
        AVStream *streamIn = (index == videoIndexOut ? videoStreamIn : audioStreamIn);
        AVStream *streamOut = formatCtx->streams[index];
        AVRational timeBaseIn = streamIn->time_base;
        AVRational timeBaseOut = streamOut->time_base;

        //转换时间基准
        if (index == videoIndexOut) {
            FFmpegHelper::rescalePacket(packet, timeBaseIn, videoCount, frameRate);
            FFmpegHelper::rescalePacket(packet, timeBaseIn, timeBaseOut);
        } else if (index == audioIndexOut) {
            FFmpegHelper::rescalePacket(packet, timeBaseIn, timeBaseOut, audioDuration);
        }
#if 0
        if (index == videoIndexOut) {
            qDebug() << TIMEMS << flag << index << packet->pts << packet->dts << packet->duration;
        }
#endif
        //写入一帧数据
        //int result = av_write_frame(formatCtx, packet);
        int result = av_interleaved_write_frame(formatCtx, packet);
        if (result < 0) {
            errorCount++;
            debug(result, QString("写%1包").arg(index == audioIndexOut ? "音频" : "视频"), "");
        } else {
            errorCount = 0;
        }

        //推流超过错误次数需要重连
        if (errorCount >= 5 && saveMode != SaveMode_File) {
            isOk = false;
            errorCount = 0;
            emit receiveSaveError(VideoError_Save);
        }
    }
}

void FFmpegSave::writePacket2(AVPacket *packet, bool video)
{
    //编码后的数据包需要手动设置对应流索引
    packet->stream_index = (video ? videoIndexOut : audioIndexOut);
    this->writePacket2(packet);
}

bool FFmpegSave::checkIndex(int index)
{
    //对应流没启用或者索引非法则不继续
    if (index == videoIndexIn) {
        if (!needVideo) {
            return false;
        }
    } else if (index == audioIndexIn) {
        if (!needAudio) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void FFmpegSave::writePacket(AVPacket *packet, int index)
{
    //没打开不处理
    if (!isOk) {
        return;
    }

    //暂停期间不处理并且将关键帧标志位复位(继续录制后重新以关键帧开头保证画面流畅/没有这个处理可能暂停后继续播放中间图像会有花块)
    if (isPause) {
        keyFrame = false;
        return;
    }

    //校验索引
    if (!this->checkIndex(index)) {
        return;
    }

    //只有音频以及视频转码音频不转码情况下不需要判断关键帧
    if ((videoIndexIn < 0 && audioIndexIn >= 0) || (encodeVideo && !encodeAudio)) {
        keyFrame = true;
    }

    //保证收到视频关键帧以后再开始写入(如果首帧不是关键帧部分播放器比如vlc播放器打开可能是绿屏)
    if (!keyFrame && index == videoIndexIn) {
        keyFrame = (packet->flags & AV_PKT_FLAG_KEY);
    }
    if (!keyFrame) {
        return;
    }

    //可以直接写入到文件也可以排队处理(直接写入会改变原有的包的各种属性参数)
    //this->writePacket2(packet, packet->stream_index == videoIndexIn);
    mutex.lock();
    packets << FFmpegHelper::creatPacket(packet);
    mutex.unlock();
}

void FFmpegSave::writeFrame(AVFrame *frame, int index)
{
    //没打开或者暂停阶段不处理
    if (!isOk || isPause) {
        return;
    }

    //校验索引
    if (!this->checkIndex(index)) {
        return;
    }

    //只有音频以及视频编码的情况下不需要判断关键帧
    if ((videoIndexIn < 0 && audioIndexIn >= 0) || encodeVideo) {
        keyFrame = true;
    }
    if (!keyFrame) {
        return;
    }

    mutex.lock();
    //启用了转换则用先执行转换
    if (videoSwsCtx) {
        int result = sws_scale(videoSwsCtx, (const quint8 **)frame->data, frame->linesize, 0, videoHeight, videoFrame->data, videoFrame->linesize);
        if (result >= 0) {
            frames << av_frame_clone(videoFrame);
        }
    } else {
        frames << av_frame_clone(frame);
    }
    mutex.unlock();
}
