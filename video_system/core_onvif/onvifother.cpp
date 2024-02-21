#include "onvifother.h"

OnvifOther::OnvifOther(QObject *parent) : QObject(parent)
{
    device = (OnvifDevice *)parent;
}

QString OnvifOther::getResult(const QString &value)
{
    return value.split(OnvifValueSplit).last();
}

int OnvifOther::getResult2(const QString &value)
{
    //这里为什么是 toDouble 而不是 toInt, 因为发现有些有小数点toInt转换失败
    return value.split(OnvifValueSplit).last().toDouble();
}

QString OnvifOther::getResult3(const QString &value, const QString &key)
{
    //tt:Name:etho  取出带前缀的值
    QString result = value;
    result.replace(QString("%1%2").arg(key).arg(OnvifValueSplit), "");
    return result;
}

QString OnvifOther::writeData(const QString &key, const QString &value, const QString &flag,
                              bool xmlns, bool value4)
{
    if (key.isEmpty() || value.isEmpty()) {
        return QString();
    }

    //传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifXml::getSendData(device->getHeadData(), key, xmlns);
    QByteArray dataSend = file.toUtf8();

    //请求地址一般是onvif地址,有些早期的设备可能不兼容需要填媒体地址
    QString postUrl = device->onvifAddr;
    if (flag.contains("图片")) {
        postUrl = device->imageUrl;
    } else if (flag.contains("OSD")) {
        postUrl = device->mediaUrl;
    }

    //发送网络请求
    //最后参数表示超时时间,一般请求都是很快的,除非对方不在线则卡很久,需要设置下超时时间
    QNetworkReply *reply = device->request->post(postUrl, dataSend, OnvifRequest::timeout + 500);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    device->checkData(reply, dataReceive, flag);

    OnvifQuery query;
    if (!query.setData(dataReceive)) {
        return QString();
    }

    //可能有多个关键字需要获取
    QStringList results;
    QStringList list = value.split("|");
    foreach (QString str, list) {
        QString result;
        if (value4) {
            result = OnvifHelper::getValue4(dataReceive, str);
        } else {
            result = OnvifHelper::getValue3(dataReceive, str);
        }

        if (result != "-1") {
            results << QString("%1%2%3").arg(str).arg(OnvifValueSplit).arg(result);
        }
    }

    //有些设备节点返回是 <SetxxxResponse/> 而不是 <SetxxxResponse></SetxxxResponse>
    if (results.count() == 0) {
        results << QString("%1%2").arg(list.first()).arg(OnvifValueSplit);
    }

    return results.join(OnvifResultSplit);
}

QString OnvifOther::systemReboot()
{
    return writeData("SystemReboot", "Message", "重启设备");
}

QString OnvifOther::hardReset()
{
    return writeData("HardReset", "Message", "复位硬件");
}

QString OnvifOther::getZeroConfig()
{
    return writeData("GetZeroConfiguration", "InterfaceToken|Enabled|Addresses", "获取自动配置参数");
}

QString OnvifOther::getNtp()
{
    return writeData("GetNTP", "DNSname|IPv4Address", "获取NTP");
}

bool OnvifOther::setNtp(const QString &ntp, bool isIp)
{
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";
    QString schema = "http://www.onvif.org/ver10/schema";

    QStringList list;
    list << QString("    <SetNTP xmlns=\"%1\">").arg(wsdl);
    list << QString("      <FromDHCP>false</FromDHCP>");
    list << QString("      <NTPManual>");
    list << QString("        <Type xmlns=\"%1\">%2</Type>").arg(schema).arg(isIp ? "IPv4" : "DNS");
    list << QString("        <%3 xmlns=\"%1\">%2</%3>").arg(schema).arg(ntp).arg(isIp ? "IPv4Address" : "DNSname");
    list << QString("      </NTPManual>");
    list << QString("    </SetNTP>");

    QString result = writeData(list.join("\r\n"), "SetNTPResponse", "设置NTP", false);
    return result.contains("SetNTPResponse");
}

QString OnvifOther::getHostName()
{
    return writeData("GetHostname", "Name", "获取主机名称");
}

