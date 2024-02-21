#include "onvifquery.h"

OnvifQuery::OnvifQuery(QObject *parent) : QObject(parent)
{
}

bool OnvifQuery::setData(const QByteArray &data, const QString &fileName)
{
    //文件名不为空则从文件读取
    this->fileName = fileName;
    if (fileName.isEmpty()) {
        if (data.isEmpty()) {
            return false;
        }
        buffer = data;
    } else {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly)) {
            return false;
        }

        buffer = file.readAll();
        file.close();
        if (buffer.length() <= 0) {
            return false;
        }
    }

    //从源头过滤html转义字符
    buffer.replace("%20", " ");
    if (!doc.setContent(buffer)) {
        qDebug() << TIMEMS << "数据格式错误";
        return false;
    }

    QDomElement element = doc.documentElement();
    QDomNamedNodeMap nodeMap = element.attributes();
    int count = nodeMap.count();
    QHash<QString, QString> names;

    //循环取出属性和值
    for (int i = 0; i < count; ++i) {
        QString nodeName = nodeMap.item(i).nodeName();
        QString nodeValue = element.attribute(nodeName);
        QStringList list = nodeName.split(':', SkipEmptyPartsx);
        if (list.length() == 2) {
            nodeName = list.at(1);
        }

        names.insert(nodeName, nodeValue);
    }

    wsdlAddr.discovery = names.key("http://schemas.xmlsoap.org/ws/2005/04/discovery", "");
    wsdlAddr.schema = names.key("http://www.onvif.org/ver10/schema", "");
    wsdlAddr.addressing = names.key("http://www.w3.org/2005/08/addressing", "");
    wsdlAddr.wsnt = names.key("http://docs.oasis-open.org/wsn/b-2", "");

    wsdlAddr.wsdlDevice = names.key("http://www.onvif.org/ver10/device/wsdl", "");
    wsdlAddr.wsdlMedia = names.key("http://www.onvif.org/ver10/media/wsdl", "");
    wsdlAddr.wsdlEvents = names.key("http://www.onvif.org/ver10/events/wsdl", "");
    wsdlAddr.wsdlPtz = names.key("http://www.onvif.org/ver20/ptz/wsdl", "");
    return true;
}

void OnvifQuery::getSearchInfo(OnvifDeviceInfo &deviceInfo, const QString &ip)
{
    //<tns:XAddrs>http://192.168.0.15:81/onvif/device_service</tns:XAddrs>
    //<tns:Scopes>包括下面的一堆信息</tns:Scopes>
    //onvif://www.onvif.org/Profile/Streaming
    //onvif://www.onvif.org/type/video_encoder
    //onvif://www.onvif.org/type/ptz
    //onvif://www.onvif.org/type/audio_encoder
    //onvif://www.onvif.org/location/
    //onvif://www.onvif.org/name/IMCP
    //onvif://www.onvif.org/macaddr/48ea631bae10
    //onvif://www.onvif.org/MAC/bc:ba:c2:e3:3c:ae
    //onvif://www.onvif.org/version/IPC_2401DE_2421DE_34X1DE_54X1DE_5421DH_24X1SCF-B5265P13
    //onvif://www.onvif.org/serial/210235C1MCA158001067
    //onvif://www.onvif.org/hardware/HIC5421S-C
    //onvif://www.onvif.org/type/IPC
    //onvif://www.onvif.org/register_status/offline
    //onvif://www.onvif.org/register_server/0.0.0.0:5060
    //onvif://www.onvif.org/regist_id/1B-AE-10

    QDomNodeList nodeXAddrs = doc.elementsByTagName(QString("%1:XAddrs").arg(wsdlAddr.discovery));
    if (nodeXAddrs.count() == 1) {
        QString addr = nodeXAddrs.at(0).toElement().text();
        //过滤下IPV6地址 http://192.168.1.64/onvif/device_service http://[fe80::9a8b:aff:fe6e:867c]/onvif/device_service
        //发现还有串数据的要过滤 http://192.168.10.152/onvif/device_service http://192.168.10.172/onvif/device_service http://[fe80::9a8b:aff:fe4a:ad]/onvif/device_service
        QStringList addrs = addr.split(" ");
        if (ip.isEmpty()) {
            addr = addrs.first();
        } else {
            //ip不为空则说明指定设备搜索
            foreach (QString s, addrs) {
                if (s.contains("http://[")) {
                    continue;
                }
                if (s.contains(ip)) {
                    addr = s;
                    break;
                }
            }
        }

        //可能还有非法的onvif地址,如果addr地址不包含IP地址则不用继续
        if (!OnvifHelper::isIP(OnvifHelper::getUrlIP(addr))) {
            return;
        }

        deviceInfo.onvifAddr = addr;
        deviceInfo.deviceIp = OnvifHelper::getUrlIP(addr);
    }

    QDomNodeList nodeScopes = doc.elementsByTagName(QString("%1:Scopes").arg(wsdlAddr.discovery));
    if (nodeScopes.count() > 0) {
        QString scopes = nodeScopes.at(0).toElement().text();
        QStringList list = scopes.split(" ");
        //可以自行增加其他设备信息
        foreach (QString str, list) {
            QStringList l = str.split("/");
            if (l.contains("name")) {
                deviceInfo.name = l.last();
            } else if (l.contains("location")) {
                deviceInfo.location = l.last();
            } else if (l.contains("hardware")) {
                deviceInfo.hardware = l.last();
            } else if (l.contains("macaddr")) {
                deviceInfo.macaddr = l.last();
            } else if (l.contains("MAC")) {
                deviceInfo.macaddr = l.last();
            }
        }
    }
}

