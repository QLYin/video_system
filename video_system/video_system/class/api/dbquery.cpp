#include "dbquery.h"
#include "qthelper.h"
#include "maphelper.h"
#include "devicehelper.h"

bool DbQuery::loadfromdb = false;

QString DbQuery::addUserLog(const QString &logCh, const QString &logType, const QString &logContent, const QString &logMark, bool exec)
{
    DbData::UserLogID++;
    QString sql = "insert into LogInfo(LogID,TriggerTime,UserName,LogCh,LogType,LogContent,LogMark) values";
    QStringList values;
    values << QString::number(DbData::UserLogID);
    values << DATETIME;
    values << UserHelper::CurrentUserName;
    values << logCh;
    values << logType;
    values << logContent;
    values << logMark;

    sql = QString("%1('%2')").arg(sql).arg(values.join("','"));
    DbHelper::execSql(sql);
    return sql;
}

void DbQuery::addUserLog(const QString &logType, const QString &logContent)
{
    addUserLog("00", logType, logContent, "");
}

void DbQuery::addUserLog(const QString &logContent)
{
    addUserLog("用户操作", logContent);
}

void DbQuery::addUserLog(int count, const QString &logContent)
{
    QSqlDatabase::database().transaction();
    for (int i = 0; i < count; ++i) {
        DbQuery::addUserLog(logContent);
    }
    QSqlDatabase::database().commit();
}

void DbQuery::deleteUserLog(const QString &timeStart, const QString &timeEnd)
{
    QString sql = "delete from LogInfo where 1=1";
    DbHelper::getBetweenDate(sql, AppConfig::LocalDbType, "TriggerTime", timeStart, timeEnd);
    DbHelper::execSql(sql);
}

void DbQuery::clearUserLog()
{
    DbData::UserLogID = 0;
    DbHelper::clearTable("LogInfo", AppConfig::LocalDbType);
}

void DbQuery::loadNvrInfo()
{
    DbData::NvrInfo_Count = 0;
    DbData::NvrInfo_NvrID.clear();
    DbData::NvrInfo_NvrName.clear();
    DbData::NvrInfo_NvrType.clear();
    DbData::NvrInfo_NvrIP.clear();
    DbData::NvrInfo_UserName.clear();
    DbData::NvrInfo_UserPwd.clear();
    DbData::NvrInfo_NvrEnable.clear();

    QString sql = "select NvrID,NvrName,NvrType,NvrIP,UserName,UserPwd,NvrEnable from NvrInfo order by NvrID asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int nvrID = query.value(0).toInt();
        QString nvrName = query.value(1).toString();
        QString nvrType = query.value(2).toString();
        QString nvrIP = query.value(3).toString();
        QString userName = query.value(4).toString();
        QString userPwd = query.value(5).toString();
        QString nvrEnable = query.value(6).toString();
        if (nvrEnable != "启用") {
            continue;
        }

        DbData::NvrInfo_Count++;
        DbData::NvrInfo_NvrID << nvrID;
        DbData::NvrInfo_NvrName << nvrName;
        DbData::NvrInfo_NvrType << nvrType;
        DbData::NvrInfo_NvrIP << nvrIP;
        DbData::NvrInfo_UserName << userName;
        DbData::NvrInfo_UserPwd << userPwd;
        DbData::NvrInfo_NvrEnable << nvrEnable;
    }

    //如果没有一台NVR则虚拟一个,方便生成树状列表
    if (DbData::NvrInfo_Count == 0) {
        DbData::NvrInfo_Count = 1;
        DbData::NvrInfo_NvrID << 1;
        DbData::NvrInfo_NvrName << "默认设备";
        DbData::NvrInfo_NvrType << "海康";
        DbData::NvrInfo_NvrIP << "192.168.1.128";
        DbData::NvrInfo_UserName << "admin";
        DbData::NvrInfo_UserPwd << "123456";
        DbData::NvrInfo_NvrEnable << "启用";

        //将摄像机中的信息对应的NVR名称全部换掉
        QString sql = "update IpcInfo set NvrName='默认设备'";
        DbHelper::execSql(sql);
        DbQuery::loadIpcInfo();
        DeviceHelper::initDeviceTree();
        DeviceHelper::initVideoIcon();
    }
}

