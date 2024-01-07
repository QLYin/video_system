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
   
    // 解锁设备
    // cmd : UnlockDevice
    static void sendUnlockDevice();

    // 同步ipc信息1 幕墙4
    // cmd : DataSync
    static void sendDataSync(int type);

    // 同步场景信息
    static void sendSceneInfo();

    // 设置幕墙
    static void sendWallSet();

};

#endif // TCPCLIENTHELPER_H
