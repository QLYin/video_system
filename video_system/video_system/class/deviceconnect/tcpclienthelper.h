#ifndef TCPCLIENTHELPER_H
#define TCPCLIENTHELPER_H

#include "head.h"
#include "tcpclient.h"

#ifndef TIME
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#endif

class TcpClientHelper
{
public:
    // 解码卡
    static void sendDevCmd(const QString& cmd, const QVariantMap& param);

    // ipc
    static void sendIPCCmd(const QString& cmd, const QVariantMap& param);

    // 幕墙
    static void sendWallCmd(const QString& cmd, const QVariantMap& param);

    // 数据同步
    static void sendDataSync(int type);

    static void sendCmd(const QString& cmd, QVariantMap param = QVariantMap());

};

#endif // TCPCLIENTHELPER_H
