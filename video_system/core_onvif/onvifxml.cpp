#include "onvifxml.h"
#include "onvifhelper.h"

#define wsseToken false
QString OnvifXml::getUserToken(const QString &userName, const QString &userPwd, qint64 timeOffset)
{
    //要转成UTC格式的时间 "2019-08-10T03:31:37S"  "2020-10-11T09:24:44.988Z"
    //有些设备需要按照设备上的时间来鉴权(否则会失败)
    QDateTime DateTime = QDateTime::currentDateTime().addSecs(timeOffset).toUTC();
    QByteArray Created = DateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ").toLatin1();

    //指定字符串进行密码加密 LKqI6G/AikKCQrN0zqZFlg==
    QByteArray Nonce = Created.toBase64();
    QByteArray Nonce2 = QByteArray::fromBase64(Nonce);
    QByteArray Password = Nonce2 + Created + userPwd.toLatin1();
    Password = QCryptographicHash::hash(Password, QCryptographicHash::Sha1).toBase64();

    //固定字符串
    QString Enter = "\r\n        ";
    QString Type = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest";
    QString EncodingType = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary";
    QString xmlns = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd";

    QStringList list;
    if (wsseToken) {
        list << QString("%1<wsse:Username>%2</wsse:Username>").arg(Enter).arg(userName);
        list << QString("%1<wsse:Password Type=\"%2\">%3</wsse:Password>").arg(Enter).arg(Type).arg(QString(Password));
        list << QString("%1<wsse:Nonce>%2</wsse:Nonce>").arg(Enter).arg(QString(Nonce));
        list << QString("%1<wsu:Created>%2</wsu:Created>").arg(Enter).arg(QString(Created));
    } else {
        list << QString("%1<Username>%2</Username>").arg(Enter).arg(userName);
        list << QString("%1<Password Type=\"%2\">%3</Password>").arg(Enter).arg(Type).arg(QString(Password));
        list << QString("%1<Nonce EncodingType=\"%2\">%3</Nonce>").arg(Enter).arg(EncodingType).arg(QString(Nonce));
        list << QString("%1<Created xmlns=\"%2\">%3</Created>").arg(Enter).arg(xmlns).arg(QString(Created));
    }

    list << QString("%1").arg("\r\n      ");
    return list.join("");
}

QString OnvifXml::getHeadData(const QString &userToken)
{
    QString wsse = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd";
    QString wsu = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd";

    QStringList list;
    if (wsseToken) {
        list << QString("  <s:Header>");
        list << QString("    <wsse:Security s:mustUnderstand=\"1\" xmlns:wsse=\"%1\" xmlns:wsu=\"%2\">").arg(wsse).arg(wsu);
        list << QString("      <wsse:UsernameToken>%1</wsse:UsernameToken>").arg(userToken);
        list << QString("    </wsse:Security>");
        list << QString("  </s:Header>");
    } else {
        list << QString("  <s:Header>");
        list << QString("    <Security s:mustUnderstand=\"1\" xmlns=\"%1\">").arg(wsse);
        list << QString("      <UsernameToken>%1</UsernameToken>").arg(userToken);
        list << QString("    </Security>");
        list << QString("  </s:Header>");
    }

    return list.join("\r\n");
}

QString OnvifXml::getSendData(const QString &head, const QString &key, bool xmlns)
{
    QString soap = "http://www.w3.org/2003/05/soap-envelope";
    QString trt = "http://www.onvif.org/ver10/media/wsdl";
    QString tds = "http://www.onvif.org/ver10/device/wsdl";
    QString xsi = "http://www.w3.org/2001/XMLSchema-instance";
    QString xsd = "http://www.w3.org/2001/XMLSchema";

    QStringList list;
    list << QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    list << QString("<s:Envelope xmlns:s=\"%1\" xmlns:trt=\"%2\">").arg(soap).arg(trt);
    list << head;

    list << QString("  <s:Body xmlns:xsi=\"%1\" xmlns:xsd=\"%2\">").arg(xsi).arg(xsd);
    if (xmlns) {
        list << QString("    <%1 xmlns=\"%2\"/>").arg(key).arg(tds);
    } else {
        list << QString("%1").arg(key);
    }

    list << QString("  </s:Body>");
    list << QString("</s:Envelope>");
    return list.join("\r\n");
}

QByteArray OnvifXml::getSearchDeviceXml(int type)
{
    QString wsdl = "http://www.onvif.org/ver10/network/wsdl";
    QString soap = "http://www.w3.org/2003/05/soap-envelope";
    QString addressing = "http://schemas.xmlsoap.org/ws/2004/08/addressing";
    QString discovery = "http://schemas.xmlsoap.org/ws/2005/04/discovery";
    QString probe = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
    QString xsi = "http://www.w3.org/2001/XMLSchema-instance";
    QString xsd = "http://www.w3.org/2001/XMLSchema";

    QStringList list;
    list << QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    list << QString("<Envelope xmlns:dn=\"%1\" xmlns=\"%2\">").arg(wsdl).arg(soap);
    list << QString("  <Header>");
    //这里要填入uuid
    list << QString("    <wsa:MessageID xmlns:wsa=\"%1\">uuid:%2</wsa:MessageID>").arg(addressing).arg(OnvifHelper::getUuid());
    list << QString("    <wsa:To xmlns:wsa=\"%1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>").arg(addressing);
    list << QString("    <wsa:Action xmlns:wsa=\"%1\">%2</wsa:Action>").arg(addressing).arg(probe);
    list << QString("  </Header>");
    list << QString("  <Body>");
    list << QString("    <Probe xmlns:xsi=\"%1\" xmlns:xsd=\"%2\" xmlns=\"%3\">").arg(xsi).arg(xsd).arg(discovery);
    list << QString("      <Types>dn:NetworkVideoTransmitter</Types>");
    list << QString("      <Scopes />");
    list << QString("    </Probe>");
    list << QString("  </Body>");
    list << QString("</Envelope>");
    return list.join("\r\n").toUtf8();
}
