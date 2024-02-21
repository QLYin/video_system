#ifndef ONVIFHELPER_H
#define ONVIFHELPER_H

#include "onvifdevice.h"
class OnvifDevice;

class OnvifHelper
{
public:
    //获取uuid
    static QString getUuid();
    //设置延时
    static void sleep(int msec);

    //判断是否是合法的IP
    static bool isIP(const QString &ip);
    //取出url地址中的地址
    static QString getUrlIP(const QString &url);
    //取出url地址中的端口
    static int getUrlPort(const QString &url);
    //判断主机地址及端口是否在线
    static bool hostLive(const QString &hostName, int port, int timeout = 1000);

    //通用获取数据节点方法
    static QString getValue2(const QByteArray &data, const QString &key);
    static QString getValue3(const QByteArray &data, const QString &key);
    static QString getValue4(const QByteArray &data, const QString &key);

    //读取本地文件内容
    static QByteArray getFile(const QString &fileName);
    //取出第一个url
    static QString getFirstUrl(const QString &url, int index = 0);

    //IP地址字符串与整型转换
    static QString ipv4IntToString(quint32 ip);
    static quint32 ipv4StringToInt(const QString &ip);

    //子网掩码和前缀长度转换 24=255.255.255.0
    static quint32 ipv4ToPrefixLength(const QString &ip);
    static QString prefixLengthToIpv4(int prefixlen);

    //osd坐标和屏幕坐标互换
    static QPoint osdPosToScreenPos(int width, int height, float x, float y);
    static QPointF screenPosToOsdPos(int width, int height, float x, float y);
};

#endif // ONVIFHELPER_H