bool OnvifOther::setHostName(const QString &hostname)
{
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";

    QStringList list;
    list << QString("    <SetHostname xmlns=\"%1\">").arg(wsdl);
    list << QString("      <FromDHCP>false</FromDHCP>");
    list << QString("      <Name>%1</Name>").arg(hostname);
    list << QString("    </SetHostname>");

    QString result = writeData(list.join("\r\n"), "SetHostnameResponse", "设置主机名称", false);
    return result.contains("SetHostnameResponse");
}

QString OnvifOther::getGateway()
{
    return writeData("GetNetworkDefaultGateway", "IPv4Address", "获取网关地址");
}

bool OnvifOther::setGateway(const QString &gateway)
{
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";

    QStringList list;
    list << QString("    <SetNetworkDefaultGateway xmlns=\"%1\">").arg(wsdl);
    list << QString("      <IPv4Address>%1</IPv4Address>").arg(gateway);
    //list << QString("      <IPv6Address>%1</IPv6Address>").arg("::");
    list << QString("    </SetNetworkDefaultGateway>");

    QString result = writeData(list.join("\r\n"), "SetNetworkDefaultGatewayResponse", "设置网关地址", false);
    return result.contains("SetNetworkDefaultGatewayResponse");
}

QString OnvifOther::getDns()
{
    return writeData("GetDNS", "IPv4Address|SearchDomain|FromDHCP", "获取DNS地址");
}

bool OnvifOther::setDns(const QString &dns, const QString &dhcp)
{
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";
    QString schema = "http://www.onvif.org/ver10/schema";

    QStringList list;
    list << QString("    <SetDNS xmlns=\"%1\">").arg(wsdl);
    list << QString("      <FromDHCP>%1</FromDHCP>").arg(dhcp);
    list << QString("      <DNSManual>");
    list << QString("        <Type xmlns=\"%1\">%2</Type>").arg(schema).arg("IPv4");
    list << QString("        <IPv4Address xmlns=\"%1\">%2</IPv4Address>").arg(schema).arg(dns);
    list << QString("      </DNSManual>");
    list << QString("    </SetDNS>");

    QString result = writeData(list.join("\r\n"), "SetDNSResponse", "设置DNS地址", false);
    return result.contains("SetDNSResponse");
}

OnvifNetConfig OnvifOther::getInterface()
{
    OnvifNetConfig netConfig;
    QString result = writeData("GetNetworkInterfaces", "tt:Name|tt:HwAddress|tt:Address|tt:DHCP", "获取网卡信息", true, true);
    QStringList list = result.split(OnvifResultSplit);
    if (list.count() == 4) {
        netConfig.name = getResult3(list.at(0), "tt:Name");
        netConfig.hwAddress = getResult3(list.at(1), "tt:HwAddress");
        netConfig.ipAddress = getResult3(list.at(2), "tt:Address");
        netConfig.ipDhcp = getResult3(list.at(3), "tt:DHCP");
    }

    return netConfig;
}

bool OnvifOther::setInterface(const OnvifNetConfig &netConfig)
{
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";
    QString schema = "http://www.onvif.org/ver10/schema";

    QStringList list;
    list << QString("    <SetNetworkInterfaces xmlns=\"%1\">").arg(wsdl);
    list << QString("      <InterfaceToken>%1</InterfaceToken>").arg(netConfig.name);
    list << QString("      <NetworkInterface>");
    list << QString("        <Enabled xmlns=\"%1\">true</Enabled>").arg(schema);
    list << QString("        <MTU xmlns=\"%1\">1500</MTU>").arg(schema);
    list << QString("        <IPv4 xmlns=\"%1\">").arg(schema);
    list << QString("          <Enabled>true</Enabled>");
    list << QString("          <Manual>");
    list << QString("            <Address>%1</Address>").arg(netConfig.ipAddress);
    list << QString("            <PrefixLength>%1</PrefixLength>").arg(OnvifHelper::ipv4ToPrefixLength(netConfig.mask));
    list << QString("          </Manual>");
    list << QString("          <DHCP>%1</DHCP>").arg(netConfig.ipDhcp);
    list << QString("        </IPv4>");
    list << QString("      </NetworkInterface>");
    list << QString("    </SetNetworkInterfaces>");

    QString result = writeData(list.join("\r\n"), "SetNetworkInterfacesResponse", "设置网卡信息", false);
    return result.contains("SetNetworkInterfacesResponse");
}

QString OnvifOther::getProtocol()
{
    return writeData("GetNetworkProtocols", "Name|Port", "获取支持协议");
}

