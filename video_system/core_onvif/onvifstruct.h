#ifndef ONVIFSTRUCT_H
#define ONVIFSTRUCT_H

#include <QVariantList>
#include <QString>
#include <QPointF>
#include <QColor>
#include <QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
#define debugx debug.noquote()
#else
#define debugx debug
#endif

//设备基础信息结构体
struct OnvifDeviceInfo {
    //这部分是广播搜索返回的
    QString onvifAddr;      //协议地址-onvif地址
    QString deviceIp;       //设备地址-ip地址
    QString name;           //设备厂家
    QString location;       //厂家城市
    QString hardware;       //硬件描述
    QString macaddr;        //硬件地址

    //这部分是需要用户认证才能获取的
    QString manufacturer;   //厂商名称
    QString model;          //设备型号
    QString firmwareVersion;//固件版本
    QString serialNumber;   //序列编号
    QString hardwareId;     //硬件编号

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifDeviceInfo &deviceInfo) {
        QStringList list;
        list << QString("协议地址: %1").arg(deviceInfo.onvifAddr);
        list << QString("设备地址: %1").arg(deviceInfo.deviceIp);
        list << QString("设备厂家: %1").arg(deviceInfo.name);
        list << QString("厂家城市: %1").arg(deviceInfo.location);
        list << QString("硬件描述: %1").arg(deviceInfo.hardware);
        list << QString("硬件地址: %1").arg(deviceInfo.macaddr);

        list << QString("厂商名称: %1").arg(deviceInfo.manufacturer);
        list << QString("设备型号: %1").arg(deviceInfo.model);
        list << QString("固件版本: %1").arg(deviceInfo.firmwareVersion);
        list << QString("序列编号: %1").arg(deviceInfo.serialNumber);
        list << QString("硬件编号: %1").arg(deviceInfo.hardwareId);

        debugx << list.join("\n");
        return debug;
    }
};

//设备数据信息结构体
struct OnvifDeviceData {
    //主动设置onvif对象的用户和地址
    QString flag;           //唯一标识
    QString userName;       //用户名称
    QString userPwd;        //用户密码

    QString onvifAddr;      //请求地址
    QString profileToken;   //配置文件
    QString videoSource;    //视频文件

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifDeviceData &deviceData) {
        QStringList list;
        list << QString("用户名称: %1").arg(deviceData.userName);
        list << QString("用户密码: %1").arg(deviceData.userPwd);

        list << QString("请求地址: %1").arg(deviceData.onvifAddr);
        list << QString("配置文件: %1").arg(deviceData.profileToken);
        list << QString("视频文件: %1").arg(deviceData.videoSource);

        debugx << list.join("\n");
        return debug;
    }
};

//设备配置文件信息结构体
struct OnvifProfileInfo {
    QString token;  //文件标识
    QString name;   //文件名称
    QString fixed;  //文件填充

    qreal minX;     //坐标X最小值
    qreal maxX;     //坐标X最大值
    qreal minY;     //坐标Y最小值
    qreal maxY;     //坐标Y最大值
    qreal minZ;     //坐标Z最小值
    qreal maxZ;     //坐标Z最大值

    //默认构造函数
    OnvifProfileInfo() {
        minX = 0;
        maxX = 0;
        minY = 0;
        maxY = 0;
        minZ = 0;
        maxZ = 0;
    }

    //数据集合
    QVariant data() {
        QVariantList data;
        data << token;
        data << name;
        data << fixed;

        data << minX;
        data << maxX;
        data << minY;
        data << maxY;
        data << minZ;
        data << maxZ;
        return data;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifProfileInfo &profileInfo) {
        QStringList list;
        list << QString("文件标识: %1").arg(profileInfo.token);
        list << QString("文件填充: %1").arg(profileInfo.fixed);
        list << QString("文件名称: %1").arg(profileInfo.name);

        list << QString("坐标X: [%1 - %2]").arg(profileInfo.minX).arg(profileInfo.maxX);
        list << QString("坐标Y: [%1 - %2]").arg(profileInfo.minY).arg(profileInfo.maxY);
        list << QString("坐标Z: [%1 - %2]").arg(profileInfo.minZ).arg(profileInfo.maxZ);

        debugx << list.join("\n");
        return debug;
    }
};

