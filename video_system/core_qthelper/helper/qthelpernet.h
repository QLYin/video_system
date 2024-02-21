#ifndef QTHELPERNET_H
#define QTHELPERNET_H

#include <QObject>

class QtHelperNet
{
public:
    //从字符串获取IP地址
    static QString getIP(const QString &url);
    //判断是否是IP地址
    static bool isIP(const QString &ip);
    //判断是否是MAC地址
    static bool isMac(const QString &mac);
    //判断是否是合法的电话号码
    static bool isTel(const QString &tel);
    //判断是否是合法的邮箱地址
    static bool isEmail(const QString &email);

    //IP地址字符串与整型转换
    static QString ipv4IntToString(quint32 ip);
    static quint32 ipv4StringToInt(const QString &ip);

    //判断主机地址及端口是否在线
    static bool hostLive(const QString &hostName, int port, int timeout = 1000);
    //下载网络文件
    static bool download(const QString &url, const QString &fileName, int timeout = 1000);
    //获取网页所有源代码
    static QByteArray getHtml(const QString &url, int timeout = 1000);

    //获取本机公网IP地址
    static QString getNetIP(const QString &html);
    //获取本机IP
    static QString getLocalIP();
    //获取本机IP地址集合
    static QStringList getLocalIPs();
    //Url地址转为IP地址
    static QString urlToIP(const QString &url);
};

#endif // QTHELPERNET_H
