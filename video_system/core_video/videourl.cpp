#include "videourl.h"
#include "qfile.h"
#include "qdebug.h"
#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

void VideoUrl::readUrls(const QString &fileName, QStringList &urls)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream in(&file);
    int count = urls.count();
    while (!in.atEnd()) {
        //每次读取一行
        QString line = in.readLine();
        //去除空格回车换行
        line = line.trimmed();
        line.replace("\r", "");
        line.replace("\n", "");
        //空行或者注释行不用处理
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        //格式: 0,rtsp://192.168.1.200
        QStringList list = line.split(",");
        if (list.count() != 2) {
            continue;
        }

        int channel = list.at(0).toInt();
        //替换成新的url
        if (channel >= 0 && channel < count) {
            urls[channel] = list.at(1);
        }
    }

    file.close();
}

void VideoUrl::writeUrls(const QString &fileName, const QStringList &urls)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QTextStream out(&file);
    int count = urls.count();
    for (int i = 0; i < count; ++i) {
        QString url = urls.at(i);
        if (!url.isEmpty()) {
            out << QString("%1,%2\n").arg(i).arg(url);
        }
    }

    file.close();
}

QStringList VideoUrl::getUrls(int type)
{
    QStringList urls;

    //安卓上暂时就放几个测试就好
#ifdef Q_OS_ANDROID
    appendUrl(urls, "http://222.175.159.226:808/hls/20231/index.m3u8");
    appendUrl(urls, "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4");
    appendUrl(urls, "http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4");
    appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");
    appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear1/prog_index.m3u8");
    appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101?transportmode=unicast&profile=Profile_2");
    appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
    return urls;
#endif

    if (type & 0x01) {
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video1");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video2");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101?transportmode=unicast&profile=Profile_2");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/102?transportmode=unicast&profile=Profile_2");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=1&unicast=true&proto=Onvif");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.160:554/stream0?username=admin&password=E10ADC3949BA59ABBE56E057F20F883E");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.160:554/stream1?username=admin&password=E10ADC3949BA59ABBE56E057F20F883E");
    }

    //全网最全音视频媒体流 https://blog.csdn.net/weixin_42182599/article/details/126054178
    if (type & 0x02) {
        appendUrl(urls, "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4");
        appendUrl(urls, "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4");
        appendUrl(urls, "http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4");
        appendUrl(urls, "http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4");
        appendUrl(urls, "http://vfx.mtime.cn/Video/2022/12/17/mp4/221217153424902164.mp4");

        appendUrl(urls, "http://vjs.zencdn.net/v/oceans.mp4");
        appendUrl(urls, "http://media.w3.org/2010/05/sintel/trailer.mp4");
        appendUrl(urls, "http://sdk-release.qnsdk.com/VID_20220207_144828.mp4");
        appendUrl(urls, "http://sf1-hscdn-tos.pstatp.com/obj/media-fe/xgplayer_doc_video/flv/xgplayer-demo-360p.flv");
        appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear1/prog_index.m3u8");
        appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8");

        appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");
        appendUrl(urls, "http://vd2.bdstatic.com/mda-mhhf5mr00yyhpfjs/1080p/cae_h264/1629284581057661229/mda-mhhf5mr00yyhpfjs.mp4");
        appendUrl(urls, "http://vd2.bdstatic.com/mda-mhig1c3sw223mx8p/1080p/cae_h264/1629380139191731149/mda-mhig1c3sw223mx8p.mp4");
        appendUrl(urls, "http://vd3.bdstatic.com/mda-mekfm7wu6f4rtt99/1080p/cae_h264/1621595318412084671/mda-mekfm7wu6f4rtt99.mp4");
        appendUrl(urls, "http://vd2.bdstatic.com/mda-mesdm11tdp69a9ye/1080p/cae_h264/1622115758783335310/mda-mesdm11tdp69a9ye.mp4");
    }

    if (type & 0x04) {
        //格式 搜索到的 https://music.163.com/#/song?id=179768
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=179768.mp3");
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=5238772.mp3");
        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=447925558.mp3");
    }

    if (type & 0x08) {
#ifdef Q_OS_WIN
        appendUrl(urls, "f:/mp4/push/1.mp4");
        appendUrl(urls, "f:/mp4/other/1.mkv");
        appendUrl(urls, "f:/mp4/other/1.wmv");
        appendUrl(urls, "f:/mp4/other/1.mov");
        appendUrl(urls, "f:/mp4/video/1.mp4");
        appendUrl(urls, "f:/mp4/rotate/90.mp4");
        appendUrl(urls, "f:/mp4/rotate/180.mp4");
        appendUrl(urls, "f:/mp4/other/avi0.avi");
        appendUrl(urls, "f:/mp4/ts/1.ts");
        appendUrl(urls, "f:/mp4/asf/1.asf");
        appendUrl(urls, "f:/mp4/high/1.mp4");
        appendUrl(urls, "f:/mp4/high/4k.mp4");
        appendUrl(urls, "f:/mp4/push/record2651.mp4");
        appendUrl(urls, "f:/mp4/264265/test.h264");
        appendUrl(urls, "f:/mp4/新建文件夹/1.mp4");
#endif
    }

    if (type & 0x10) {
#ifdef Q_OS_WIN
        appendUrl(urls, "f:/mp3/1.mp3");
        appendUrl(urls, "f:/mp3/1.wav");
        appendUrl(urls, "f:/mp3/1.wma");
        appendUrl(urls, "f:/mp3/1.mid");
#endif
    }

    if (type & 0x20) {
#ifdef Q_OS_WIN
        appendUrl(urls, "video=OBS Virtual Camera");
        appendUrl(urls, "video=USB Video Device|1280x720|25");
#else
        appendUrl(urls, "/dev/video0");
        appendUrl(urls, "/dev/video1");
#endif
    }

    if (type & 0x40) {
#if defined(Q_OS_WIN)
        appendUrl(urls, "f:/mp3/1.mp3");
        appendUrl(urls, "f:/mp4/ts/1.ts");
        appendUrl(urls, "f:/mp4/other/1.mkv");
        appendUrl(urls, "f:/mp4/video/1.mp4");
        appendUrl(urls, "f:/mp4/push/16.mp4");
        appendUrl(urls, "f:/mp4/high/1.mp4");
        appendUrl(urls, "f:/mp4/rotate/90.mp4");
        appendUrl(urls, "f:/mp4/push/record2651.mp4");
        appendUrl(urls, "f:/mp4/264265/test.h264");
        appendUrl(urls, "f:/mp4/新建文件夹/1.mp4");
        appendUrl(urls, "video=USB Video Device|1280x720|25");
        //appendUrl(urls, "video=USB Video Device:audio=麦克风 (USB Audio Device)|1280x720|25");
        appendUrl(urls, "desktop=desktop||25|0|0");
#elif defined(Q_OS_LINUX)
        appendUrl(urls, "/home/liu/1.mp4");
        appendUrl(urls, "/dev/video0");
        appendUrl(urls, "desktop=:0.0||25|0|0");
#elif defined(Q_OS_MAC)
        appendUrl(urls, "desktop=0:0||25|0|0");
#endif

        appendUrl(urls, "http://music.163.com/song/media/outer/url?id=5238772.mp3");
        appendUrl(urls, "http://video.ubtrobot.com/A Fly In The Hand.mp4");
        appendUrl(urls, "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4");
        appendUrl(urls, "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4");
        appendUrl(urls, "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8");
        appendUrl(urls, "http://47.114.127.78:6900/cf3619ac1efe848d052d37c6b219470b");
        appendUrl(urls, "rtmp://47.114.127.78:6908/cf3619ac1efe848d052d37c6b219470b");
        appendUrl(urls, "http://222.175.159.226:808/hls/20231/index.m3u8");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.15:554/media/video1");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101?transportmode=unicast&profile=Profile_2");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
        appendUrl(urls, "rtsp://admin:Admin123456@192.168.0.108:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
    }

    if (type & 0x80) {
        appendUrl(urls, "http://222.175.159.226:808/hls/20231/index.m3u8");
        appendUrl(urls, "http://222.175.159.226:808/hls/20232/index.m3u8");
        appendUrl(urls, "http://222.175.159.226:808/hls/20233/index.m3u8");
        appendUrl(urls, "http://222.175.159.226:808/hls/20234/index.m3u8");
    }

    return urls;
}

