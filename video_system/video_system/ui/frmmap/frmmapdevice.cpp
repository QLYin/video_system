#include "frmmapdevice.h"
#include "ui_frmmapdevice.h"
#include "frmmapcore.h"
#include "qthelper.h"
#include "dbquery.h"
#include "devicehelper.h"

frmMapDevice::frmMapDevice(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapDevice)
{
    ui->setupUi(this);
    this->initForm();
}

frmMapDevice::~frmMapDevice()
{
    delete ui;
}

void frmMapDevice::showEvent(QShowEvent *)
{
    loadIpcName();
}

void frmMapDevice::initForm()
{
    //默认地图中心坐标
    QStringList list = AppConfig::MapCenter.split(",");
    ui->txtLongitude->setText(list.at(0));
    ui->txtLatitude->setText(list.at(1));

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
    //显示路况信息
    baidu->setShowTrafficControl(true);
    //显示浮动工具栏
    baidu->setShowOverlayTool(true);
    //显示地图类型控件
    baidu->setShowMapTypeControl(true);
    //启用单击获取经纬度
    baidu->setEnableClickPoint(true);
    //显示缩放级别导航
    baidu->setShowNavigationControl(true);
    //设置地图缩放级别,可以自行调整到自己合适的级别
    baidu->setMapZoom(AppConfig::MapZoom);

    ui->txtZoom->setText(QString::number(AppConfig::MapZoom));
    ui->frameRight->setFixedWidth(AppData::RightWidth + 20);

    lastTime = QDateTime::currentDateTime();
    connect(AppEvent::Instance(), SIGNAL(saveIpcInfo()), this, SLOT(loadDevice()));
}

void frmMapDevice::loadDevice()
{
    //还没加载过地图则不用处理
    if (!map->getIsShow()) {
        return;
    }

    //加载完成后切换到卫星混合地图
    //runJs("setMapType(2)");

    //清空所有覆盖物
    runJs("deleteOverlay('')");

    //载入所有点
    QStringList names, addrs, points;
    DeviceHelper::getDeviceInfo(names, addrs, points);

    //自定义图标
    QString iconfile = AppData::DeviceIconFile;
    int iconsize = AppData::DeviceIconSize;
    for (int i = 0; i < names.count(); ++i) 
    {
     //   QString js = QString("addMarker('%1', '%2', '', '', 60, '%3', 2, 0, '%4', %5)")
     //                .arg(names.at(i), addrs.at(i), points.at(i)).arg(iconfile).arg(iconsize);
     //   runJs(js);
    }
}

void frmMapDevice::loadIpcName()
{
    //将摄像机的名称和编号加载到下拉框
    ui->cboxIpcName->clear();
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        int id = DbData::IpcInfo_IpcID.at(i);
        QString name = DbData::IpcInfo_IpcName.at(i);
        ui->cboxIpcName->addItem(name, id);
    }
}

void frmMapDevice::setMapLocal(bool mapLocal)
{
    baidu->setMapLocal(mapLocal);
}

void frmMapDevice::receiveDataFromJs(const QString &type, const QVariant &data)
{
    //qDebug() << "frmMapDevice" << type << data;
    QString result = data.toString();
    if (type == "zoom") {
        ui->txtZoom->setText(result);
    } else if (type == "marker") {
        //这里过滤下频繁的双击(可能导致有问题)        
        QDateTime now = QDateTime::currentDateTime();
        int offset = lastTime.msecsTo(now);
        lastTime = now;
        if (offset < 1000) {
            return;
        }

        //取出视频流地址打开
        QStringList list = result.split(" ");
        int index = DbData::IpcInfo_IpcName.indexOf(list.at(1));
        if (index >= 0) {
            QString url = DbData::getRtspAddr(index);
            DeviceHelper::showVideo(url);
        }
    } else if (type == "point") {
        //演示如何从地图上标注点获取到经纬度
        QStringList list = result.split(",");
        if (list.count() == 2) {
            QString lng = MapHelper::getLngLat1(list.at(0), 8);
            QString lat = MapHelper::getLngLat1(list.at(1), 8);
            ui->txtLongitude->setText(lng);
            ui->txtLatitude->setText(lat);
        }
    }
}

void frmMapDevice::runJs(const QString &js)
{
    map->runJs(js);
}

void frmMapDevice::on_btnZoom_clicked()
{
    runJs("getZoom()");
}

void frmMapDevice::on_btnUpdate_clicked()
{
    int index = ui->cboxIpcName->currentIndex();
    if (index < 0) {
        return;
    }

    int id = ui->cboxIpcName->itemData(index).toInt();
    QString position = QString("%1|%2").arg(ui->txtLongitude->text()).arg(ui->txtLatitude->text());
    DbQuery::updatePosition(id, position);

    //自动移动到下一个设备
    if (index < ui->cboxIpcName->count() - 1) {
        ui->cboxIpcName->setCurrentIndex(index + 1);
    }
}

void frmMapDevice::on_btnDevice_clicked()
{
    QTimer::singleShot(200, this, SLOT(loadDevice()));
}

void frmMapDevice::on_btnDistance_clicked()
{
    //找出当前经纬度值对应最近的摄像机
    double lng = ui->txtLongitude->text().toDouble();
    double lat = ui->txtLatitude->text().toDouble();
    int ipcID = DbQuery::getIpcInfo(lng, lat);
    if (ipcID >= 0) {
        int index = DbData::IpcInfo_IpcID.indexOf(ipcID);
        QString ipcName = DbData::IpcInfo_IpcName.at(index);
        ui->txtDistance->setText(ipcName);
    }
}