void DbQuery::clearNvrInfo()
{
    DbHelper::clearTable("NvrInfo", AppConfig::LocalDbType);
}

void DbQuery::addNvrInfo(const QString &ip, const QString &name, const QString &type)
{
    QString sql = "insert into NvrInfo(NvrID,NvrName,NvrType,NvrIP,UserName,UserPwd,NvrEnable) values";
    QStringList values;
    values << QString::number(DbHelper::getMaxID("NvrInfo", "NvrID") + 1);
    values << name;
    //后期这里可以根据搜索那边传过来的NVR类型进行对应查找
    values << type;
    values << ip;
    values << "admin";
    values << "123456";
    values << "启用";

    sql = QString("%1('%2')").arg(sql).arg(values.join("','"));
    DbHelper::execSql(sql);
}

void DbQuery::loadIpcInfo()
{
    DbData::IpcInfo_Count = 0;
    DbData::IpcInfo_IpcID.clear();
    DbData::IpcInfo_IpcName.clear();
    DbData::IpcInfo_NvrName.clear();
    DbData::IpcInfo_IpcType.clear();
    DbData::IpcInfo_OnvifAddr.clear();
    DbData::IpcInfo_ProfileToken.clear();
    DbData::IpcInfo_VideoSource.clear();
    DbData::IpcInfo_RtspMain.clear();
    DbData::IpcInfo_RtspSub.clear();
    DbData::IpcInfo_MainResolutionRatio.clear();
    DbData::IpcInfo_SubResolutionRatio.clear();
    DbData::IpcInfo_IpcX.clear();
    DbData::IpcInfo_IpcY.clear();
    DbData::IpcInfo_UserName.clear();
    DbData::IpcInfo_UserPwd.clear();
    DbData::IpcInfo_IpcEnable.clear();
    DbData::IpcInfo_IpcOnline.clear();

    QString column1 = "IpcID,IpcName,NvrName,IpcType";
    QString column2 = "OnvifAddr,ProfileToken,VideoSource,RtspMain,RtspSub";
    QString column3 = "MainResolutionRatio,SubResolutionRatio,IpcX,IpcY,UserName,UserPwd,IpcEnable";
    QString sql = QString("select %1,%2,%3 from IpcInfo order by IpcID asc").arg(column1).arg(column2).arg(column3);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int ipcID = query.value(0).toInt();
        QString ipcName = query.value(1).toString();
        QString nvrName = query.value(2).toString();
        QString ipcType = query.value(3).toString();
        QString onvifAddr = query.value(4).toString();
        QString profileToken = query.value(5).toString();
        QString videoSource = query.value(6).toString();
        QString rtspMain = query.value(7).toString();
        QString rtspSub = query.value(8).toString();
        QString mainResolution = query.value(9).toString();
        QString subResolution = query.value(10).toString();
        int ipcX = query.value(11).toInt();
        int ipcY = query.value(12).toInt();
        QString userName = query.value(13).toString();
        QString userPwd = query.value(14).toString();
        QString ipcEnable = query.value(15).toString();
        if (ipcEnable != "启用") {
            continue;
        }

        //如果经纬度坐标为空则默认一个
      /*  if (ipcPosition.isEmpty()) {
            ipcPosition = AppConfig::MapCenter;
        }*/

        DbData::IpcInfo_Count++;
        DbData::IpcInfo_IpcID << ipcID;
        DbData::IpcInfo_IpcName << ipcName;
        DbData::IpcInfo_NvrName << nvrName;
        DbData::IpcInfo_IpcType << ipcType;
        DbData::IpcInfo_OnvifAddr << onvifAddr;
        DbData::IpcInfo_ProfileToken << profileToken;
        DbData::IpcInfo_VideoSource << videoSource;
        DbData::IpcInfo_RtspMain << rtspMain;
        DbData::IpcInfo_RtspSub << rtspSub;
        DbData::IpcInfo_MainResolutionRatio << mainResolution;
        DbData::IpcInfo_SubResolutionRatio << subResolution;
        DbData::IpcInfo_IpcX << ipcX;
        DbData::IpcInfo_IpcY << ipcY;
        DbData::IpcInfo_UserName << userName;
        DbData::IpcInfo_UserPwd << userPwd;
        DbData::IpcInfo_IpcEnable << ipcEnable;
        DbData::IpcInfo_IpcOnline << false;
    }

    //取第一个有背景地图的设备的图片作为默认图片
  /*  if (DbData::IpcInfo_Count > 0) {
        for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
            QString imageName = DbData::IpcInfo_SubResolutionRatio.at(i);
            if (!imageName.contains("无")) {
                AppData::CurrentImage = imageName;
                break;
            }
        }
    }*/

    //可能默认图片不存在则取图片列表中的第一张
   /* if (AppData::MapNames.count() > 0 && !AppData::MapNames.contains(AppData::CurrentImage)) {
        AppData::CurrentImage = AppData::MapNames.at(0);
    }    */
}