void VideoUrl::appendUrl(QStringList &urls, const QString &url)
{
    if (!urls.contains(url)) {
        urls << url;
    }
}

QStringList VideoUrl::getUrls2(int index)
{
    QStringList urls;
    if (index == 1) {
        for (int i = 1; i <= 4; ++i) {
            urls << QString("f:/mp4/push/%1.mp4").arg(i);
        }
    } else if (index == 2) {
        urls << "f:/mp4/asf/1.asf";
        urls << "f:/mp4/push/16.mp4";
        urls << "f:/mp4/high/1.mp4";
        urls << "f:/mp4/high/4k.mp4";
    } else if (index == 3) {
        for (int i = 1; i <= 16; ++i) {
            urls << QString("f:/mp4/push/%1.mp4").arg(i);
        }
    } else if (index == 4) {
        urls << "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
        urls << "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
        urls << "http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4";
        urls << "http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4";
    } else if (index == 5) {
        for (int i = 1; i <= 16; ++i) {
            urls << "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
        }
    } else if (index == 6) {
        urls << "http://222.175.159.226:808/hls/20231/index.m3u8";
        urls << "http://222.175.159.226:808/hls/20232/index.m3u8";
        urls << "http://222.175.159.226:808/hls/20233/index.m3u8";
        urls << "http://222.175.159.226:808/hls/20234/index.m3u8";
    } else if (index == 7) {
        for (int i = 1; i <= 16; ++i) {
            urls << QString("http://222.175.159.226:808/hls/2023%1/index.m3u8").arg(i);
        }
    } else if (index == 8) {
        urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
        urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101?transportmode=unicast&profile=Profile_2";
        urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
        urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
    } else if (index == 9) {
        for (int i = 1; i <= 16; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
        }
    } else if (index == 10) {
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/101?transportmode=unicast&profile=Profile_2";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
        }
    } else if (index == 11) {
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video2";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.64:554/Streaming/Channels/102?transportmode=unicast&profile=Profile_2";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.100:554/cam/realmonitor?channel=1&subtype=1&unicast=true&proto=Onvif";
        }
        for (int i = 1; i <= 4; ++i) {
            urls << "rtsp://admin:Admin123456@192.168.0.106:554/cam/realmonitor?channel=1&subtype=1&unicast=true&proto=Onvif";
        }
    }

    return urls;
}