void OnvifQuery::getDeviceInfo(OnvifDeviceInfo &deviceInfo)
{
    //<tds:Manufacturer>ONVIF_ICAMERA</tds:Manufacturer>
    //<tds:Model>MSJ10_AF</tds:Model>
    //<tds:FirmwareVersion>V3.0.2.9 build 2020-12-22 16:27:00</tds:FirmwareVersion>
    //<tds:SerialNumber>EF000000008B9B50</tds:SerialNumber>
    //<tds:HardwareId>1419d68a-1dd2-11b2-a105-F000008B9B50</tds:HardwareId>

    QDomNodeList nodeManufacturer = doc.elementsByTagName(QString("%1:Manufacturer").arg(wsdlAddr.wsdlDevice));
    if (nodeManufacturer.count() == 1) {
        deviceInfo.manufacturer = nodeManufacturer.at(0).toElement().text();
    }

    QDomNodeList nodeModel = doc.elementsByTagName(QString("%1:Model").arg(wsdlAddr.wsdlDevice));
    if (nodeModel.count() == 1) {
        deviceInfo.model = nodeModel.at(0).toElement().text();
    }

    QDomNodeList nodeFirmwareVersion = doc.elementsByTagName(QString("%1:FirmwareVersion").arg(wsdlAddr.wsdlDevice));
    if (nodeFirmwareVersion.count() == 1) {
        deviceInfo.firmwareVersion = nodeFirmwareVersion.at(0).toElement().text();
    }

    QDomNodeList nodeSerialNumber = doc.elementsByTagName(QString("%1:SerialNumber").arg(wsdlAddr.wsdlDevice));
    if (nodeSerialNumber.count() == 1) {
        deviceInfo.serialNumber = nodeSerialNumber.at(0).toElement().text();
    }

    QDomNodeList nodeHardwareId = doc.elementsByTagName(QString("%1:HardwareId").arg(wsdlAddr.wsdlDevice));
    if (nodeHardwareId.count() == 1) {
        deviceInfo.hardwareId = nodeHardwareId.at(0).toElement().text();
    }
}

