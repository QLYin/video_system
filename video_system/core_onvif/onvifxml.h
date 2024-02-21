#ifndef ONVIFXML_H
#define ONVIFXML_H

#include "qbytearray.h"
#include "qstring.h"

class OnvifXml
{
public:
    //获取用户认证数据
    static QString getUserToken(const QString &userName, const QString &userPwd, qint64 timeOffset);
    //获取通用头部数据
    static QString getHeadData(const QString &userToken);
    //获取通用发送数据
    static QString getSendData(const QString &head, const QString &key, bool xmlns = true);

    //获取搜索数据
    static QByteArray getSearchDeviceXml(int type);
};

#endif // ONVIFXML_H
