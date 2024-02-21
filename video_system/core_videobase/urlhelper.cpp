#include "urlhelper.h"
#include "qregexp.h"

QString UrlHelper::getRtspUrl(const CompanyType &companyType, int channel, int streamType)
{
    QString url;
    if (companyType == CompanyType_HaiKang) {
        url = QString("rtsp://admin:12345@[Addr]:554/Streaming/Channels/%1%2?transportmode=unicast").arg(channel + 1).arg(streamType + 1, 2, 10, QChar('0'));
    } else if (companyType == CompanyType_DaHua) {
        url = QString("rtsp://admin:12345@[Addr]:554/cam/realmonitor?channel=%1&subtype=%2&unicast=true&proto=Onvif").arg(channel + 1).arg(streamType);
    } else if (companyType == CompanyType_YuShi) {
        url = QString("rtsp://admin:12345@[Addr]/media/video%1").arg(streamType + 1);
    } else if (companyType == CompanyType_Normal) {
        url = QString("rtsp://admin:12345@[Addr]/live?channel=%1&stream=%2").arg(channel + 1).arg(streamType);
    } else {
        url = QString("rtsp://admin:12345@[Addr]:554/%1").arg(streamType);
    }
    return url;
}

QString UrlHelper::getRtspUrl(const UrlPara &urlPara)
{
    QString url;
    //头部地址格式完全一致
    QString head = QString("rtsp://%1:%2@%3:554").arg(urlPara.userName).arg(urlPara.userPwd).arg(urlPara.deviceIP);
    if (urlPara.companyType == CompanyType_HaiKang) {
        //实时预览格式 rtsp://admin:12345@192.168.1.128:554/Streaming/Channels/101?transportmode=unicast
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/Streaming/tracks/101?starttime=20120802t063812z&endtime=20120802t064816z
        //流媒体视频流 rtsp://172.6.24.15:554/Devicehc8://172.6.22.106:8000:0:0?username=admin&password=12345
        //日期时间格式 ISO 8601 表示Zulu(GMT) 时间 YYYYMMDD”T”HHmmSS.fraction”Z”，
        //unicast表示单播,multicast表示多播,默认单播可以省略
        //101解析: 1是通道号 01是通道的码流编号 也可以是02 03
        QString startTimeISO = urlPara.dateTimeStart.toString(Qt::ISODate);
        startTimeISO.replace("-", "");
        startTimeISO.replace(":", "");
        startTimeISO.toLower();

        QString endTimeISO = urlPara.dateTimeEnd.toString(Qt::ISODate);
        endTimeISO.replace("-", "");
        endTimeISO.replace(":", "");
        endTimeISO.toLower();

        //通道号和码流编号
        QString info = QString("%1%2%3").arg(urlPara.channel).arg(0).arg(urlPara.streamType + 1);
        //回放时间范围
        QString time = QString("starttime=%1z&endtime=%2z").arg(startTimeISO).arg(endTimeISO);
        //实时和回放地址格式不同
        if (urlPara.videoType == 0) {
            url = QString("%1/Streaming/Channels/%2").arg(head).arg(info);
        } else if (urlPara.videoType == 1) {
            url = QString("%1/Streaming/tracks/%2?%3").arg(head).arg(info).arg(time);
        }
    } else if (urlPara.companyType == CompanyType_DaHua) {
        //实时预览格式 rtsp://192.168.1.128:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/cam/playback?channel=1&subtype=0&starttime=2021_03_18_11_36_01&endtime=2021_03_18_12_05_01
        QString startTimeStr = urlPara.dateTimeStart.toString("yyyy_MM_dd_HH_mm_ss");
        QString endTimeStr = urlPara.dateTimeEnd.toString("yyyy_MM_dd_HH_mm_ss");
        //通道号和码流编号
        QString info = QString("channel=%1&subtype=%2").arg(urlPara.channel).arg(urlPara.streamType);
        //回放时间范围
        QString time = QString("starttime=%1&endtime=%2").arg(startTimeStr).arg(endTimeStr);
        //实时和回放地址格式不同
        if (urlPara.videoType == 0) {
            url = QString("%1/cam/realmonitor?%2&unicast=true&proto=Onvif").arg(head).arg(info);
        } else if (urlPara.videoType == 1) {
            url = QString("%1/cam/playback?%2&%3").arg(head).arg(info).arg(time);
        }
    } else {
        //实时预览格式 rtsp://admin:12345@192.168.1.128:554/live?channel=1&stream=1
        //视频回放格式 rtsp://admin:12345@192.168.1.128:554/file?channel=1&start=1494485280&stop=1494485480
        //先转换时间戳,1970年到该时间经过的秒数
        qint64 startTimeSec = urlPara.dateTimeStart.toMSecsSinceEpoch() / 1000;
        qint64 stopTimeSec = urlPara.dateTimeEnd.toMSecsSinceEpoch() / 1000;
        //回放时间范围
        QString time = QString("start=%1&stop=%2").arg(startTimeSec).arg(stopTimeSec);
        //实时和回放地址格式不同
        if (urlPara.videoType == 0) {
            url = QString("%1/live?channel=%2&stream=%3").arg(head).arg(urlPara.channel).arg(urlPara.streamType);
        } else if (urlPara.videoType == 1) {
            url = QString("%1/file?channel=%2&%3").arg(head).arg(urlPara.channel).arg(time);
        }
    }

    //还有一种通用格式 rtsp://admin:12345@192.168.1.128:554/0  0-主码流 1-子码流
    return url;
}