//预置位信息结构体
struct OnvifPresetInfo {
    QString token;  //预置位编号
    QString name;   //预置位名称

    qreal x;        //预置位X坐标
    qreal y;        //预置位Y坐标
    qreal z;        //预置位Z坐标

    //默认构造函数
    OnvifPresetInfo() {
        token = "1";
        name = "ptz1";

        x = 0;
        y = 0;
        z = 0;
    }

    //数据集合
    QVariant data() {
        QVariantList data;
        data << token;
        data << name;

        data << x;
        data << y;
        data << z;
        return data;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifPresetInfo &presetInfo) {
        QStringList list;
        list << QString("预置位编号: %1").arg(presetInfo.token);
        list << QString("预置位名称: %1").arg(presetInfo.name);

        list << QString("预置位X坐标: %1").arg(presetInfo.x);
        list << QString("预置位Y坐标: %1").arg(presetInfo.y);
        list << QString("预置位Z坐标: %1").arg(presetInfo.z);

        debugx << list.join("\n");
        return debug;
    }
};

//标签信息结构体
struct OnvifOsdInfo {
    QString token;          //标签标识
    QString config;         //配置名称
    QString positionType;   //位置类型
    QPoint position;        //标签坐标

    int fontSize;           //字体大小
    QColor fontColor;       //字体颜色

    QString textType;       //文字类型
    QString text;           //文字内容
    QString dateFormat;     //日期格式
    QString timeFormat;     //时间格式

    //默认构造函数
    OnvifOsdInfo() {
        token = "1";
        positionType = "Custom";
        position = QPoint(0, 0);

        fontColor = "#ffffff";
        dateFormat = "yyyy-MM-dd";
        timeFormat = "HH:mm:ss";
    }

    //数据集合
    QVariant data() {
        QVariantList data;
        data << token;
        data << config;
        data << positionType;
        data << position;

        data << fontSize;
        data << fontColor;

        data << textType;
        data << text;
        data << dateFormat;
        data << timeFormat;
        return data;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifOsdInfo &osdInfo) {
        QStringList list;
        list << QString("标签标识: %1").arg(osdInfo.token);
        list << QString("配置名称: %1").arg(osdInfo.config);
        list << QString("位置类型: %1").arg(osdInfo.positionType);
        list << QString("标签坐标: [%1, %2]").arg(osdInfo.position.x()).arg(osdInfo.position.y());

        list << QString("字体大小: %1").arg(osdInfo.fontSize);
        list << QString("字体颜色: %1").arg(osdInfo.fontColor.name());

        list << QString("文字类型: %1").arg(osdInfo.textType);
        list << QString("文字内容: %1").arg(osdInfo.text);
        list << QString("日期格式: %1").arg(osdInfo.dateFormat);
        list << QString("时间格式: %1").arg(osdInfo.timeFormat);

        debugx << list.join("\n");
        return debug;
    }
};

//节点地址结构体
struct OnvifWsdlAddr {
    //解析数据需要用到对应的节点名称
    QString discovery;      //设备发现 http://schemas.xmlsoap.org/ws/2005/04/discovery
    QString schema;         //节点结构 http://www.onvif.org/ver10/schema
    QString addressing;     //请求地址 http://www.w3.org/2005/08/addressing
    QString wsnt;           //事件订阅 http://docs.oasis-open.org/wsn/b-2

