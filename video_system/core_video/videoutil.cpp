#include "videoutil.h"
#include "videourl.h"

#ifdef audiox
#include "audiohelper.h"
#endif

#ifdef qmedia
#include "qmediahelper.h"
#endif

#ifdef ffmpeg
#include "ffmpeghelper.h"
#endif

#ifdef vlcx
#include "vlchelper.h"
#endif

#ifdef mpvx
#include "mpvhelper.h"
#endif

#ifdef mdkx
#include "mdkhelper.h"
#endif

#ifdef qtavx
#include "qtavhelper.h"
#endif

#ifdef haikang
#include "haikanghelper.h"
#endif

#ifdef easyplayer
#include "easyplayerhelper.h"
#endif

QString VideoUtil::getVersion()
{
#ifdef qmedia
    return getVersion(VideoCore_QMedia);
#elif ffmpeg
    return getVersion(VideoCore_FFmpeg);
#elif vlcx
    return getVersion(VideoCore_Vlc);
#elif mpvx
    return getVersion(VideoCore_Mpv);
#elif mdkx
    return getVersion(VideoCore_Mdk);
#elif qtavx
    return getVersion(VideoCore_Qtav);
#elif haikang
    return getVersion(VideoCore_HaiKang);
#elif easyplayer
    return getVersion(VideoCore_EasyPlayer);
#else
    return getVersion(VideoCore_None);
#endif
}

QString VideoUtil::getVersion(const VideoCore &videoCore)
{
    QString version = "none v1.0";
    if (videoCore == VideoCore_QMedia) {
#ifdef qmedia
        version = "Qt v" + QMediaHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_FFmpeg) {
#ifdef ffmpeg
        version = "ffmpeg v" + FFmpegHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_Vlc) {
#ifdef vlcx
        version = "vlc v" + VlcHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_Mpv) {
#ifdef mpvx
        version = "mpv v" + MpvHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_Mdk) {
#ifdef mdkx
        version = "mdk v" + MdkHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_Qtav) {
#ifdef qtavx
        version = "qtav v" + QtavHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_HaiKang) {
#ifdef haikang
        version = "haikang v" + HaiKangHelper::getVersion();
#endif
    } else if (videoCore == VideoCore_EasyPlayer) {
#ifdef easyplayer
        version = "easyplayer v" + EasyPlayerHelper::getVersion();
#endif
    }

    return version;
}

VideoCore VideoUtil::getVideoCore(int type)
{
    VideoCore videoCore = VideoCore_None;
    if (type == 0) {
#ifdef qmedia
        videoCore = VideoCore_QMedia;
#elif ffmpeg
        videoCore = VideoCore_FFmpeg;
#elif vlcx
        videoCore = VideoCore_Vlc;
#elif mpvx
        videoCore = VideoCore_Mpv;
#elif mdkx
        videoCore = VideoCore_Mdk;
#elif qtavx
        videoCore = VideoCore_Qtav;
#elif haikang
        videoCore = VideoCore_HaiKang;
#elif easyplayer
        videoCore = VideoCore_EasyPlayer;
#endif
    } else if (type == 1) {
#ifdef qmedia
        videoCore = VideoCore_QMedia;
#endif
    } else if (type == 2) {
#ifdef ffmpeg
        videoCore = VideoCore_FFmpeg;
#endif
    } else if (type == 3) {
#ifdef vlcx
        videoCore = VideoCore_Vlc;
#endif
    } else if (type == 4) {
#ifdef mpvx
        videoCore = VideoCore_Mpv;
#endif
    } else if (type == 5) {
#ifdef mdkx
        videoCore = VideoCore_Mdk;
#endif
    } else if (type == 6) {
#ifdef qtavx
        videoCore = VideoCore_Qtav;
#endif
    } else if (type == 10) {
#ifdef haikang
        videoCore = VideoCore_HaiKang;
#endif
    } else if (type == 20) {
#ifdef easyplayer
        videoCore = VideoCore_EasyPlayer;
#endif
    }

    return videoCore;
}

