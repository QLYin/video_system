#include "devicethreadui.h"
#include "devicehelper.h"
#include "dbquery.h"

DeviceThreadUI::DeviceThreadUI(QObject *parent) : QThread(parent)
{
    stopped = false;
}

void DeviceThreadUI::run()
{
    //先初始化随机数种子 否则生成的随机数很可能一致
    srand((quint64)currentThreadId());

    while (!stopped) {
        //演示每隔一段时间插入消息
        QString msg = "测试消息";
        QString result = "已处理";
        QString file = QString(":/image/liuyifei%1.jpg").arg(rand() % 3);
        //异步调用添加表格消息和图文警情
        QMetaObject::invokeMethod(this, "addMsg", Q_ARG(QString, msg));
        QMetaObject::invokeMethod(this, "addMsgList", Q_ARG(QString, msg), Q_ARG(QString, result), Q_ARG(QImage, QImage(file)));

        //演示设备GPS移动
        AppEvent::Instance()->slot_moveDevice(0, "121.415086", "31.183769");

        //演示报警闪烁
        if (1) {
            static bool isAlarm = true;
            isAlarm = !isAlarm;
            if (!isAlarm) {
                AppEvent::Instance()->slot_startAlarm();
                //演示插入一条带视频文件地址的报警记录用于双击播放
                DbQuery::addUserLog("0", "设备日志", "测试报警", "https://hls01open.ys7.com/openlive/6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8");
            } else {
                AppEvent::Instance()->slot_stopAlarm();
                DbQuery::addUserLog("停止报警");
            }
        }

        //演示指定通道编号设置视频和通道交换
        if (1) {
            static int index = 0;
            if (index == 0) {
                AppEvent::Instance()->slot_loadVideo(0, "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4");
            } else if (index == 1) {
                AppEvent::Instance()->slot_loadVideo(0, "http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4");
            } else if (index == 2) {
                AppEvent::Instance()->slot_loadVideo(1, "http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4");
            } else if (index == 3) {
                AppEvent::Instance()->slot_loadVideo(1, "http://vfx.mtime.cn/Video/2022/12/17/mp4/221217153424902164.mp4");
            } else if (index == 4) {
                AppEvent::Instance()->slot_changeVideo(2, 3);
            } else if (index == 5) {
                AppEvent::Instance()->slot_changeVideo(3, 2);
            }

            index++;
            if (index == 6) {
                index = 0;
            }
        }

        //延时一段时间
        msleep(5 * 1000);
    }

    stopped = false;
}

void DeviceThreadUI::addMsg(const QString &msg, quint8 type)
{
    DeviceHelper::addMsg(msg, type);
}

void DeviceThreadUI::addMsgList(const QString &msg, const QString &result, const QImage &image)
{
    DeviceHelper::addMsgList(msg, result, image);
}