    //还有一些其他不常用节点比如 replay recording deviceIO
    QString wsdlDevice;     //设备节点 http://www.onvif.org/ver10/device/wsdl
    QString wsdlMedia;      //媒体节点 http://www.onvif.org/ver10/media/wsdl
    QString wsdlMedia2;     //媒体节点 http://www.onvif.org/ver20/media/wsdl
    QString wsdlImaging;    //图片节点 http://www.onvif.org/ver20/imaging/wsdl
    QString wsdlEvents;     //事件节点 http://www.onvif.org/ver10/events/wsdl
    QString wsdlAnalytics;  //分析节点 http://www.onvif.org/ver20/analytics/wsdl
    QString wsdlPtz;        //云台节点 http://www.onvif.org/ver20/ptz/wsdl

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifWsdlAddr &wsdlAddr) {
        QStringList list;
        list << QString("设备发现: %1").arg(wsdlAddr.discovery);
        list << QString("节点结构: %1").arg(wsdlAddr.schema);
        list << QString("请求地址: %1").arg(wsdlAddr.addressing);
        list << QString("事件订阅: %1").arg(wsdlAddr.wsnt);

        list << QString("设备节点: %1").arg(wsdlAddr.wsdlDevice);
        list << QString("媒体节点: %1").arg(wsdlAddr.wsdlMedia);
        list << QString("媒体节点: %1").arg(wsdlAddr.wsdlMedia2);
        list << QString("图片节点: %1").arg(wsdlAddr.wsdlImaging);
        list << QString("事件节点: %1").arg(wsdlAddr.wsdlEvents);
        list << QString("分析节点: %1").arg(wsdlAddr.wsdlAnalytics);
        list << QString("云台节点: %1").arg(wsdlAddr.wsdlPtz);

        debugx << list.join("\n");
        return debug;
    }
};

//请求地址结构体
struct OnvifHttpAddr {
    //根据命名空间去查找节点地址
    QString addrDevice;     //设备地址 http://192.168.0.160:80/Device
    QString addrMedia;      //媒体地址 http://192.168.0.160:80/Media
    QString addrMedia2;     //媒体地址 http://192.168.0.160:80/Media2
    QString addrImaging;    //图片地址 http://192.168.0.160:80/Imaging
    QString addrEvents;     //事件地址 http://192.168.0.160:80/Event
    QString addrAnalytics;  //分析地址 http://192.168.0.160:80/Analytics
    QString addrPtz;        //云台地址 http://192.168.0.160:80/PTZ

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifHttpAddr &httpAddr) {
        QStringList list;
        list << QString("设备地址: %1").arg(httpAddr.addrDevice);
        list << QString("媒体地址: %1").arg(httpAddr.addrMedia);
        list << QString("媒体地址2: %1").arg(httpAddr.addrMedia2);
        list << QString("图片地址: %1").arg(httpAddr.addrImaging);
        list << QString("事件地址: %1").arg(httpAddr.addrEvents);
        list << QString("分析地址: %1").arg(httpAddr.addrAnalytics);
        list << QString("云台地址: %1").arg(httpAddr.addrPtz);

        debugx << list.join("\n");
        return debug;
    }
};

//报警事件结构体
struct OnvifEventInfo {
    QString time;           //触发时间
    QString sourceName;     //报警源名
    QString sourceValue;    //报警源值
    QString dataName;       //报警名称
    QString dataValue;      //报警数值

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifEventInfo &eventInfo) {
        QStringList list;
        list << QString("触发时间: %1").arg(eventInfo.time);
        list << QString("报警源名: %1").arg(eventInfo.sourceName);
        list << QString("报警源值: %1").arg(eventInfo.sourceValue);
        list << QString("报警名称: %1").arg(eventInfo.dataName);
        list << QString("报警数值: %1").arg(eventInfo.dataValue);

        debugx << list.join("\n");
        return debug;
    }
};

//视频配置结构体
struct OnvifVideoSource {
    QString token;          //配置名称
    qreal framerate;        //视频帧率
    int width;              //视频宽度
    int height;             //视频高度

    qreal brightness;       //明亮度值
    qreal colorSaturation;  //饱和度值
    qreal contrast;         //对比度值
    qreal sharpness;        //尖锐度值

    //默认构造函数
    OnvifVideoSource() {
        token = "VideoSource_1";
        framerate = 0;
        width = 0;
        height = 0;

        brightness = 0;
        colorSaturation = 0;
        contrast = 0;
        sharpness = 0;
    }