void OnvifQuery::getScopes(OnvifDeviceInfo &deviceInfo)
{
    QDomNodeList nodeScopes = doc.elementsByTagName(QString("%1:Scopes").arg(wsdlAddr.wsdlDevice));
    int count = nodeScopes.count();
    for (int i = 0; i < count; ++i) {
        QString scopes = nodeScopes.at(i).toElement().text();
        QStringList list = scopes.split("/");
        if (list.count() >= 5) {
            QString key = list.at(3);
            QString value = list.last();
            if (key == "name") {
                deviceInfo.name = value;
            } else if (key == "location") {
                deviceInfo.location = value;
            } else if (key == "hardware") {
                deviceInfo.hardware = value;
            }
        }
    }
}

void OnvifQuery::checkAddr(const QString &addrPort, QString &addr, bool checkOnvif)
{
    //如果传入的文件名称说明当前是测试数据则不处理
    if (!fileName.isEmpty()) {
        return;
    }

    QStringList list = addr.split("/");
    if (addr.isEmpty() || list.count() <= 3) {
        return;
    }

    //将内网的地址换成外网的(新增设备映射到外网后返回的数据中内网地址转换成外网的)
    list[2] = addrPort;

    //将地址补全字符串(测试发现天地伟业的摄像机返回的地址都是没有带onvif字样的需要补上才行)
    //http://192.168.0.160:80/Media 要转成 http://192.168.0.160:80/onvif/Media
    if (checkOnvif && !addr.contains("/onvif/")) {
        list.insert(3, "onvif");
    }

    addr = list.join("/");
}

OnvifHttpAddr OnvifQuery::getServices(const QString &addrPort)
{
    //<tds:Namespace>http://www.onvif.org/ver10/media/wsdl</tds:Namespace>
    //<tds:XAddr>http://192.168.0.160:80/Media</tds:XAddr>
    //<tds:Namespace>http://www.onvif.org/ver20/ptz/wsdl</tds:Namespace>
    //<tds:XAddr>http://192.168.0.160:80/PTZ</tds:XAddr>

    OnvifHttpAddr httpAddr;

    //取出来时刚好按照顺序一一对应
    QDomNodeList nodeNamespace = doc.elementsByTagName(QString("%1:Namespace").arg(wsdlAddr.wsdlDevice));
    QDomNodeList nodeXAddr = doc.elementsByTagName(QString("%1:XAddr").arg(wsdlAddr.wsdlDevice));
    int count1 = nodeNamespace.count();
    int count2 = nodeXAddr.count();
    if (count1 == count2) {
        for (int i = 0; i < count1; ++i) {
            QString name = nodeNamespace.at(i).toElement().text();
            QString addr = nodeXAddr.at(i).toElement().text();
            this->checkAddr(addrPort, addr, true);

            //可以自行增加其他的
            if (name == "http://www.onvif.org/ver10/device/wsdl") {
                httpAddr.addrDevice = addr;
            } else if (name == "http://www.onvif.org/ver10/media/wsdl") {
                httpAddr.addrMedia = addr;
            } else if (name == "http://www.onvif.org/ver20/media/wsdl") {
                httpAddr.addrMedia2 = addr;
            } else if (name == "http://www.onvif.org/ver20/ptz/wsdl") {
                httpAddr.addrPtz = addr;
            } else if (name == "http://www.onvif.org/ver20/imaging/wsdl") {
                httpAddr.addrImaging = addr;
            } else if (name == "http://www.onvif.org/ver10/events/wsdl") {
                httpAddr.addrEvents = addr;
            } else if (name == "http://www.onvif.org/ver20/analytics/wsdl") {
                httpAddr.addrAnalytics = addr;
            }
        }
    }

    return httpAddr;
}

