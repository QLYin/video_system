#ifndef VIDEOURL_H
#define VIDEOURL_H

#include <QStringList>

class VideoUrl
{
public:
    //地址类型
    enum UrlType {
        RtspVideo = 0x01,
        HttpVideo = 0x02,
        HttpAudio = 0x04,
        FileVideo = 0x08,
        FileAudio = 0x10,
        UsbCamera = 0x20,
        Simple = 0x40,
        TvStation = 0x80,
        All = RtspVideo | HttpVideo | HttpAudio | FileVideo | UsbCamera | TvStation
    };

    //读取和写入地址
    static void readUrls(const QString &fileName, QStringList &urls);
    static void writeUrls(const QString &fileName, const QStringList &urls);

    //获取地址集合
    static QStringList getUrls(int type);
    //去重插入地址
    static void appendUrl(QStringList &urls, const QString &url);

    //根据类型获取地址
    static QStringList getUrls2(int index);
};

#endif // VIDEOURL_H
