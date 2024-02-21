#include "frmdevicegps2.h"
#include "ui_frmdevicegps2.h"
#include "frmmapcore.h"
#include "qthelper.h"

frmDeviceGps2::frmDeviceGps2(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceGps2)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
}

frmDeviceGps2::~frmDeviceGps2()
{
    delete ui;
}

void frmDeviceGps2::initForm()
{
    //实例化地图内核类
    map = new frmMapCore;
    //设置默认中心点
    map->setCenterPoint("121.432612,31.190846");
    ui->verticalLayout->addWidget(map);

    //关联加载完成信号
    connect(map, SIGNAL(loadFinished(bool)), this, SLOT(initMarker()));

    //赋值百度地图通信类
    baidu = map->getMapBaiDu();
    //设置地图缩放级别,可以自行调整到自己合适的级别
    baidu->setMapZoom(13);
    //是否采用本地离线地图
    //baidu->setMapLocal(true);

    //定时器模拟轨迹,自行调整间隔控制速度
    isInit = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(moveMarker()));
    timer->start(100);
}

void frmDeviceGps2::runJs(const QString &js)
{
    map->runJs(js);
}

void frmDeviceGps2::initData()
{
    listName << "无人机1" << "无人机2";
    listColor << "#22A3A9" << "#D64D54";
    listIndex << 0 << 0;

    //从文件读取
    loadData(":/gps1.txt");
    loadData(":/gps2.txt");
}

void frmDeviceGps2::loadData(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString points = QString(file.readAll());
        points.replace("\r", "");
        points.replace("\n", "");
        listPoints << points.split(";");
        file.close();
    }
}

void frmDeviceGps2::initMarker()
{
    //根据设备名称的数量初始化标注点
    int count = listName.count();
    for (int i = 0; i < count; ++i) {
        //先删除原有的再添加新的
        QString name = listName.at(i);
        QString js = QString("deleteMarker('%1')").arg(name);
        runJs(js);

        //图片文件在可执行文件下的config/device目录
        QString icon = "./device/device_airplane.png";
        int size = 60;
        js = QString("addMarker('%1', '', '', '', 60, '%1', 0, 0, '%2', %3)").arg(name).arg(icon).arg(size);
        runJs(js);
    }

    isInit = true;
}

void frmDeviceGps2::moveMarker()
{
    int count = listName.count();
    if (!isVisible() || !isInit || count == 0) {
        return;
    }

    if (listName.count() != listColor.count() || listColor.count() != listPoints.count()) {
        return;
    }

    //清空之前的轨迹点
    runJs("deleteOverlay('Polyline')");

    //挨个对设备的路径进行绘制
    for (int i = 0; i < count; ++i) {
        QString name = listName.at(i);
        QString color = listColor.at(i);
        QStringList points = listPoints.at(i);

        //索引到最后则自动重新来
        int index = listIndex.at(i);
        if (index == points.count() - 1) {
            index = 0;
            listIndex[i] = 0;
        }

        //找出和上一个点之间的角度
        int angle = 0;
        QString point = points.at(index);
        //第一个点和最后一个点不用处理
        if (index > 0 && index < points.count() - 1) {
            //上一个点坐标
            QString point2 = points.at(index - 1);
            //计算当前上一个点和当前点的旋转角度
            angle = MapHelper::getAngle(point2, point);
        }

        //执行移动设备点函数,参数带旋转角度
        QString js = QString("moveMarker('%1', '%2', %3)").arg(name).arg(point).arg(angle);
        runJs(js);
        //qDebug() << TIMEMS << name << color << point << angle;

        //如果是重新绘制则取出第一个点到当前索引的所有点
        //如果是沿线移动则绘制所有点
        int moveMode = 0;
        QString temp;
        if (moveMode == 0) {
            QStringList list;
            for (int j = 0; j < index; ++j) {
                list << points.at(j);
            }
            temp = list.join("|");
        } else {
            temp = points.join("|");
        }

        //不同轨迹不同颜色
        js = QString("addPolyline('%1', '%2')").arg(temp).arg(color);
        runJs(js);

        //重新移动索引
        listIndex[i]++;
    }
}