OnvifHttpAddr OnvifQuery::getCapabilities(const QString &addrPort)
{
    //<tt:Media><tt:XAddr>http://192.168.0.160:80/onvif/media</tt:XAddr></tt:Media>
    //<tt:PTZ><tt:XAddr>http://192.168.0.160:80/onvif/ptz</tt:XAddr></tt:PTZ>

    OnvifHttpAddr httpAddr;

    //按照关键字模糊匹配
    QDomNodeList nodeXAddr = doc.elementsByTagName(QString("%1:XAddr").arg(wsdlAddr.schema));
    int count = nodeXAddr.count();
    for (int i = 0; i < count; ++i) {
        QString addr = nodeXAddr.at(i).toElement().text();
        QString addr2 = addr.toLower();
        this->checkAddr(addrPort, addr, true);

        if (addr2.contains("device")) {
            httpAddr.addrDevice = addr;
        } else if (addr2.contains("media")) {
            httpAddr.addrMedia = addr;
        } else if (addr2.contains("ptz")) {
            httpAddr.addrPtz = addr;
        } else if (addr2.contains("image")) {
            httpAddr.addrImaging = addr;
        } else if (addr2.contains("event")) {
            httpAddr.addrEvents = addr;
        } else if (addr2.contains("analy")) {
            httpAddr.addrAnalytics = addr;
        }
    }

    return httpAddr;
}

QList<OnvifProfileInfo> OnvifQuery::getProfiles()
{
    //<trt:Profiles token="MediaProfile000" fixed="true"><tt:Name>MediaProfile_Channel1_MainStream</tt:Name></trt:Profiles>

    QList<OnvifProfileInfo> profiles;
    QDomNodeList nodeProfiles = doc.elementsByTagName(QString("%1:Profiles").arg(wsdlAddr.wsdlMedia));
    int count = nodeProfiles.count();
    for (int i = 0; i < count; ++i) {
        OnvifProfileInfo profile;
        QDomElement element = nodeProfiles.at(i).toElement();
        profile.token = element.attribute("token");
        profile.fixed = element.attribute("fixed");

        QDomNodeList nodeName = element.elementsByTagName(QString("%1:Name").arg(wsdlAddr.schema));
        if (nodeName.count() > 0) {
            profile.name = nodeName.at(0).toElement().text();
        }

        //每个配置文件对应xyz三个每个都有min max
        //<tt:Min>-1.000000</tt:Min>
        //<tt:Max>1.000000</tt:Max>
        QDomNodeList nodeMin = element.elementsByTagName(QString("%1:Min").arg(wsdlAddr.schema));
        QDomNodeList nodeMax = element.elementsByTagName(QString("%1:Max").arg(wsdlAddr.schema));
        int count1 = nodeMin.count();
        int count2 = nodeMax.count();
        if (count1 == 3 && count1 == count2) {
            profile.minX = nodeMin.at(0).toElement().text().toDouble();
            profile.maxX = nodeMax.at(0).toElement().text().toDouble();
            profile.minY = nodeMin.at(1).toElement().text().toDouble();
            profile.maxY = nodeMax.at(1).toElement().text().toDouble();
            profile.minZ = nodeMin.at(2).toElement().text().toDouble();
            profile.maxZ = nodeMax.at(2).toElement().text().toDouble();
        }

        profiles << profile;
    }

    return profiles;
}

