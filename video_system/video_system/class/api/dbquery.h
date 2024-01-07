#ifndef DBQUERY_H
#define DBQUERY_H

#include "datacsv.h"
#include "dbconnthread.h"
#include "devicebutton.h"

class DbQuery
{
public:
    //是否从数据库加载
    static bool loadfromdb;    

    //添加用户日志
    static QString addUserLog(const QString &logCh, const QString &logType, const QString &logContent,
                              const QString &logMark, bool exec = true);
    static void addUserLog(const QString &logType, const QString &logContent);
    static void addUserLog(const QString &logContent);

    //随机生成数据
    static void addUserLog(int count, const QString &logContent);

    //删除用户记录
    static void deleteUserLog(const QString &timeStart, const QString &timeEnd);
    static void clearUserLog();

    //载入+清空+添加 录像机信息
    static void loadNvrInfo();
    static void clearNvrInfo();
    static void addNvrInfo(const QString &ip, const QString &name, const QString &type);

    //载入+清空+删除+批量删除 摄像机信息
    static void loadIpcInfo();
    static void clearIpcInfo();
    static void deleteIpcInfo(const QString &nvrName);
    static void deleteIpcInfos(const QString &ids);

    //根据编号获取信息比如经纬度
    static void getIpcInfo(int ipcID, QString &ipcPosition);
    static void getIpcInfo(const QString &rtsp, QString &ipcPosition);

    //根据输入经纬度获取离这个经纬度最近的摄像机
    static int getIpcInfo(double lng, double lat);
    //更新设备经纬度+按钮位置
    static void updatePosition(DeviceButton *btn);
    static void updatePosition(int id, const QString &position);

    //载入+清空+删除+批量删除 轮询信息
    static void loadPollInfo();
    static void clearPollInfo();
    static void deletePollInfo(const QString &rtspAddr);
    static void deletePollInfos(const QString &addrs);

    //载入+清空 录像计划信息
    static void loadRecordInfo();
    static void clearRecordInfo();
    //获取录像计划
    static QList<QStringList> getRecordTimes();

    //校验数据库
    static bool checkDb();
};

#endif // DBQUERY_H
