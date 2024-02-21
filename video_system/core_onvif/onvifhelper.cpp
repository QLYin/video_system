#include "onvifhelper.h"
#include "qregexp.h"

QString OnvifHelper::getUuid()
{
    QUuid uid = QUuid::createUuid();
    QString str = uid.toString();
    QRegExp reg("\\{|\\}");
    int start = reg.indexIn(str);
    int length = reg.matchedLength();
    QString temp = str.mid(start, length);
    str.replace(temp, "");
    return str;
}

void OnvifHelper::sleep(int msec)
{
    if (msec > 0) {
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        QTime endTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < endTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
#else
        QThread::msleep(msec);
#endif
    }
}

bool OnvifHelper::isIP(const QString &ip)
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}

QString OnvifHelper::getUrlIP(const QString &url)
{
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    int start = regExp.indexIn(url);
    int length = regExp.matchedLength();
    return url.mid(start, length);
}

int OnvifHelper::getUrlPort(const QString &url)
{
    //必须是最后一个:符号(用户可能地址中带了用户信息用:作为分隔符)
    int index = url.lastIndexOf(":");
    QString temp = url.mid(index + 1, 6);
    QStringList list = temp.split("/");
    int port = list.at(0).toInt();
    return (port == 0 ? (url.startsWith("rtsp://") ? 554 : 80) : port);
}

bool OnvifHelper::hostLive(const QString &hostName, int port, int timeout)
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

QString OnvifHelper::getValue2(const QByteArray &data, const QString &key)
{
    //直接暴力取节点 比如 wsa5:Address
    QString value = "-1";
    int indexStart = data.indexOf(key.toUtf8()) + key.length() + 1;
    int indexEnd = data.indexOf(QString("/" + key).toUtf8()) - 1;
    //qDebug() << key << indexStart << indexEnd;
    if (indexStart >= 0 && indexEnd >= 0 && indexStart < indexEnd) {
        value = data.mid(indexStart, indexEnd - indexStart);
    }

    return value;
}

QString OnvifHelper::getValue3(const QByteArray &data, const QString &key)
{
    //字符串匹配取节点 比如 Address
    QString value = "-1";
    int indexStart = data.indexOf(key.toUtf8()) + key.length() + 1;
    int indexEnd = data.lastIndexOf(key.toUtf8());
    //qDebug() << key << indexStart << indexEnd;
    if (indexStart >= 0 && indexEnd >= 0 && indexStart < indexEnd) {
        value = data.mid(indexStart, indexEnd - indexStart);
        //要去除结尾 http://192.168.1.247:7655</wsa5:
        value = value.split("</").first();
    }

    return value;
}

QString OnvifHelper::getValue4(const QByteArray &data, const QString &key)
{
    //直接暴力取节点 比如 wsa5:Address 自动拼成<wsa5:Address> 找位置
    QString value = "-1";
    QString key1 = QString("<%1>").arg(key);
    QString key2 = QString("</%1>").arg(key);
    int indexStart = data.indexOf(key1.toUtf8()) + key1.length();
    int indexEnd = data.indexOf(key2.toUtf8());
    //qDebug() << key << indexStart << indexEnd;
    if (indexStart >= 0 && indexEnd >= 0 && indexStart < indexEnd) {
        value = data.mid(indexStart, indexEnd - indexStart);
    }

    return value;
}

QByteArray OnvifHelper::getFile(const QString &fileName)
{
    QByteArray data;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
        //加上回车换行方便打印查看
        data += "\r\n";
        file.close();
    }

    return data;
}

QString OnvifHelper::getFirstUrl(const QString &url, int index)
{
    QString result;
    QStringList list = url.split(' ', SkipEmptyPartsx);
    if (list.length() > index) {
        result = list.at(index).trimmed();
    }

    return result;
}

QString OnvifHelper::ipv4IntToString(quint32 ip)
{
    QString result = QString("%1.%2.%3.%4").arg((ip >> 24) & 0xFF).arg((ip >> 16) & 0xFF).arg((ip >> 8) & 0xFF).arg(ip & 0xFF);
    return result;
}

quint32 OnvifHelper::ipv4StringToInt(const QString &ip)
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

quint32 OnvifHelper::ipv4ToPrefixLength(const QString &ip)
{
    QStringList list = ip.split(".");
    quint8 c1 = list.at(0).toInt();
    quint8 c2 = list.at(1).toInt();
    quint8 c3 = list.at(2).toInt();
    quint8 c4 = list.at(3).toInt();
    quint32 sum = c1 << 24 | c2 << 16 | c3 << 8 | c4;

    //绝大部分的情况快速判断即可
    if (sum >= 0xffffffff) {
        return 32;
    } else if (sum == 0xffffff00) {
        return 24;
    } else if (sum == 0xffff0000) {
        return 16;
    } else if (sum == 0xff000000) {
        return 8;
    }

    //挨个计算
    quint32 result = 0;
    for (int i = 0; i < 32; ++i) {
        if ((sum << i) & 0x80000000) {
            result++;
        } else {
            break;
        }
    }

    return result;
}

QString OnvifHelper::prefixLengthToIpv4(int prefixlen)
{
    //绝大部分的情况快速判断即可
    if (prefixlen == 8) {
        return "255.0.0.0";
    } else if (prefixlen == 16) {
        return "255.255.0.0";
    } else if (prefixlen == 24) {
        return "255.255.255.0";
    } else if (prefixlen >= 32) {
        return "255.255.255.255";
    }

    int sum = 0;
    for (int i = prefixlen, j = 31; i > 0; i--, j--) {
        sum += (1 << j);
    }

    return QString("%1.%2.%3.%4").arg((sum >> 24) & 0xff).arg((sum >> 16) & 0xff).arg((sum >> 8) & 0xff).arg(sum & 0xff);
}

QPoint OnvifHelper::osdPosToScreenPos(int width, int height, float x, float y)
{
    x = (1.0 + x) * ((float)width) / 2.0;
    y = (1.0 - y) * ((float)height) / 2.0;
    return QPoint(x, y);
}

QPointF OnvifHelper::screenPosToOsdPos(int width, int height, float x, float y)
{
    x = ((float)(x - (int)width / 2)) / (float)(width / 2);
    y = ((float)(y - (int)height / 2)) / (float)(height / 2);
    if (y != 0) {
        y = -y;
    }
    return QPointF(x, y);
}