QList<OnvifPresetInfo> OnvifQuery::getPresets()
{
    //取到结果 1-255 连续的字符串 有些设备回复的信息不够全可能只有 Name 字段
    //<tptz:Preset token=\"1\"><tt:Name>1</tt:Name><tt:PTZPosition><tt:PanTilt x="0.0" y="0.0"/><tt:Zoom x="0.0"/></tt:PTZPosition></tptz:Preset>
    //<tptz:Preset token=\"2\"><tt:Name>2</tt:Name><tt:PTZPosition><tt:PanTilt x="0.0" y="0.0"/><tt:Zoom x="0.0"/></tt:PTZPosition></tptz:Preset>
    //<tptz:Preset token=\"255\"><tt:Name>255</tt:Name><tt:PTZPosition><tt:PanTilt x="0.0" y="0.0"/><tt:Zoom x="0.0"/></tt:PTZPosition></tptz:Preset>

    QList<OnvifPresetInfo> presets;
    QDomNodeList nodePresets = doc.elementsByTagName(QString("%1:Preset").arg(wsdlAddr.wsdlPtz));
    int count = nodePresets.count();
    for (int i = 0; i < count; ++i) {
        OnvifPresetInfo preset;
        QDomElement element = nodePresets.at(i).toElement();
        preset.token = element.attribute("token");

        QDomNodeList nodeName = element.elementsByTagName(QString("%1:Name").arg(wsdlAddr.schema));
        if (nodeName.count() > 0) {
            preset.name = nodeName.at(0).toElement().text();
        }

        QDomNodeList nodePosition = element.elementsByTagName(QString("%1:PTZPosition").arg(wsdlAddr.schema));
        if (nodePosition.count() > 0) {
            nodePosition = nodePosition.at(0).childNodes();
            if (nodePosition.count() == 2) {
                QDomNode nodePanTilt = nodePosition.at(0);
                QDomNode nodeZoom = nodePosition.at(1);
                preset.x = nodePanTilt.toElement().attribute("x").toDouble();
                preset.y = nodePanTilt.toElement().attribute("y").toDouble();
                //下面这个属性值是 x 不是 z 哦
                preset.z = nodeZoom.toElement().attribute("x").toDouble();
            }
        }

        presets << preset;
    }

    return presets;
}

void OnvifQuery::getStatus(qreal &x, qreal &y, qreal &z)
{
    QDomNodeList nodeList = doc.elementsByTagName(QString("%1:%2").arg(wsdlAddr.wsdlPtz).arg("PTZStatus"));
    if (nodeList.count() == 1) {
        QDomElement element = nodeList.at(0).toElement();
        QDomNodeList nodePosition = element.elementsByTagName(QString("%1:Position").arg(wsdlAddr.schema));
        if (nodePosition.count() > 0) {
            nodePosition = nodePosition.at(0).childNodes();
            if (nodePosition.count() == 2) {
                QDomNode nodePanTilt = nodePosition.at(0);
                QDomNode nodeZoom = nodePosition.at(1);
                x = nodePanTilt.toElement().attribute("x").toDouble();
                y = nodePanTilt.toElement().attribute("y").toDouble();
                //下面这个属性值是 x 不是 z 哦
                z = nodeZoom.toElement().attribute("x").toDouble();
            }
        }
    }
}

QString OnvifQuery::getValueByTagName(const QString &name, const QString &wsdl)
{
    //视频地址 <tt:Uri>rtsp://192.168.0.160:554/stream0?username=admin&amp;password=E10ADC3949BA59ABBE56E057F20F883E</tt:Uri>
    //抓图地址 <tt:Uri>http://192.168.0.160:80/cgi-bin/snapshot.cgi?stream=0</tt:Uri>
    //事件订阅 <wsa:Address>http://192.168.0.64/onvif/Events/PullSubManager_20210611T073516Z_0</wsa:Address>

    QString value;
    QString tagName = QString("%1:%2").arg(wsdl).arg(name);
    QDomNodeList node = doc.elementsByTagName(tagName);
    if (node.count() > 0) {
        value = node.at(0).toElement().text();
        //value.replace("amp;", "");
    }

    return value;
}

QString OnvifQuery::getValueByTagName(QDomElement element, const QString &name)
{
    QString value;
    QString tagName = QString("%1:%2").arg(wsdlAddr.schema).arg(name);
    QDomNodeList node = element.elementsByTagName(tagName);
    if (node.count() > 0) {
        value = node.at(0).toElement().text();
        //value.replace("amp;", "");
    }

    return value;
}

QStringList OnvifQuery::getValueByTagName(QDomElement element, const QString &name, const QStringList &keys)
{
    QStringList values;
    QString tagName = QString("%1:%2").arg(wsdlAddr.schema).arg(name);
    QDomNodeList node = element.elementsByTagName(tagName);
    if (node.count() > 0) {
        QDomElement element = node.at(0).toElement();
        foreach (QString key, keys) {
            values << element.attribute(key);
        }
    }

    return values;
}