QStringList VideoUtil::getAudioDevice(const VideoCore &videoCore)
{
    QStringList names, descriptions;
    if (videoCore == VideoCore_Vlc) {
#ifdef vlcx
        VlcHelper::getAudioDevice(names, descriptions);
#endif
    } else if (videoCore == VideoCore_Mpv) {
#ifdef mpvx
        MpvHelper::getAudioDevice(names, descriptions);
#endif
    } else {
#ifdef audiox
#ifdef multimedia
        AudioHelper::getAudioDevice(descriptions, names, false);
#endif
#endif
    }
    return descriptions;
}

VideoThread *VideoUtil::newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode)
{
    VideoThread *videoThread = NULL;
    if (videoCore == VideoCore_QMedia) {
#ifdef qmedia
        videoThread = new QMediaThread(videoMode, parent);
#endif
    } else if (videoCore == VideoCore_FFmpeg) {
#ifdef ffmpeg
        videoThread = new FFmpegThread(parent);
#endif
    } else if (videoCore == VideoCore_Vlc) {
#ifdef vlcx
        videoThread = new VlcThread(parent);
#endif
    } else if (videoCore == VideoCore_Mpv) {
#ifdef mpvx
        videoThread = new MpvThread(parent);
#endif
    } else if (videoCore == VideoCore_Mdk) {
#ifdef mdkx
        videoThread = new MdkThread(parent);
#endif
    } else if (videoCore == VideoCore_Qtav) {
#ifdef qtavx
        videoThread = new QtavThread(parent);
#endif
    } else if (videoCore == VideoCore_HaiKang) {
#ifdef haikang
        videoThread = new HaiKangThread(parent);
#endif
    } else if (videoCore == VideoCore_EasyPlayer) {
#ifdef easyplayer
        videoThread = new EasyPlayerThread(parent);
#endif
    }

    //如果都没有定义则实例化基类
    if (!videoThread) {
        videoThread = new VideoThread(parent);
    }

    return videoThread;
}

void VideoUtil::renameFile(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo file(fileName);
    QString path = file.path();
    QString name = file.fileName();

    //找到目录下的所有视频文件
    QStringList filter;
    filter << "*.mp3" << "*.mp4" << "*.avi" << "*.asf" << "*.mkv" << "*.ts";
    QDir dir(path);
    QStringList files = dir.entryList(filter);
    if (files.count() == 0) {
        return;
    }

    //录制的视频文件是按照内部规则命名的(要在生成文件后对文件重命名)
    //为了产生规范的命名需要在 libvlc_new 的时候设置 meta-title
    //建议每个视频通道必须设置唯一标识不然同一个时刻保存的文件只能重命名一个
    //正确名称: video1_2021-09-26-16-15-41.mp4 ch1_2021-09-26-16-15-41.mp4
    //默认名称: vlc-record-2022-11-04-14h07m43s-2.mp4-.mp4
    //默认名称: vlc-record-2021-09-26-16h15m13s-dshow___-.avi
    //默认名称: vlc-record-2021-09-26-20h46m19s-rtsp___192.168.0.15_media_video2-.avi
    //默认名称: vlc-record-2021-09-26-20h46m16s-6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8-.mp4
    //规范名称: vlc-record-2021-09-26-16h15m13s-Ch1-.avi vlc-record-2021-09-26-16h15m13s-Ch2-.mp4

    foreach (QString file, files) {
        //已经改名的不用处理(未改名的都是vlc-record-开头)
        if (!file.startsWith("vlc-record-")) {
            continue;
        }

        //取出文件名称的时间部分
        QStringList list = file.split("-");
        if (list.count() < 7) {
            continue;
        }

        //取出指定标识(如果有 -ch -video 字样说明是指定了名称)
        QString flag = list.at(6);
        QString flag2 = flag.toLower();
        QString suffix = file.split(".").last();
        if (!flag2.startsWith("ch") && !flag2.startsWith("video")) {
            //没有指定格式的就按照日期时间命名
            name = QString("%1-%2-%3-%4").arg(list.at(2)).arg(list.at(3)).arg(list.at(4)).arg(list.at(5));
            QDateTime dateTime = QDateTime::fromString(name, "yyyy-MM-dd-h'h'm'm's's'");
            name = dateTime.toString("yyyy-MM-dd-HH-mm-ss-zzz") + "." + suffix;
        } else {
            //重新调整前缀(有时候同一时刻的会有偏差)
            QStringList names = name.split("_");
            name.replace(names.first(), flag2);
        }

        QString oldName = path + "/" + file;
        QString newName = path + "/" + name.replace("mp4", suffix);
        QFile::rename(oldName, newName);
        qDebug() << TIMEMS << QString("文件重名 -> 标识: %1 原名: %2 新名: %3").arg(flag).arg(file).arg(name);
        break;
    }
}