void DbQuery::clearIpcInfo()
{
    DbHelper::clearTable("IpcInfo", AppConfig::LocalDbType);
}

void DbQuery::deleteIpcInfo(const QString &nvrName)
{
    QString sql = QString("delete from IpcInfo where NvrName='%1'").arg(nvrName);
    DbHelper::execSql(sql);
}

void DbQuery::deleteIpcInfos(const QString &ids)
{
    //合并成一条语句删除
    QString sql = QString("delete from IpcInfo where IpcID in (%1)").arg(ids);
    DbHelper::execSql(sql);
}

void DbQuery::getIpcInfo(int ipcID, QString &ipcPosition)
{
   /* for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        if (DbData::IpcInfo_IpcID.at(i) == ipcID) {
            ipcPosition = DbData::IpcInfo_MainResolutionRatio.at(i);
            break;
        }
    }*/
}

void DbQuery::getIpcInfo(const QString &rtsp, QString &ipcPosition)
{
    //可能是主码流或者子码流
   /* for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        if (DbData::IpcInfo_RtspMain.at(i) == rtsp || DbData::IpcInfo_RtspSub.at(i) == rtsp) {
            ipcPosition = DbData::IpcInfo_MainResolutionRatio.at(i);
            break;
        }
    }*/
}

int DbQuery::getIpcInfo(double lng, double lat)
{
    int ipcID = -1;
    //地球周长
    double max = 40076 * 1000;
    //double max = std::numeric_limits<double>::max();
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        QString ipcPosition = DbData::IpcInfo_MainResolutionRatio.at(i);
        QStringList list = ipcPosition.split("|");
        double lng2 = list.at(0).toDouble();
        double lat2 = list.at(1).toDouble();

        //比较最近的经纬度
        double distance = MapHelper::getDistance(lng, lat, lng2, lat2);
        if (distance < max) {
            max = distance;
            ipcID = DbData::IpcInfo_IpcID.at(i);
        }
    }

    return ipcID;
}

void DbQuery::updatePosition(DeviceButton *btn)
{
    int x = btn->geometry().x();
    int y = btn->geometry().y();
    QString id = btn->property("ipcID").toString();
    QString sql = QString("update IpcInfo set IpcX='%1',IpcY='%2' where IpcID='%3'").arg(x).arg(y).arg(id);
    DbHelper::execSql(sql);
}

void DbQuery::updatePosition(int id, const QString &position)
{
    QString sql = QString("update IpcInfo set IpcPosition='%1' where IpcID='%2'").arg(position).arg(id);
    DbHelper::execSql(sql);
    DbQuery::loadIpcInfo();
    AppEvent::Instance()->slot_saveIpcInfo();
}

void DbQuery::loadPollInfo()
{
    DbData::PollInfo_Count = 0;
    DbData::PollInfo_PollID.clear();
    DbData::PollInfo_PollGroup.clear();
    DbData::PollInfo_RtspMain.clear();
    DbData::PollInfo_RtspSub.clear();

    QString sql = "select PollID,PollGroup,RtspMain,RtspSub from PollInfo order by PollGroup asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int pollID = query.value(0).toInt();
        QString pollGroup = query.value(1).toString();
        QString rtspMain = query.value(2).toString();
        QString rtspSub = query.value(3).toString();

        DbData::PollInfo_Count++;
        DbData::PollInfo_PollID << pollID;
        DbData::PollInfo_PollGroup << pollGroup;
        DbData::PollInfo_RtspMain << rtspMain;
        DbData::PollInfo_RtspSub << rtspSub;
    }
}

