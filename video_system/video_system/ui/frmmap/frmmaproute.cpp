#include "frmmaproute.h"
#include "ui_frmmaproute.h"
#include "frmmapcore.h"
#include "qthelper.h"

frmMapRoute::frmMapRoute(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapRoute)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmMapRoute::~frmMapRoute()
{
    delete ui;
}

void frmMapRoute::showEvent(QShowEvent *)
{
    //只需要加载一次,避免重复初始化
    static bool isShow = false;
    if (!isShow) {
        isShow = true;
        QTimer::singleShot(1000, this, SLOT(on_btnSearchData_clicked()));
    }
}

QTableWidget *frmMapRoute::getTableWidget()
{
    //哪个可见就采用哪个
    if (ui->tableWidgetSource->isVisible()) {
        return ui->tableWidgetSource;
    } else {
        return ui->tableWidgetTarget;
    }
}

void frmMapRoute::initForm()
{
    ui->widgetRight->setFixedWidth(AppData::RightWidth + 20);
    ui->tabWidget->setProperty("tabCount", 2);

    map = new frmMapCore;
    ui->verticalLayout->addWidget(map);
    //关联数据信号
    connect(map, SIGNAL(receiveDataFromJs(QString, QVariant)),
            this, SLOT(receiveDataFromJs(QString, QVariant)));

    //赋值百度地图通信类
    baidu = map->getMapBaiDu();
    //加粗线条
    baidu->setStrokeWeight(10);
    //设置地图占高度 取屏幕高度的倍数
    baidu->setHeight(QtHelper::deskHeight() * 0.6);
    //启用单击获取经纬度
    baidu->setEnableClickPoint(true);
    //设置地图缩放级别,可以自行调整到自己合适的级别
    baidu->setMapZoom(AppConfig::MapZoom);

    //定时器模拟轨迹
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(moveMarker()));

    //ui->labInfo->setFixedHeight(ui->labInfo->sizeHint().height() + 5);
    this->initTable(ui->tableWidgetSource);
    this->initTable(ui->tableWidgetTarget);
}