void VideoUtil::resetCursor()
{
    qApp->setOverrideCursor(Qt::ArrowCursor);
    qApp->restoreOverrideCursor();
    //qDebug() << TIMEMS << "resetCursor";
}

void VideoUtil::loadVideoCore(QComboBox *cbox, int &videoCore, bool simple)
{
    //冻结该控件所有信号
    cbox->blockSignals(true);
    cbox->clear();

    if (simple) {
#ifdef ffmpeg
        cbox->addItem("ffmpeg", 2);
#endif
#ifdef mdkx
        cbox->addItem("mdk", 5);
#endif
#ifdef qtavx
        cbox->addItem("qtav", 6);
#endif
    } else {
#ifdef qmedia
        cbox->addItem("qmedia", 1);
#endif
#ifdef ffmpeg
        cbox->addItem("ffmpeg", 2);
#endif
#ifdef vlcx
        cbox->addItem("vlc", 3);
#endif
#ifdef mpvx
        cbox->addItem("mpv", 4);
#endif
#ifdef mdkx
        cbox->addItem("mdk", 5);
#endif
#ifdef qtavx
        cbox->addItem("qtav", 6);
#endif
#ifdef haikang
        cbox->addItem("海康", 10);
#endif
#ifdef easyplayer
        cbox->addItem("easyplayer", 20);
#endif
    }

    //设置默认值
    int index = cbox->findData(videoCore);
    cbox->setCurrentIndex(index < 0 ? 0 : index);
    videoCore = cbox->itemData(cbox->currentIndex()).toInt();

    //取消冻结信号
    cbox->blockSignals(false);
}

void VideoUtil::loadVideoUrl(QComboBox *cbox, const QString &videoUrl, int type)
{
    //冻结该控件所有信号
    cbox->blockSignals(true);

    if (type >= 0) {
        cbox->clear();
        cbox->addItems(VideoUrl::getUrls(VideoUrl::UrlType(type)));
    }

    cbox->setCurrentIndex(cbox->findText(videoUrl));
    if (cbox->lineEdit()) {
        cbox->lineEdit()->setText(videoUrl);
        cbox->lineEdit()->setCursorPosition(0);
    }

    //取消冻结信号
    cbox->blockSignals(false);
}

void VideoUtil::loadScaleMode(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("自动调整");
    cbox->addItem("等比缩放");
    cbox->addItem("拉伸填充");
}

void VideoUtil::loadVideoMode(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("句柄");
    cbox->addItem("绘制");
    cbox->addItem("GPU");
}

void VideoUtil::loadDecodeType(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("速度优先");
    cbox->addItem("质量优先");
    cbox->addItem("均衡处理");
    cbox->addItem("最快速度");
}

void VideoUtil::loadEncodeType(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("自动处理");
    cbox->addItem("仅限文件");
    cbox->addItem("所有转码");
}