void DbQuery::clearPollInfo()
{
    DbHelper::clearTable("PollInfo", AppConfig::LocalDbType);
}

void DbQuery::deletePollInfo(const QString &rtspAddr)
{
    QString sql = QString("delete from PollInfo where RtspMain='%1'").arg(rtspAddr);
    DbHelper::execSql(sql);
}

void DbQuery::deletePollInfos(const QString &addrs)
{
    QString sql = QString("delete from PollInfo where RtspMain in (%1)").arg(addrs);
    DbHelper::execSql(sql);
}

void DbQuery::loadRecordInfo()
{
    DbData::RecordInfo_Count = 0;
    DbData::RecordInfo_RecordCh.clear();
    DbData::RecordInfo_RecordWeek1.clear();
    DbData::RecordInfo_RecordWeek2.clear();
    DbData::RecordInfo_RecordWeek3.clear();
    DbData::RecordInfo_RecordWeek4.clear();
    DbData::RecordInfo_RecordWeek5.clear();
    DbData::RecordInfo_RecordWeek6.clear();
    DbData::RecordInfo_RecordWeek7.clear();

    QString sql = "select RecordCh,RecordWeek1,RecordWeek2,RecordWeek3,RecordWeek4,RecordWeek5,RecordWeek6,RecordWeek7 from RecordInfo order by RecordCh asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        int recordCh = query.value(0).toInt();
        QString recordWeek1 = query.value(1).toString();
        QString recordWeek2 = query.value(2).toString();
        QString recordWeek3 = query.value(3).toString();
        QString recordWeek4 = query.value(4).toString();
        QString recordWeek5 = query.value(5).toString();
        QString recordWeek6 = query.value(6).toString();
        QString recordWeek7 = query.value(7).toString();

        DbData::RecordInfo_Count++;
        DbData::RecordInfo_RecordCh << recordCh;
        DbData::RecordInfo_RecordWeek1 << recordWeek1;
        DbData::RecordInfo_RecordWeek2 << recordWeek2;
        DbData::RecordInfo_RecordWeek3 << recordWeek3;
        DbData::RecordInfo_RecordWeek4 << recordWeek4;
        DbData::RecordInfo_RecordWeek5 << recordWeek5;
        DbData::RecordInfo_RecordWeek6 << recordWeek6;
        DbData::RecordInfo_RecordWeek7 << recordWeek7;
    }
}

void DbQuery::clearRecordInfo()
{
    DbHelper::clearTable("RecordInfo", AppConfig::LocalDbType);
}

QList<QStringList> DbQuery::getRecordTimes()
{
    //先设置空的值然后从数据库中拿到设置过的
    QList<QStringList> recordTimes;
    for (int i = 0; i < AppConfig::VideoCount; ++i) {
        QStringList day;
        for (int j = 0; j < 7; ++j) {
            QStringList min;
            for (int k = 0; k < 48; ++k) {
                min << "0";
            }
            day << min.join(",");
        }
        recordTimes << day;
    }

    if (!AppConfig::SaveVideo) {
        return recordTimes;
    }

    for (int i = 0; i < DbData::RecordInfo_Count; ++i) {
        QStringList list;
        int ch = DbData::RecordInfo_RecordCh.at(i);
        list << DbData::RecordInfo_RecordWeek1.at(i);
        list << DbData::RecordInfo_RecordWeek2.at(i);
        list << DbData::RecordInfo_RecordWeek3.at(i);
        list << DbData::RecordInfo_RecordWeek4.at(i);
        list << DbData::RecordInfo_RecordWeek5.at(i);
        list << DbData::RecordInfo_RecordWeek6.at(i);
        list << DbData::RecordInfo_RecordWeek7.at(i);
        recordTimes[ch - 1] = list;
    }

    return recordTimes;
}

bool DbQuery::checkDb()
{
    //如果表中的NVR在NVR信息表中不存在则弹出提示
    foreach (QString nvrName, DbData::IpcInfo_NvrName) {
        if (!DbData::NvrInfo_NvrName.contains(nvrName)) {
            QtHelper::showMessageBoxError("摄像机表中的录像机信息不存在, 请重新检查!");
            return false;
        }
    }

    return true;
}