QString OnvifQuery::getStreamUri(const QString &ip, int port)
{
    QString addr = getValueByTagName("Uri", wsdlAddr.schema);
    //设置了端口则取对应设置的映射后的端口
    port = (port == 0 ? OnvifHelper::getUrlPort(addr) : port);
    this->checkAddr(QString("%1:%2").arg(ip).arg(port), addr, false);
    return addr;
}

QString OnvifQuery::getSnapshotUri(const QString &addrPort)
{
    QString addr = getValueByTagName("Uri", wsdlAddr.schema);
    this->checkAddr(addrPort, addr, false);
    return addr;
}

QString OnvifQuery::getEventAddr(const QString &addrPort)
{
    QString addr = getValueByTagName("Address", wsdlAddr.addressing);
    this->checkAddr(addrPort, addr, false);
    return addr;
}

OnvifEventInfo OnvifQuery::getEventInfo()
{
    //<tev:TerminationTime>1970-02-07T16:46:16Z</tev:TerminationTime>
    //<tt:Source><tt:SimpleItem Name="InputToken" Value="AlarmIn_1"/></tt:Source>
    //<tt:Data><tt:SimpleItem Name="LogicalState" Value="false"/></tt:Data>

    OnvifEventInfo eventInfo;
    QDomNodeList nodeTime = doc.elementsByTagName(QString("%1:TerminationTime").arg(wsdlAddr.wsdlEvents));
    QDomNodeList nodeSource = doc.elementsByTagName(QString("%1:Source").arg(wsdlAddr.schema));
    QDomNodeList nodeData = doc.elementsByTagName(QString("%1:Data").arg(wsdlAddr.schema));
    int count = nodeTime.count();
    int count1 = nodeSource.count();
    int count2 = nodeData.count();
    //有数量才说明有报警事件
    //for (int i = 0; i < count; ++i) {
    if (count == 1) {
        QDomElement element = nodeTime.at(0).toElement();
        eventInfo.time = element.text();
        if (count1 == 1 && count1 == count2) {
            element = nodeSource.at(0).toElement();
            nodeSource = element.elementsByTagName(QString("%1:SimpleItem").arg(wsdlAddr.schema));
            element = nodeSource.at(0).toElement();
            eventInfo.sourceName = element.attribute("Name");
            eventInfo.sourceValue = element.attribute("Value");

            element = nodeData.at(0).toElement();
            nodeData = element.elementsByTagName(QString("%1:SimpleItem").arg(wsdlAddr.schema));
            element = nodeData.at(0).toElement();
            eventInfo.dataName = element.attribute("Name");
            eventInfo.dataValue = element.attribute("Value");
        }
    }

    return eventInfo;
}

QList<OnvifVideoSource> OnvifQuery::getVideoSources()
{
    //<trt:VideoSources token="VideoSourceToken001">
    //  <tt:Framerate>25.000000</tt:Framerate>
    //  <tt:Resolution><tt:Width>1280</tt:Width><tt:Height>720</tt:Height></tt:Resolution>
    //  <tt:Imaging><tt:Brightness>0</tt:Brightness><tt:ColorSaturation>0</tt:ColorSaturation><tt:Contrast>0</tt:Contrast><tt:Sharpness>0</tt:Sharpness></tt:Imaging>
    //</trt:VideoSources>

    QList<OnvifVideoSource> videoSources;
    QDomNodeList nodeVideoSources = doc.elementsByTagName(QString("%1:VideoSources").arg(wsdlAddr.wsdlMedia));
    int count = nodeVideoSources.count();
    for (int i = 0; i < count; ++i) {
        OnvifVideoSource videoSource;
        QDomElement element = nodeVideoSources.at(i).toElement();
        videoSource.token = element.attribute("token");

        //帧率
        videoSource.framerate = getValueByTagName(element.toElement(), "Framerate").toDouble();

        //分辨率
        QDomNodeList nodeResolution = element.elementsByTagName(QString("%1:Resolution").arg(wsdlAddr.schema));
        if (nodeResolution.count() > 0) {
            nodeResolution = nodeResolution.at(0).childNodes();
            if (nodeResolution.count() == 2) {
                QDomNode nodeWidth = nodeResolution.at(0);
                QDomNode nodeHeight = nodeResolution.at(1);
                videoSource.width = nodeWidth.toElement().text().toInt();
                videoSource.height = nodeHeight.toElement().text().toInt();
            }
        }

        //图片参数(亮度/色彩饱和度/对比度/锐度)
        QDomNodeList nodeImaging = element.elementsByTagName(QString("%1:Imaging").arg(wsdlAddr.schema));
        if (nodeImaging.count() > 0) {
            //可能有多个节点(按照名称去查找)
            element = element.toElement();
            videoSource.brightness = getValueByTagName(element, "Brightness").toDouble();
            videoSource.colorSaturation = getValueByTagName(element, "ColorSaturation").toDouble();
            videoSource.contrast = getValueByTagName(element, "Contrast").toDouble();
            videoSource.sharpness = getValueByTagName(element, "Sharpness").toDouble();
        }

        videoSources << videoSource;
    }

    return videoSources;
}