bool OnvifOther::setProtocol()
{
    return true;
}

OnvifNetConfig OnvifOther::getNetConfig()
{
    OnvifNetConfig netConfig;

    //第一步: 获取主机地址
    QString result = writeData("GetHostname", "Name", "获取主机名称");
    QStringList list = result.split(OnvifResultSplit);
    if (list.count() == 1) {
        netConfig.hostName = getResult(list.at(0));
    }

    //第二步: 获取网关地址
    result = writeData("GetNetworkDefaultGateway", "IPv4Address", "获取网关地址");
    list = result.split(OnvifResultSplit);
    if (list.count() == 1) {
        netConfig.gateway = getResult(list.at(0));
    }

    //第三步: 获取DNS
    result = writeData("GetDNS", "IPv4Address|SearchDomain|FromDHCP", "获取NDS地址");
    list = result.split(OnvifResultSplit);
    if (list.count() >= 2) {
        netConfig.dns = getResult(list.at(0));
        netConfig.dnsDhcp = getResult(list.at(1));
    }

    //第四步: 获取网卡信息
    result = writeData("GetNetworkInterfaces", "tt:Name|tt:HwAddress|tt:Address|tt:DHCP|tt:PrefixLength", "获取网卡信息", true, true);
    list = result.split(OnvifResultSplit);
    if (list.count() == 5) {
        netConfig.name = getResult3(list.at(0), "tt:Name");
        netConfig.hwAddress = getResult3(list.at(1), "tt:HwAddress");
        netConfig.ipAddress = getResult3(list.at(2), "tt:Address");
        netConfig.ipDhcp = getResult3(list.at(3), "tt:DHCP");
        QString prefixLength = getResult3(list.at(4), "tt:PrefixLength");
        netConfig.mask = OnvifHelper::prefixLengthToIpv4(prefixLength.toInt());
    }

    return netConfig;
}

bool OnvifOther::setNetConfig(const OnvifNetConfig &netConfig)
{
    //过滤空值
    if (netConfig.hostName.isEmpty()) {
        return false;
    } else if (netConfig.ipAddress.isEmpty()) {
        return false;
    } else if (netConfig.mask.isEmpty()) {
        return false;
    } else if (netConfig.gateway.isEmpty()) {
        return false;
    }

    //第一步: 设置主机地址
    bool ok1 = setHostName(netConfig.hostName);
    //第二步: 设置网关地址
    bool ok2 = setGateway(netConfig.gateway);
    //第三步: 设置DNS
    bool ok3 = setDns(netConfig.dns, netConfig.dnsDhcp);
    //第四步: 设置网卡信息
    bool ok4 = setInterface(netConfig);
    return ok4;
}

QString OnvifOther::getDateTime()
{
    QString result = writeData("GetSystemDateAndTime", "tt:Year|tt:Month|tt:Day|tt:Hour|tt:Minute|tt:Second|tt:TZ", "获取设备时间", true, true);
    QStringList list = result.split(OnvifResultSplit);
    int count = list.count();
    if (count < 6) {
        return result;
    }

    QString year = getResult(list.at(0));
    QString month = getResult(list.at(1));
    QString day = getResult(list.at(2));
    QString hour = getResult(list.at(3));
    QString min = getResult(list.at(4));
    QString sec = getResult(list.at(5));

    //计算时区并赋值(有些小厂设备居然没有时区节点)
    QString timezone = device->timezone;
    if (count == 7) {
        timezone = list.at(6);
        timezone = timezone.mid(6, timezone.length() - 6);
        device->timezone = timezone;
    }

    //将日期根据时区进行运算
    QString str = QString("%1-%2-%3 %4:%5:%6").arg(year).arg(month).arg(day).arg(hour).arg(min).arg(sec);
    QDateTime dt = QDateTime::fromString(str, "yyyy-M-d h:m:s");
    if (!device->timezone.contains("GMT-08")) {
        //必须是浮点数/有些时区是半小时的
        float zone = 8;
        //获取当前系统所在时区
#if (QT_VERSION >= QT_VERSION_CHECK(5,2,0))
        QDateTime now = QDateTime::currentDateTime();
        zone = (float)now.offsetFromUtc() / (60 * 60);
        //夏令时要减去1小时
        if (now.isDaylightTime()) {
            zone -= 1;
        }
#endif
        dt = dt.addSecs(zone * 60 * 60);
    }

    //2023-05-22 新增时间差值计算(有些设备需要用设备的时间去鉴权)
    device->timeOffset = QDateTime::currentDateTime().secsTo(dt);

    //不足两位补零
    list = dt.toString("yyyy-M-d-h-m-s").split("-");
    result = QString("%1-%2-%3 %4:%5:%6 %7").arg(list.at(0)).arg(list.at(1), 2, '0').arg(list.at(2), 2, '0')
             .arg(list.at(3), 2, '0').arg(list.at(4), 2, '0').arg(list.at(5), 2, '0').arg(timezone);
    return result;
}