void frmMapRoute::initConfig()
{
    ui->txtStartAddr->setText(AppConfig::RouteStartAddr);
    connect(ui->txtStartAddr, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtEndAddr->setText(AppConfig::RouteEndAddr);
    connect(ui->txtEndAddr, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->cboxRouteType->setCurrentIndex(AppConfig::RouteType);
    connect(ui->cboxRouteType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtPointCount->setText(QString::number(AppConfig::RoutePointCount));
    connect(ui->txtPointCount, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtDeviceName->setText(AppConfig::RouteDeviceName);
    connect(ui->txtDeviceName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    //添加移动间隔
    QStringList listMoveInterval, listMoveIntervalx;
    listMoveInterval << "0.02 秒钟" << "0.05 秒钟" << "0.10 秒钟" << "0.30 秒钟" << "0.50 秒钟" << "1.00 秒钟" << "3.00 秒钟";
    listMoveIntervalx << "20" << "50" << "100" << "300" << "500" << "1000" << "3000";

    int count = listMoveInterval.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxMoveInterval->addItem(listMoveInterval.at(i), listMoveIntervalx.at(i));
    }

    ui->cboxMoveInterval->setCurrentIndex(AppConfig::RouteMoveInterval);
    connect(ui->cboxMoveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxMoveMode->addItem("重新绘制");
    ui->cboxMoveMode->addItem("沿线运动");
    ui->cboxMoveMode->setCurrentIndex(AppConfig::RouteMoveMode);
    connect(ui->cboxMoveMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
}

void frmMapRoute::saveConfig()
{
    AppConfig::RouteStartAddr = ui->txtStartAddr->text().trimmed();
    AppConfig::RouteEndAddr = ui->txtEndAddr->text().trimmed();
    AppConfig::RouteType = ui->cboxRouteType->currentIndex();
    AppConfig::RoutePointCount = ui->txtPointCount->text().trimmed().toInt();
    AppConfig::RouteDeviceName = ui->txtDeviceName->text().trimmed();
    AppConfig::RouteMoveInterval = ui->cboxMoveInterval->currentIndex();
    AppConfig::RouteMoveMode = ui->cboxMoveMode->currentIndex();
    AppConfig::writeConfig();
}

void frmMapRoute::setInfo(int angle, int index, int count)
{
    QString info = QString("角度 %1°/第 %2 个/共 %3 个").arg(angle).arg(index).arg(count);
    ui->labTip->setText(info);
}

void frmMapRoute::initTable(QTableWidget *tableWidget)
{
    //初始化表格控件
    QtHelper::initTableView(tableWidget);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "编号" << "经纬度值");
    tableWidget->setColumnWidth(0, 45);
    tableWidget->setColumnWidth(1, 100);
}

void frmMapRoute::addItem(QTableWidget *tableWidget, int index, const QString &point)
{
    //编号
    QTableWidgetItem *itemId = new QTableWidgetItem;
    itemId->setText(QString::number(index + 1));
    itemId->setTextAlignment(Qt::AlignCenter);
    tableWidget->setItem(index, 0, itemId);

    QTableWidgetItem *itemPoint = new QTableWidgetItem;
    //重新过滤小数点,对齐更好看
    itemPoint->setText(MapHelper::getLngLat2(point));
    //设置原始数据更准确
    itemPoint->setData(Qt::UserRole, point);
    tableWidget->setItem(index, 1, itemPoint);
}

void frmMapRoute::receiveDataFromJs(const QString &type, const QVariant &data)
{
    if (data.isNull()) {
        return;
    }

    //qDebug() << "frmMapRoute" << type << data;
    QString result = data.toString();
    if (type == "point") {
        if (ui->ckSelectAddr->isChecked()) {
            QString point = MapHelper::getLngLat2(result);
            //判断哪里勾选了就设置到哪里
            if (ui->rbtnStartAddr->isChecked()) {
                ui->txtStartAddr->setText(point);
            } else if (ui->rbtnEndAddr->isChecked()) {
                ui->txtEndAddr->setText(point);
            }
        }
    } else if (type == "routepoints") {
        //将查询路径转换成经纬度坐标点集合数据显示
        routeDatas.clear();
        ui->tableWidgetSource->clearContents();
        //可能会有多个路径集合,目前测试下来都是一个路径集合
        QStringList datas = result.split("|");
        foreach (QString data, datas) {
            QStringList points = data.split(";");
            routeDatas << points;
            int count = points.count();
            ui->tableWidgetSource->setRowCount(count);
            for (int i = 0; i < count; ++i) {
                addItem(ui->tableWidgetSource, i, points.at(i));
            }
        }

        setInfo(0, 0, 0);
    }
}

void frmMapRoute::runJs(const QString &js)
{
    map->runJs(js);
}

void frmMapRoute::on_btnSearchData_clicked()
{
    int routeType = ui->cboxRouteType->currentIndex();
    int policyType = ui->cboxPolicyType->currentIndex();
    QString startAddr = ui->txtStartAddr->text().trimmed();
    QString endAddr = ui->txtEndAddr->text().trimmed();
    baidu->setRotueInfo(routeType, policyType, startAddr, endAddr);
    map->loadMap();

    ui->rbtnStartAddr->setChecked(true);
    ui->tabWidget->setCurrentIndex(0);
}

void frmMapRoute::moveMarker()
{
    QTableWidget *tableWidget = getTableWidget();
    int row = tableWidget->currentRow();
    int count = tableWidget->rowCount();
    if (row >= 0 && row < count) {
        //找出和上一个点之间的角度
        int angle = 0;
        QString point = tableWidget->item(row, 1)->data(Qt::UserRole).toString();
        //第一个点和最后一个点不用处理
        if (row > 0 && row < count - 1) {
            //上一个点坐标
            QString point2 = tableWidget->item(row - 1, 1)->data(Qt::UserRole).toString();
            //计算当前上一个点和当前点的旋转角度
            angle = MapHelper::getAngle(point2, point);
        }

        //执行移动设备点函数,参数带旋转角度
        QString js = QString("moveMarker('%1', '%2', %3)").arg(name).arg(point).arg(angle);
        runJs(js);

        //重新绘制轨迹点
        if (ui->cboxMoveMode->currentIndex() == 0) {
            //清空之前的轨迹点
            js = QString("deleteOverlay('Polyline')");
            runJs(js);

            //取出第一个点到当前焦点所在行的点组成已经走过的轨迹点集合重新绘制
            QStringList points;
            for (int i = 0; i <= row; ++i) {
                points << tableWidget->item(i, 1)->data(Qt::UserRole).toString();
            }

            js = QString("addPolyline('%1')").arg(points.join("|"));
            runJs(js);
        }

        //显示当前第几个数据
        setInfo(angle, row + 1, count);
        tableWidget->setCurrentCell(row + 1, 0);
    } else {
        on_btnTestData_clicked();
    }
}

void frmMapRoute::on_btnTestData_clicked()
{
    QTableWidget *tableWidget = getTableWidget();
    if (ui->btnTestData->text() == "模拟轨迹") {
        //限制最小数量
        if (tableWidget->rowCount() < 2) {
            return;
        }

        //第一步: 添加一个标记
        name = ui->txtDeviceName->text().trimmed();
        if (name.isEmpty()) {
            name = "国产大飞机C919";
        }

        //图片文件在可执行文件下的config/device目录
        QString icon = "./device/device_airplane.png";
        int size = 60;
        QString js = QString("addMarker('%1', '', '', '', 60, '%1', 0, 0, '%2', %3)").arg(name).arg(icon).arg(size);
        runJs(js);

        //第二步: 移到第一个点
        tableWidget->setFocus();
        tableWidget->setCurrentCell(0, 0);
        ui->btnTestData->setText("停止模拟");
        ui->tabWidget->setTabEnabled(ui->tableWidgetSource->isVisible() ? 1 : 0, false);

        //第三步: 启动定时器并立即执行一次
        int index = ui->cboxMoveInterval->currentIndex();
        timer->start(ui->cboxMoveInterval->itemData(index).toInt());
        moveMarker();
    } else {
        //清空标记
        QString js = QString("deleteMarker('%1')").arg(name);
        runJs(js);

        //停止定时器
        timer->stop();
        ui->btnTestData->setText("模拟轨迹");
        ui->tabWidget->setTabEnabled(ui->tableWidgetSource->isVisible() ? 1 : 0, true);
    }
}

void frmMapRoute::on_btnCheckData_clicked()
{
    if (timer->isActive()) {
        return;
    }

    //第一步: 计算总数,求平均值=实际总数/预期总数+1,预期总数>=实际总数则不用处理
    int countSource = ui->tableWidgetSource->rowCount();
    int countTarget = ui->txtPointCount->text().trimmed().toInt();
    if (countTarget >= countSource) {
        QtHelper::showMessageBoxError("目标点数不能大于等于原数据点数!");
        ui->txtPointCount->setFocus();
        return;
    }

    //第二步: 根据平均值挨个取出值
    QStringList points;
    int avg = countSource / countTarget + 1;
    for (int i = 0; i < countSource; i += avg) {
        QString point = ui->tableWidgetSource->item(i, 1)->data(Qt::UserRole).toString();
        points << point;
    }

    //必须加上末尾这个作为结束,如果刚好除尽则不用
    QString point = ui->tableWidgetSource->item(countSource - 1, 1)->data(Qt::UserRole).toString();
    if (points.last() != point) {
        points << point;
    }

    //第三步: 将数据重新填入筛选数据列表
    int count = points.count();
    ui->tableWidgetTarget->clearContents();
    ui->tableWidgetTarget->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        addItem(ui->tableWidgetTarget, i, points.at(i));
    }

    ui->tabWidget->setCurrentIndex(1);
}

void frmMapRoute::on_btnDrawData_clicked()
{
    if (routeDatas.count() == 0) {
        QtHelper::showMessageBoxError("请先单击查询路线获取路线的坐标点集合!");
        return;
    }

    //清空之前的轨迹点
    runJs("deleteOverlay('Polyline')");

    //将收到的路径点集合分线段绘制
    foreach (QStringList data, routeDatas) {
        QString points = data.join("|");
        QString js = QString("addPolyline('%1', '#ff0000')").arg(points);
        runJs(js);
    }
}

void frmMapRoute::on_cboxRouteType_currentIndexChanged(int index)
{
    QStringList policyType;
    if (index == 0) {
        policyType << "最少时间" << "最少换乘" << "最少步行" << "不乘地铁";
    } else {
        policyType << "最少时间" << "最短距离" << "避开高速";
    }

    ui->cboxPolicyType->clear();
    ui->cboxPolicyType->addItems(policyType);
}