OnvifOsdInfo OnvifQuery::getOsd(QDomElement element, const QSize &videoSize)
{
    //<trt:OSDs token=\"0\">
    //  <tt:VideoSourceConfigurationToken>video_source_config</tt:VideoSourceConfigurationToken>
    //  <tt:Type>Text</tt:Type>
    //  <tt:Position>
    //    <tt:Type>Custom</tt:Type>
    //    <tt:Pos y=\"0.939999998\" x=\"-0.959999979\"></tt:Pos>
    //  </tt:Position>
    //  <tt:TextString>
    //    <tt:Type>DateAndTime</tt:Type>
    //    <tt:DateFormat>yyyy-MM-dd</tt:DateFormat>
    //    <tt:TimeFormat>HH:mm:ss</tt:TimeFormat>
    //    <tt:FontSize>42</tt:FontSize>
    //  </tt:TextString>
    //</trt:OSDs>

    //<trt:OSDs token=\"1\">
    //  <tt:VideoSourceConfigurationToken>video_source_config</tt:VideoSourceConfigurationToken>
    //  <tt:Type>Text</tt:Type>
    //  <tt:Position>
    //    <tt:Type>Custom</tt:Type>
    //    <tt:Pos y=\"0.939999998\" x=\"0.5\"></tt:Pos>
    //  </tt:Position>
    //  <tt:TextString>
    //    <tt:Type>Plain</tt:Type>
    //    <tt:FontSize>42</tt:FontSize>
    //    <tt:PlainText>\xE5\xAE\x87\xE8\xA7\x86\xE6\xB5\x8B\xE8\xAF\x95\xE7\x9B\xB8\xE6\x9C\xBA</tt:PlainText>
    //  </tt:TextString>
    //</trt:OSDs>

    OnvifOsdInfo osd;
    osd.token = element.attribute("token");
    QDomNodeList nodePosition = element.elementsByTagName(QString("%1:Position").arg(wsdlAddr.schema));
    QDomNodeList nodeTextString = element.elementsByTagName(QString("%1:TextString").arg(wsdlAddr.schema));
    if (nodePosition.count() > 0) {
        element = element.toElement();
        osd.config = getValueByTagName(element, "VideoSourceConfigurationToken");

        QString dateFormat = getValueByTagName(element, "DateFormat");
        if (!dateFormat.isEmpty()) {
            osd.dateFormat = dateFormat;
        }

        QString timeFormat = getValueByTagName(element, "TimeFormat");
        if (!timeFormat.isEmpty()) {
            osd.timeFormat = timeFormat;
        }

        osd.fontSize = getValueByTagName(element, "FontSize").toInt();
        osd.textType = getValueByTagName(nodeTextString.at(0).toElement(), "Type");
        osd.text = getValueByTagName(element, "PlainText");

        //获取位置类型
        osd.positionType = getValueByTagName(nodePosition.at(0).toElement(), "Type");
        //获取位置(需要转换成屏幕坐标)
        QStringList xy = getValueByTagName(element, "Pos", QStringList() << "x" << "y");
        if (xy.count() == 2) {
            osd.position = OnvifHelper::osdPosToScreenPos(videoSize.width(), videoSize.height(), xy.at(0).toFloat(), xy.at(1).toFloat());
        }

        //qDebug() << getValueByTagName(element, "Color", QStringList() << "X" << "Y" << "Z");
        //qDebug() << getValueByTagName(element, "ChannelName");
    }

    return osd;
}

