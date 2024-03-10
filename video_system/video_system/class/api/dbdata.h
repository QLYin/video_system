#ifndef DBDATA_H
#define DBDATA_H

#include "dbconnthread.h"

class DbData
{
public:
    //本地数据库连接
    static DbConnThread *DbLocal;

    //用户日志表最大编号
    static int UserLogID;    

    //录像机信息表
    static int NvrInfo_Count;
    static QList<int> NvrInfo_NvrID;
    static QList<QString> NvrInfo_NvrName;
    static QList<QString> NvrInfo_NvrType;
    static QList<QString> NvrInfo_NvrIP;
    static QList<QString> NvrInfo_UserName;
    static QList<QString> NvrInfo_UserPwd;
    static QList<QString> NvrInfo_NvrEnable;

    //摄像机信息表
    static int IpcInfo_Count;
    static QList<int> IpcInfo_IpcID;
    static QList<QString> IpcInfo_IpcName;
    static QList<QString> IpcInfo_NvrName;
    static QList<QString> IpcInfo_IpcType;
    static QList<QString> IpcInfo_IpAddr;
    static QList<QString> IpcInfo_OnvifAddr;
    static QList<QString> IpcInfo_ProfileToken;
    static QList<QString> IpcInfo_VideoSource;
    static QList<QString> IpcInfo_RtspMain;
    static QList<QString> IpcInfo_RtspSub;
    static QList<QString> IpcInfo_MainResolutionRatio;
    static QList<QString> IpcInfo_SubResolutionRatio;
    static QList<int> IpcInfo_IpcX;
    static QList<int> IpcInfo_IpcY;
    static QList<QString> IpcInfo_UserName;
    static QList<QString> IpcInfo_UserPwd;
    static QList<QString> IpcInfo_IpcEnable;
    static QList<bool> IpcInfo_IpcOnline;

    //根据配置文件默认打开主码流还是子码流取出流地址
    static QString getRtspAddr(int index);

    //轮询信息表
    static int PollInfo_Count;
    static QList<int> PollInfo_PollID;
    static QList<QString> PollInfo_PollGroup;
    static QList<QString> PollInfo_RtspMain;
    static QList<QString> PollInfo_RtspSub;

    //录像计划表
    static int RecordInfo_Count;
    static QList<int> RecordInfo_RecordCh;
    static QList<QString> RecordInfo_RecordWeek1;
    static QList<QString> RecordInfo_RecordWeek2;
    static QList<QString> RecordInfo_RecordWeek3;
    static QList<QString> RecordInfo_RecordWeek4;
    static QList<QString> RecordInfo_RecordWeek5;
    static QList<QString> RecordInfo_RecordWeek6;
    static QList<QString> RecordInfo_RecordWeek7;
};

#endif // DBDATA_H