void VideoUtil::loadHardware(QComboBox *cbox, const VideoCore &videoCore, QString &hardware)
{
    //冻结该控件所有信号
    cbox->blockSignals(true);
    cbox->clear();

    QStringList list;
    list << "none";

    //ffmpeg建议用dxva2 / vlc建议用any / mpv建议用auto
    if (videoCore == VideoCore_FFmpeg) {
        //list << "qsv" << "cuvid";
    } else if (videoCore == VideoCore_Vlc) {
        list << "any";
    } else if (videoCore == VideoCore_Mpv) {
        list << "auto";
    } else if (videoCore == VideoCore_Mdk) {
#if defined(Q_OS_WIN)
        list << "qsv" << "cuda" << "mft11" << "mft9";
#endif
    } else if (videoCore == VideoCore_Qtav) {
#if defined(Q_OS_WIN)
        list << "qsv" << "cuda";
#endif
    }

    //因特尔显卡=vaapi 英伟达显卡=vdpau
    //安卓 mediacodec  树莓派 mmal  CUDA平台 nvdec cuda  瑞星微 rkmpp
#if defined(Q_OS_WIN)
    list << "dxva2" << "d3d11va";
#elif defined(Q_OS_ANDROID)
    list << "mediacodec";
#elif defined(Q_OS_LINUX)
    list << "vaapi" << "vdpau";
#elif defined(Q_OS_MAC)
    list << "videotoolbox";
#endif

    cbox->addItems(list);

    //设置默认值
    int index = list.indexOf(hardware);
    cbox->setCurrentIndex(index < 0 ? 0 : index);
    hardware = cbox->currentText();

    //取消冻结信号
    cbox->blockSignals(false);
}

void VideoUtil::loadTransport(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("auto");
    cbox->addItem("tcp");
    cbox->addItem("udp");
}

void VideoUtil::loadCaching(QComboBox *cbox, const VideoCore &videoCore, int &caching)
{
    //冻结该控件所有信号
    cbox->blockSignals(true);
    cbox->clear();

    QStringList listText, listData;
    if (videoCore == VideoCore_FFmpeg || videoCore == VideoCore_Mdk || videoCore == VideoCore_Qtav) {
        listText << "1 MB" << "4 MB" << "8 MB" << "12 MB" << "16 MB" << "20 MB" << "30 MB" << "50 MB" << "100 MB";
        listData << "1024000" << "4096000" << "8192000" << "12288000" << "16384000" << "20480000" << "30720000" << "51200000" << "102400000";
    } else {
        listText << "0.2 秒" << "0.3 秒" << "0.5 秒" << "1.0 秒" << "3.0 秒" << "5.0 秒" << "8.0 秒";
        listData << "200" << "300" << "500" << "1000" << "3000" << "5000" << "8000";
    }

    int count = listText.count();
    for (int i = 0; i < count; ++i) {
        cbox->addItem(listText.at(i), listData.at(i));
    }

    //设置默认值
    int index = listData.indexOf(QString::number(caching));
    cbox->setCurrentIndex(index < 0 ? 2 : index);
    caching = cbox->itemData(cbox->currentIndex()).toInt();

    //取消冻结信号
    cbox->blockSignals(false);
}

void VideoUtil::loadOpenSleepTime(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("1 秒", 1000);
    cbox->addItem("3 秒", 3000);
    cbox->addItem("5 秒", 5000);
}

void VideoUtil::loadReadTimeout(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("不处理", 0);
    cbox->addItem("5 秒", 5000);
    cbox->addItem("10 秒", 10000);
    cbox->addItem("15 秒", 15000);
    cbox->addItem("30 秒", 30000);
    cbox->addItem("60 秒", 60000);
}

void VideoUtil::loadConnectTimeout(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("不处理", 0);
    cbox->addItem("0.3 秒", 300);
    cbox->addItem("0.5 秒", 500);
    cbox->addItem("1.0 秒", 1000);
    cbox->addItem("3.0 秒", 3000);
    cbox->addItem("5.0 秒", 5000);
}

void VideoUtil::loadSpeed(QComboBox *cbox)
{
    cbox->clear();
    cbox->addItem("0.5x", "0.5");
    cbox->addItem("1.0x", "1.0");
    cbox->addItem("2.0x", "2.0");
    cbox->addItem("5.0x", "5.0");
    cbox->addItem("8.0x", "8.0");
    cbox->addItem("15.0x", "15.0");
    cbox->addItem("30.0x", "30.0");
}

//ffplay -ar 44100 -ac 2 -f s16le -i d:/out.pcm
void VideoUtil::writeData(QFile *file, char *data, qint64 len)
{
    if (!file->isOpen()) {
        file->open(QFile::WriteOnly);
    }

    if (file->isOpen()) {
        file->write(data, len);
    }
}