bool OnvifOther::setDateTime(const QDateTime &datetime, bool ntp)
{
    QStringList temp = datetime.toString("yyyy-M-d-h-m-s").split("-");
    QString wsdl = "http://www.onvif.org/ver10/device/wsdl";
    QString schema = "http://www.onvif.org/ver10/schema";

    QStringList list;
    list << QString("    <SetSystemDateAndTime xmlns=\"%1\">").arg(wsdl);
    list << QString("      <DateTimeType>%1</DateTimeType>").arg(ntp ? "NTP" : "Manual");
    list << QString("      <DaylightSavings>%1</DaylightSavings>").arg("false");
    list << QString("      <TimeZone>");
    list << QString("        <TZ xmlns=\"%1\">%2</TZ>").arg(schema).arg(ntp ? device->timezone : "CST-8");
    list << QString("      </TimeZone>");

    if (!ntp) {
        list << QString("      <UTCDateTime>");
        list << QString("        <Date xmlns=\"%1\">").arg(schema);
        list << QString("          <Year>%1</Year>").arg(temp.at(0));
        list << QString("          <Month>%1</Month>").arg(temp.at(1));
        list << QString("          <Day>%1</Day>").arg(temp.at(2));
        list << QString("        </Date>");
        list << QString("        <Time xmlns=\"%1\">").arg(schema);
        list << QString("          <Hour>%1</Hour>").arg(temp.at(3));
        list << QString("          <Minute>%1</Minute>").arg(temp.at(4));
        list << QString("          <Second>%1</Second>").arg(temp.at(5));
        list << QString("        </Time>");
        list << QString("      </UTCDateTime>");
    }

    list << QString("    </SetSystemDateAndTime>");

    QString result = writeData(list.join("\r\n"), "SetSystemDateAndTimeResponse", "设置设备时间", false);
    return result.contains("SetSystemDateAndTimeResponse");
}

bool OnvifOther::setImageSetting(const QString &videoSource, const OnvifImageSetting &imageSetting)
{
    QString wsdl = "http://www.onvif.org/ver20/imaging/wsdl";
    QString schema = "http://www.onvif.org/ver10/schema";

    //下面的部分参数值是写死的后期可以做成参数传入
    QStringList list;
    list << QString("    <SetImagingSettings xmlns=\"%1\">").arg(wsdl);
    list << QString("      <VideoSourceToken>%1</VideoSourceToken>").arg(videoSource);
    list << QString("      <ImagingSettings>");

    //图片参数(明亮度/饱和度/对比度/尖锐度)
    list << QString("        <Brightness xmlns=\"%1\">%2</Brightness>").arg(schema).arg(imageSetting.brightness);
    list << QString("        <ColorSaturation xmlns=\"%1\">%2</ColorSaturation>").arg(schema).arg(imageSetting.colorSaturation);
    list << QString("        <Contrast xmlns=\"%1\">%2</Contrast>").arg(schema).arg(imageSetting.contrast);
    list << QString("        <Sharpness xmlns=\"%1\">%2</Sharpness>").arg(schema).arg(imageSetting.sharpness);
#if 0
    //图像背光补偿
    list << QString("        <BacklightCompensation xmlns=\"%1\">").arg(schema);
    list << QString("          <Mode>OFF</Mode>");
    list << QString("        </BacklightCompensation>");

    //滤镜
    list << QString("        <Exposure xmlns=\"%1\">").arg(schema);
    list << QString("          <Mode>AUTO</Mode>");
    list << QString("          <MinExposureTime>10</MinExposureTime>");
    list << QString("          <MaxExposureTime>40000</MaxExposureTime>");
    list << QString("          <MinGain>0</MinGain>");
    list << QString("          <MaxGain>100</MaxGain>");
    list << QString("          <MinIris>-22</MinIris>");
    list << QString("          <MaxIris>0</MaxIris>");
    list << QString("        </Exposure>");

    //聚焦策略
    list << QString("        <Focus xmlns=\"%1\">").arg(schema);
    list << QString("          <AutoFocusMode>MANUAL</AutoFocusMode>");
    list << QString("          <DefaultSpeed>1</DefaultSpeed>");
    list << QString("        </Focus>");

    //红外切换策略
    list << QString("        <IrCutFilter xmlns=\"%1\">AUTO</IrCutFilter>").arg(schema);
    //宽动态范围
    list << QString("        <WideDynamicRange xmlns=\"%1\"><Mode>OFF</Mode></WideDynamicRange>").arg(schema);
    //白平衡
    list << QString("        <WhiteBalance xmlns=\"%1\"><Mode>AUTO</Mode></WhiteBalance>").arg(schema);
    //持久化
    //list << QString("        <ForcePersistence>true</ForcePersistence>");

    //拓展属性
    list << QString("        <Extension>");
    list << QString("          <Extension>");
    list << QString("            <Extension>");
    list << QString("              <Defogging>");
    list << QString("                <Mode>OFF</Mode>");
    list << QString("              </Defogging>");
    list << QString("              <NoiseReduction>");
    list << QString("                <Level>0.500000</Level>");
    list << QString("              </NoiseReduction>");
    list << QString("            </Extension>");
    list << QString("          </Extension>");
    list << QString("        </Extension>");
#endif
    list << QString("      </ImagingSettings>");
    list << QString("      <ForcePersistence>true</ForcePersistence>");
    list << QString("    </SetImagingSettings>");

    QString result = writeData(list.join("\r\n"), "SetImagingSettingsResponse", "设置图片参数", false);
    return result.contains("SetImagingSettingsResponse");
}

