#include "frmdevicegps.h"
#include "ui_frmdevicegps.h"
#include "frmmapcore.h"
#include "qthelper.h"

frmDeviceGps::frmDeviceGps(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceGps)
{
    ui->setupUi(this);
    this->initForm();
}

frmDeviceGps::~frmDeviceGps()
{
    delete ui;
}

void frmDeviceGps::initForm()
{
    //实例化地图内核类
    map = new frmMapCore;
    ui->verticalLayout->addWidget(map);

    //关联加载完成信号
    connect(map, SIGNAL(loadFinished(bool)), this, SLOT(loadDevice()));
    //关联数据信号
    connect(map, SIGNAL(receiveDataFromJs(QString, QVariant)),
            this, SLOT(receiveDataFromJs(QString, QVariant)));

    //赋值百度地图通信类
    baidu = map->getMapBaiDu();
    //启用单击获取经纬度
    baidu->setEnableClickPoint(true);
    //设置地图缩放级别,可以自行调整到自己合适的级别
    baidu->setMapZoom(AppConfig::MapZoom - 1);

    //关联全局信号指定设备移动
    connect(AppEvent::Instance(), SIGNAL(moveDevice(int, QString, QString)),
            this, SLOT(moveDevice(int, QString, QString)));
}

void frmDeviceGps::loadDevice()
{
    //已经加载过就不用加载
    if (listPoints.count() > 0) {
        return;
    }

    //自定义图标
    for (int i = 0; i < AppData::GpsDeviceCount; ++i) {
        //先删除原有的
        QString name = AppData::GpsDeviceNames.at(i);
        QString point = AppData::GpsDevicePoints.at(i);
        QString js = QString("deleteMarker('%1')").arg(name);
        runJs(js);
        js = QString("addMarker('%1', '%2', '', '', 60, '%3', 2, 0, '%4', %5)").arg(name, "", point).arg(AppData::DeviceIconFile).arg(AppData::DeviceIconSize);
        runJs(js);
        listPoints << QStringList();
    }

    //演示测试数据模拟移动
    if (AppConfig::WorkMode == 1 || AppConfig::WorkMode == 2) {
        QTimer::singleShot(1000, this, SLOT(moveDevice()));
    }
}

void frmDeviceGps::moveDevice()
{
    //下面代码可以注释,用于模拟轨迹
    for (int i = 0; i < AppData::GpsDeviceCount; ++i) {
        //取出初始坐标
        QString point = AppData::GpsDevicePoints.at(i);
        QStringList list = point.split(",");
        double lng = list.at(0).toDouble();
        double lat = list.at(1).toDouble();
        moveDevice(i, 15, lng, lat, -0.00008, -0.00002);
    }
}

void frmDeviceGps::moveDevice(int id, int count, double lngValue, double latValue, double lngOffset, double latOffset)
{
    moveDevice(id, QString::number(lngValue, 'f', 6), QString::number(latValue, 'f', 6));
    for (int i = 0; i < count; ++i) {
        //qDebug() << qSetRealNumberPrecision(10) << lngValue << latValue;
        lngValue += lngOffset;
        latValue += latOffset;
        moveDevice(id, QString::number(lngValue, 'f', 6), QString::number(latValue, 'f', 6));
    }
}

void frmDeviceGps::receiveDataFromJs(const QString &type, const QVariant &data)
{
    //qDebug() << "frmDeviceGps" << type << data;
    QString result = data.toString();
    if (type == "point") {
        //演示如何从鼠标点击处获取经纬度坐标
        QString lnglat = MapHelper::getLngLat2(result);
        //AppEvent::Instance()->slot_receiveLngLat(lnglat);
        qDebug() << TIMEMS << lnglat;
    }
}

void frmDeviceGps::runJs(const QString &js)
{
    map->runJs(js);
}

void frmDeviceGps::moveDevice(int id, const QString &lng, const QString &lat)
{
    if (listPoints.count() == 0) {
        return;
    }

    //先删除上一次添加的线条,不然会重叠不好看
    runJs("deleteOverlay('Polyline')");

    //添加轨迹线条
    QString point = QString("%1,%2").arg(lng).arg(lat);
    QString name = AppData::GpsDeviceNames.at(id);
    QString js = QString("moveMarker('%1', '%2')").arg(name).arg(point);
    runJs(js);

    listPoints[id] << point;
    for (int i = 0; i < AppData::GpsDeviceCount; ++i) {
        QString points = listPoints.at(i).join("|");
        QString color = AppData::GpsDeviceColors.at(i);
        QString js = QString("addPolyline('%1', '%2', 3, 1)").arg(points).arg(color);
        runJs(js);
    }
}
