#include "qthelpernet.h"
#include "qthelper.h"

QString QtHelperNet::getIP(const QString &url)
{
    //取出IP地址
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    int start = regExp.indexIn(url);
    int length = regExp.matchedLength();
    QString ip = url.mid(start, length);
    return ip;
}

bool QtHelperNet::isIP(const QString &ip)
{
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return regExp.exactMatch(ip);
}

bool QtHelperNet::isMac(const QString &mac)
{
    QRegExp regExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
    return regExp.exactMatch(mac);
}

bool QtHelperNet::isTel(const QString &tel)
{
    if (tel.length() != 11) {
        return false;
    }

    if (!tel.startsWith("13") && !tel.startsWith("14") && !tel.startsWith("15") && !tel.startsWith("18")) {
        return false;
    }

    return true;
}

bool QtHelperNet::isEmail(const QString &email)
{
    if (!email.contains("@") || !email.contains(".com")) {
        return false;
    }

    return true;
}

QString QtHelperNet::ipv4IntToString(quint32 ip)
{
    QString result = QString("%1.%2.%3.%4").arg((ip >> 24) & 0xFF).arg((ip >> 16) & 0xFF).arg((ip >> 8) & 0xFF).arg(ip & 0xFF);
    return result;
}

quint32 QtHelperNet::ipv4StringToInt(const QString &ip)
{
    int result = 0;
    if (isIP(ip)) {
        QStringList list = ip.split(".");
        int ip0 = list.at(0).toInt();
        int ip1 = list.at(1).toInt();
        int ip2 = list.at(2).toInt();
        int ip3 = list.at(3).toInt();
        result = ip3 | ip2 << 8 | ip1 << 16 | ip0 << 24;
    }
    return result;
}

bool QtHelperNet::hostLive(const QString &hostName, int port, int timeout)
{
    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    //主动测试下连接
    QTcpSocket tcpSocket;
    QObject::connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(hostName, port);
    eventLoop.exec();

    //判断最终状态
    bool ok = (tcpSocket.state() == QAbstractSocket::ConnectedState);
    return ok;
}

bool QtHelperNet::download(const QString &url, const QString &fileName, int timeout)
{
    QByteArray data = getHtml(url, timeout);
    if (!data.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            file.write(data);
            file.close();
        }
        return true;
    }

    return false;
}

QByteArray QtHelperNet::getHtml(const QString &url, int timeout)
{
    //初始化网络请求对象
    QNetworkAccessManager manager;
    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时 5.15开始自带了超时时间函数 默认30秒
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
    manager.setTransferTimeout(timeout);
#else
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);
#endif

    //启动网络请求
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    //读取结果
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

QString QtHelperNet::getNetIP(const QString &html)
{
    QString ip;
    QStringList list = html.split(" ");
    foreach (QString str, list) {
        //value=\"101.86.197.178\">
        if (str.contains("value=")) {
            QStringList temp = str.split("\"");
            ip = temp.at(1);
            break;
        }
    }
    return ip;
}

QString QtHelperNet::getLocalIP()
{
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    QStringList ips = getLocalIPs();
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            break;
        }
    }

    return ip;
}

QStringList QtHelperNet::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef Q_OS_WASM
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface netInterface, netInterfaces) {
            //移除虚拟机和抓包工具的虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") || humanReadableName.startsWith("npcap loopback adapter")) {
                continue;
            }

            //过滤当前网络接口
            bool flag = (netInterface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast | QNetworkInterface::CanMulticast));
            if (!flag) {
                continue;
            }

            QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
            foreach (QNetworkAddressEntry addr, addrs) {
                //只取出IPV4的地址
                if (addr.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                    continue;
                }

                QString ip4 = addr.ip().toString();
                if (ip4 != "127.0.0.1") {
                    ips << ip4;
                }
            }
        }
#endif
    }

    return ips;
}

QString QtHelperNet::urlToIP(const QString &url)
{
    QHostInfo host = QHostInfo::fromName(url);
    return host.addresses().at(0).toString();
}