void UrlHelper::removeDefaultPort(QString &url)
{
    if (url.startsWith("rtsp://")) {
        if (url.contains(":554/")) {
            url.replace(":554/", "/");
        }
    } else if (url.startsWith("rtmp://")) {
        if (url.contains(":1935/")) {
            url.replace(":1935/", "/");
        }
    } else if (url.startsWith("http://")) {
        if (url.contains(":80/")) {
            url.replace(":80/", "/");
        }
    }
}

QString UrlHelper::getUrlHost(const QString &url)
{
    //先获取IP地址
    QString host = getUrlIP(url);
    //如果非IP地址则取出中间主机部分
    if (host.split(".").first().toInt() == 0) {
        host = host.split("/").at(2);
    }
    return host;
}

QString UrlHelper::getUrlIP(const QString &url)
{
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    int start = regExp.indexIn(url);
    int length = regExp.matchedLength();
    return url.mid(start, length);
}

int UrlHelper::getUrlPort(const QString &url)
{
    //必须是最后一个:符号(用户可能地址中带了用户信息用:作为分隔符)
    int index = url.lastIndexOf(":");
    QString temp = url.mid(index + 1, 6);
    QStringList list = temp.split("/");
    int port = list.at(0).toInt();
    if (port == 0) {
        if (url.startsWith("rtsp://")) {
            port = 554;
        } else if (url.startsWith("rtmp://")) {
            port = 1935;
        } else if (url.startsWith("http://")) {
            port = 80;
        }
    }
    return port;
}

void UrlHelper::getUserInfo(const QString &url, QString &userName, QString &userPwd)
{
    userName = "admin";
    userPwd = "12345";
    //必须是最后一个@符号(因为用户名称和密码也可能是这个字符)
    int index = url.lastIndexOf("@");
    if (index > 0) {
        QString userInfo = url.mid(0, index);
        userInfo.replace("rtsp://", "");
        QStringList list = userInfo.split(":");
        userName = list.at(0);
        userPwd = list.at(1);
    }
}

QString UrlHelper::getCompanyName(const QString &url)
{
    QString companyName;
    if (url.contains("/Streaming/Channels/")) {
        companyName = "HIKVISION";
    } else if (url.contains("/cam/realmonitor?channel=")) {
        companyName = "Dahua";
    } else if (url.contains("/media/video")) {
        companyName = "UNIVIEW";
    } else if (url.contains("/live?channel=")) {
        companyName = "General";
    }
    return companyName;
}

CompanyType UrlHelper::getCompanyType(const QString &url)
{
    CompanyType companyType;
    if (url.contains("/Streaming/Channels/")) {
        companyType = CompanyType_HaiKang;
    } else if (url.contains("/cam/realmonitor?channel=")) {
        companyType = CompanyType_DaHua;
    } else if (url.contains("/media/video")) {
        companyType = CompanyType_YuShi;
    } else if (url.contains("/live?channel=")) {
        companyType = CompanyType_Normal;
    }
    return companyType;
}

void UrlHelper::getOtherInfo(const QString &url, int &channel, int &streamType)
{
    CompanyType companyType = getCompanyType(url);
    QString temp = url.split("/").last();
    if (companyType == CompanyType_HaiKang) {
        temp = temp.split("?").first();
        //101=通道1码流01 1602=通道16码流02
        channel = temp.mid(0, temp.length() - 2).toInt();
        streamType = temp.right(2).toInt();
    } else if (companyType == CompanyType_DaHua) {
        temp = temp.split("?").last();
        QStringList list = temp.split("&");
        foreach (QString text, list) {
            int value = text.split("=").last().toInt();
            if (text.startsWith("channel")) {
                channel = value;
            } else if (text.startsWith("subtype")) {
                streamType = value;
            }
        }
    } else if (companyType == CompanyType_YuShi) {
        streamType = temp.mid(5, 2).toInt();
    } else if (companyType == CompanyType_Normal) {
        temp = temp.split("?").last();
        QStringList list = temp.split("&");
        foreach (QString text, list) {
            int value = text.split("=").last().toInt();
            if (text.startsWith("channel")) {
                channel = value;
            } else if (text.startsWith("stream")) {
                streamType = value;
            }
        }
    } else {
        streamType = temp.toInt();
    }
}

void UrlHelper::getUrlPara(const QString &url, UrlPara &urlPara)
{
    urlPara.deviceIP = getUrlIP(url);
    urlPara.devicePort = getUrlPort(url);
    getUserInfo(url, urlPara.userName, urlPara.userPwd);
    urlPara.companyName = getCompanyName(url);
    urlPara.companyType = getCompanyType(url);
    getOtherInfo(url, urlPara.channel, urlPara.streamType);
}
