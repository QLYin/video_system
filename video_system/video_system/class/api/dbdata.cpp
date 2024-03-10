#include "dbdata.h"
#include "appconfig.h"

DbConnThread *DbData::DbLocal = 0;

int DbData::UserLogID = 0;

int DbData::NvrInfo_Count = 0;
QList<int> DbData::NvrInfo_NvrID = QList<int>();
QList<QString> DbData::NvrInfo_NvrName = QList<QString>();
QList<QString> DbData::NvrInfo_NvrType = QList<QString>();
QList<QString> DbData::NvrInfo_NvrIP = QList<QString>();
QList<QString> DbData::NvrInfo_UserName = QList<QString>();
QList<QString> DbData::NvrInfo_UserPwd = QList<QString>();
QList<QString> DbData::NvrInfo_NvrEnable = QList<QString>();

int DbData::IpcInfo_Count = 0;
QList<int> DbData::IpcInfo_IpcID = QList<int>();
QList<QString> DbData::IpcInfo_IpcName = QList<QString>();
QList<QString> DbData::IpcInfo_NvrName = QList<QString>();
QList<QString> DbData::IpcInfo_IpcType = QList<QString>();
QList<QString> DbData::IpcInfo_IpAddr = QList<QString>();
QList<QString> DbData::IpcInfo_OnvifAddr = QList<QString>();
QList<QString> DbData::IpcInfo_ProfileToken = QList<QString>();
QList<QString> DbData::IpcInfo_VideoSource = QList<QString>();
QList<QString> DbData::IpcInfo_RtspMain = QList<QString>();
QList<QString> DbData::IpcInfo_RtspSub = QList<QString>();
QList<QString> DbData::IpcInfo_MainResolutionRatio = QList<QString>();
QList<QString> DbData::IpcInfo_SubResolutionRatio = QList<QString>();
QList<int> DbData::IpcInfo_IpcX = QList<int>();
QList<int> DbData::IpcInfo_IpcY = QList<int>();
QList<QString> DbData::IpcInfo_UserName = QList<QString>();
QList<QString> DbData::IpcInfo_UserPwd = QList<QString>();
QList<QString> DbData::IpcInfo_IpcEnable = QList<QString>();
QList<bool> DbData::IpcInfo_IpcOnline = QList<bool>();

QString DbData::getRtspAddr(int index)
{
    if (index >= DbData::IpcInfo_Count) {
        return QString();
    }

    if (AppConfig::RtspType == 0) {
        return DbData::IpcInfo_RtspMain.at(index);
    } else {
        return DbData::IpcInfo_RtspSub.at(index);
    }
}

int DbData::PollInfo_Count = 0;
QList<int> DbData::PollInfo_PollID = QList<int>();
QList<QString> DbData::PollInfo_PollGroup = QList<QString>();
QList<QString> DbData::PollInfo_RtspMain = QList<QString>();
QList<QString> DbData::PollInfo_RtspSub = QList<QString>();

int DbData::RecordInfo_Count = 0;
QList<int> DbData::RecordInfo_RecordCh = QList<int>();
QList<QString> DbData::RecordInfo_RecordWeek1 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek2 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek3 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek4 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek5 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek6 = QList<QString>();
QList<QString> DbData::RecordInfo_RecordWeek7 = QList<QString>();