    //数据集合
    QVariant data() {
        QVariantList data;
        data << token;
        data << framerate;
        data << width;
        data << height;

        data << brightness;
        data << colorSaturation;
        data << contrast;
        data << sharpness;
        return data;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifVideoSource &videoSource) {
        QStringList list;
        list << QString("配置名称: %1").arg(videoSource.token);
        list << QString("视频帧率: %1").arg(videoSource.framerate);
        list << QString("视频宽度: %1").arg(videoSource.width);
        list << QString("视频高度: %1").arg(videoSource.height);

        list << QString("明亮度值: %1").arg(videoSource.brightness);
        list << QString("色彩度值: %1").arg(videoSource.colorSaturation);
        list << QString("饱和度值: %1").arg(videoSource.contrast);
        list << QString("尖锐度值: %1").arg(videoSource.sharpness);

        debugx << list.join("\n");
        return debug;
    }
};

//图片参数结构体
struct OnvifImageSetting {
    qreal brightness;       //明亮度
    qreal brightnessMin;    //明亮度最小值
    qreal brightnessMax;    //明亮度最大值

    qreal colorSaturation;  //饱和度
    qreal colorSaturationMin;//饱和度最小值
    qreal colorSaturationMax;//饱和度最大值

    qreal contrast;         //对比度
    qreal contrastMin;      //对比度最小值
    qreal contrastMax;      //对比度最大值

    qreal sharpness;        //尖锐度
    qreal sharpnessMin;     //尖锐度最小值
    qreal sharpnessMax;     //尖锐度最大值

    //默认构造函数
    OnvifImageSetting() {
        brightness = 0;
        brightnessMin = 0;
        brightnessMax = 255;

        colorSaturation = 0;
        colorSaturationMin = 0;
        colorSaturationMax = 255;

        contrast = 0;
        contrastMin = 0;
        contrastMax = 255;

        sharpness = 0;
        sharpnessMin = 0;
        sharpnessMax = 255;
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifImageSetting &imageSetting) {
        QStringList list;
        list << QString("明亮度值: %1 [%2 - %3]").arg(imageSetting.brightness).arg(imageSetting.brightnessMin).arg(imageSetting.brightnessMax);
        list << QString("饱和度值: %1 [%2 - %3]").arg(imageSetting.colorSaturation).arg(imageSetting.colorSaturationMin).arg(imageSetting.colorSaturationMax);
        list << QString("对比度值: %1 [%2 - %3]").arg(imageSetting.contrast).arg(imageSetting.contrastMin).arg(imageSetting.contrastMax);
        list << QString("尖锐度值: %1 [%2 - %3]").arg(imageSetting.sharpness).arg(imageSetting.sharpnessMin).arg(imageSetting.sharpnessMax);

        debugx << list.join("\n");
        return debug;
    }
};

//网络参数结构体
struct OnvifNetConfig {
    QString name;       //网卡名字
    QString hwAddress;  //物理地址
    QString hostName;   //主机名称

    QString ipDhcp;     //自动分配网络地址
    QString ipAddress;  //网络地址
    QString mask;       //子网掩码
    QString gateway;    //网关地址

    QString dnsDhcp;    //自动分配DNS地址
    QString dns;        //DNS地址

    //默认构造函数
    OnvifNetConfig() {
        name = "eth0";
        hwAddress = "ff:ff:ff:ff:ff:ff";
        hostName = "xxx";

        ipDhcp = "false";
        ipAddress = "255.255.255.255";
        mask = "255.255.255.0";
        gateway = "192.168.0.1";

        dnsDhcp = "false";
        dns = "180.76.76.76";
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const OnvifNetConfig &netConfig) {
        QStringList list;
        list << QString("网卡名字: %1").arg(netConfig.name);
        list << QString("物理地址: %1").arg(netConfig.hwAddress);
        list << QString("主机名称: %1").arg(netConfig.hostName);

        list << QString("自动分配: %1").arg(netConfig.ipDhcp);
        list << QString("网络地址: %1").arg(netConfig.ipAddress);
        list << QString("子网掩码: %1").arg(netConfig.mask);
        list << QString("网关地址: %1").arg(netConfig.gateway);

        list << QString("自动DNS: %1").arg(netConfig.dnsDhcp);
        list << QString("DNS地址: %1").arg(netConfig.dns);

        debugx << list.join("\n");
        return debug;
    }
};

#endif // ONVIFSTRUCT_H