bool OnvifOther::setOrCreateOsd(const QSize &videoSize, const OnvifOsdInfo &osd, bool create)
{
    if (osd.token.isEmpty() || osd.config.isEmpty()) {
        return false;
    }

    QString wsdl = "http://www.onvif.org/ver10/media/wsdl";
    QString flag = create ? "CreateOSD" : "SetOSD";

    QStringList list;
    list << QString("    <%1 xmlns=\"%2\">").arg(flag).arg(wsdl);
    list << QString("      <OSD token=\"%1\">").arg(osd.token);
    list << QString("        <VideoSourceConfigurationToken>%1</VideoSourceConfigurationToken>").arg(osd.config);
    list << QString("        <Type>Text</Type>");

    list << QString("        <Position>");
    list << QString("          <Type>%1</Type>").arg(osd.positionType);
    //需要将屏幕坐标转换成OSD坐标
    if (osd.positionType == "Custom") {
        QPointF pos = OnvifHelper::screenPosToOsdPos(videoSize.width(), videoSize.height(), osd.position.x(), osd.position.y());
        list << QString("          <Pos x=\"%1\" y=\"%2\"/>").arg(pos.x()).arg(pos.y());
    }
    list << QString("        </Position>");

    list << QString("        <TextString>");
    list << QString("          <Type>%1</Type>").arg(osd.textType);
    list << QString("          <FontSize>%1</FontSize>").arg(osd.fontSize);

    //可选是日期时间或者文本
    if (osd.textType == "DateAndTime") {
        list << QString("          <DateFormat>%1</DateFormat>").arg(osd.dateFormat);
        list << QString("          <TimeFormat>%1</TimeFormat>").arg(osd.timeFormat);
    } else if (osd.textType == "Plain") {
        list << QString("          <PlainText>%1</PlainText>").arg(osd.text);
    }

    list << QString("        </TextString>");
    list << QString("      </OSD>");
    list << QString("    </%1>").arg(flag);

    QString name = QString("%1Response").arg(flag);
    QString tip = create ? "创建OSD" : "设置OSD";
    QString result = writeData(list.join("\r\n"), name, tip, false);
    return result.contains(name);
}

bool OnvifOther::setOsd(const QSize &videoSize, const OnvifOsdInfo &osd)
{
    return setOrCreateOsd(videoSize, osd, false);
}

bool OnvifOther::createOsd(const QSize &videoSize, const OnvifOsdInfo &osd)
{
    return setOrCreateOsd(videoSize, osd, true);
}
