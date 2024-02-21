#ifndef URLHELPER_H
#define URLHELPER_H

#include "widgethead.h"
#include "qdatetime.h"

//厂家类型
enum CompanyType {
    CompanyType_Normal = 0,     //常规厂家
    CompanyType_HaiKang = 1,    //海康威视
    CompanyType_DaHua = 2,      //大华股份
    CompanyType_YuShi = 3,      //杭州宇视
    CompanyType_Other = 255     //其他厂家
};

//地址参数结构体
struct UrlPara {
    QString deviceIP;           //通信地址
    int devicePort;             //通信端口
    QString userName;           //用户名称
    QString userPwd;            //用户密码

    int channel;                //通道编号
    int streamType;             //码流类型
    QString companyName;        //厂家标识
    CompanyType companyType;    //厂家类型

    int videoType;              //视频类型(0-实时/1-回放)
    QDateTime dateTimeStart;    //开始时间(回放专用)
    QDateTime dateTimeEnd;      //结束时间(回放专用)

    UrlPara() {
        devicePort = 0;
        channel = 0;
        streamType = 0;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const UrlPara &urlPara) {
        QStringList list;
        list << QString("通信地址: %1").arg(urlPara.deviceIP);
        list << QString("通信端口: %1").arg(urlPara.devicePort);
        list << QString("用户名称: %1").arg(urlPara.userName);
        list << QString("用户密码: %1").arg(urlPara.userPwd);

        list << QString("通道编号: %1").arg(urlPara.channel);
        list << QString("码流类型: %1").arg(urlPara.streamType);
        list << QString("厂家标识: %1").arg(urlPara.companyName);
        list << QString("厂家类型: %1").arg(urlPara.companyType);

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
        debug.noquote() << list.join("\n");
#else
        debug << list.join("\n");
#endif
        return debug;
    }
};

class UrlHelper
{
public:
    //各个厂家的实时及回放视频流字符串(通道和码流默认约定从0开始)
    static QString getRtspUrl(const CompanyType &companyType, int channel = 0, int streamType = 0);
    static QString getRtspUrl(const UrlPara &urlPara);

    //去掉默认端口号
    static void removeDefaultPort(QString &url);

    //根据地址获取地址对应的各种信息
    static QString getUrlHost(const QString &url);
    static QString getUrlIP(const QString &url);
    static int getUrlPort(const QString &url);

    static void getUserInfo(const QString &url, QString &userName, QString &userPwd);
    static QString getCompanyName(const QString &url);
    static CompanyType getCompanyType(const QString &url);
    static void getOtherInfo(const QString &url, int &channel, int &streamType);
    static void getUrlPara(const QString &url, UrlPara &urlPara);
};

#endif // URLHELPER_H