OnvifOsdInfo OnvifQuery::getOsd(const QSize &videoSize)
{
    OnvifOsdInfo osd;
    QDomNodeList nodeOsd = doc.elementsByTagName(QString("%1:OSD").arg(wsdlAddr.wsdlMedia));
    if (nodeOsd.count() > 0) {
        QDomElement element = nodeOsd.at(0).toElement();
        osd = getOsd(element, videoSize);
    }

    return osd;
}

QList<OnvifOsdInfo> OnvifQuery::getOsds(const QSize &videoSize)
{
    QList<OnvifOsdInfo> osds;
    QDomNodeList nodeOsds = doc.elementsByTagName(QString("%1:OSDs").arg(wsdlAddr.wsdlMedia));
    int count = nodeOsds.count();
    for (int i = 0; i < count; ++i) {
        QDomElement element = nodeOsds.at(i).toElement();
        osds << getOsd(element, videoSize);
    }

    return osds;
}

void OnvifQuery::getImageOption(const QString &tagName, qreal &min, qreal &max)
{
    min = 0;
    max = 255;
    QDomNodeList nodeList = doc.elementsByTagName(QString("%1:%2").arg(wsdlAddr.schema).arg(tagName));
    if (nodeList.count() > 0) {
        QDomElement element = nodeList.at(0).toElement();
        min = getValueByTagName(element, "Min").toDouble();
        max = getValueByTagName(element, "Max").toDouble();
    }
}

void OnvifQuery::getImageOption(OnvifImageSetting &imageSetting)
{
    //<timg:GetOptionsResponse><timg:ImagingOptions>
    //  <tt:Brightness><tt:Min>0.000000</tt:Min><tt:Max>100.000000</tt:Max></tt:Brightness>
    //  <tt:ColorSaturation><tt:Min>0.000000</tt:Min><tt:Max>100.000000</tt:Max></tt:ColorSaturation>
    //  <tt:Contrast><tt:Min>0.000000</tt:Min><tt:Max>100.000000</tt:Max></tt:Contrast>
    //  <tt:Sharpness><tt:Min>0.000000</tt:Min><tt:Max>100.000000</tt:Max></tt:Sharpness>
    //</timg:ImagingOptions></timg:GetOptionsResponse>

    qreal min, max;

    getImageOption("Brightness", min, max);
    imageSetting.brightnessMin = min;
    imageSetting.brightnessMax = max;

    getImageOption("ColorSaturation", min, max);
    imageSetting.colorSaturationMin = min;
    imageSetting.colorSaturationMax = max;

    getImageOption("Contrast", min, max);
    imageSetting.contrastMin = min;
    imageSetting.contrastMax = max;

    getImageOption("Sharpness", min, max);
    imageSetting.sharpnessMin = min;
    imageSetting.sharpnessMax = max;
}

void OnvifQuery::getImageSetting(OnvifImageSetting &imageSetting)
{
    imageSetting.brightness = getValueByTagName("Brightness", wsdlAddr.schema).toDouble();
    imageSetting.colorSaturation = getValueByTagName("ColorSaturation", wsdlAddr.schema).toDouble();
    imageSetting.contrast = getValueByTagName("Contrast", wsdlAddr.schema).toDouble();
    imageSetting.sharpness = getValueByTagName("Sharpness", wsdlAddr.schema).toDouble();
}
