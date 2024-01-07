#include "frmrobotdata.h"
#include "ui_frmrobotdata.h"
#include "qthelper.h"
#include "dbquery.h"
#include "devicerobot.h"
#include "devicerobot2.h"
#include "videomanage.h"

frmRobotData::frmRobotData(QWidget *parent) : QWidget(parent), ui(new Ui::frmRobotData)
{
    ui->setupUi(this);
    this->initForm();
    this->initTable();
    this->initItem();
    this->loadData();
    QTimer::singleShot(1000, this, SLOT(initServer()));
    QTimer::singleShot(1000, this, SLOT(loadStatus()));
}

frmRobotData::~frmRobotData()
{
    delete ui;
}

void frmRobotData::initForm()
{
    //关联机器人数据解析信号
    connect(DeviceRobot::Instance(), SIGNAL(receiveData(quint8, QVariant)), this, SLOT(receiveData(quint8, QVariant)));
}

void frmRobotData::initServer()
{
    //打开机器人通信
    DeviceRobot::Instance()->open();
    DeviceRobot2::Instance()->openA();
    DeviceRobot2::Instance()->openB();
    DeviceRobot2::Instance()->openD();
}

void frmRobotData::initTable()
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "信息名称" << "数据值";
    columnWidths << 90 << 100;

    int columnCount = columnNames.count();
    ui->tableWidgetMsg->setColumnCount(columnCount);
    ui->tableWidgetMsg->setHorizontalHeaderLabels(columnNames);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidgetMsg->setColumnWidth(i, columnWidths.at(i));
    }

    QtHelper::initTableView(ui->tableWidgetMsg, AppData::RowHeight, false, false);
}

void frmRobotData::initItem()
{
    //初始化文本
    QStringList texts;
    texts << "当前经度" << "当前纬度" << "底盘速度" << "输送带状态"
          << "斜板抽屉状态" << "伸缩柜状态" << "运行模式" << "线性模组位置"
          << "大臂推杆位置" << "小臂推杆位置" << "斜板推杆位置" << "缓冲数据长度";

    int count = texts.count();
    ui->tableWidgetMsg->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        //设置名称
        QTableWidgetItem *itemName = new QTableWidgetItem;
        itemName->setText(texts.at(i));
        ui->tableWidgetMsg->setItem(i, 0, itemName);

        //设置值
        QTableWidgetItem *itemValue = new QTableWidgetItem;
        itemValue->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMsg->setItem(i, 1, itemValue);
    }
}

void frmRobotData::loadData()
{
    //动态填入对应值就行
    ui->tableWidgetMsg->item(0, 1)->setText("东经 121.7655689");
    ui->tableWidgetMsg->item(1, 1)->setText("北纬 27.6235894");
    ui->tableWidgetMsg->item(2, 1)->setText("10%");
    ui->tableWidgetMsg->item(3, 1)->setText("运转");
    ui->tableWidgetMsg->item(4, 1)->setText("静止");
    ui->tableWidgetMsg->item(5, 1)->setText("展开");
    ui->tableWidgetMsg->item(6, 1)->setText("手动模式");
    ui->tableWidgetMsg->item(7, 1)->setText("16%");
    ui->tableWidgetMsg->item(8, 1)->setText("0%");
    ui->tableWidgetMsg->item(9, 1)->setText("100%");
    ui->tableWidgetMsg->item(10, 1)->setText("50%");
    ui->tableWidgetMsg->item(11, 1)->setText("0");
}

void frmRobotData::loadStatus()
{
    //默认取第一个摄像机显示机器人工作状态
    VideoWidget *videoWidget = VideoManage::Instance()->getVideoWidgets().first();
    OsdInfo osd;
    osd.name = "status";
    osd.color = Qt::red;
    osd.fontSize = 30;
    osd.format = OsdFormat_Text;
    osd.position = OsdPosition_LeftBottom;
    osd.text = "机器人状态: 运行中";
    videoWidget->setOsd(osd);
}

void frmRobotData::receiveData(quint8 type, const QVariant &data)
{
    QString result = data.toString();
    QStringList results = result.split("|");

    //根据不同的类型显示对应的数据
    if (type == 0x01) {
        //更新对应的经纬度
        if (results.count() == 4) {
            QString lng = QString("%1.%2").arg(results.at(0)).arg(results.at(1));
            QString lat = QString("%1.%2").arg(results.at(2)).arg(results.at(3));
            ui->tableWidgetMsg->item(0, 1)->setText("东经 " + lng);
            ui->tableWidgetMsg->item(1, 1)->setText("北纬 " + lat);

            //触发机器人位置移动
            //QString position = QString("%1|%2").arg(lng).arg(lat);
            //DbData::IpcInfo_IpcPosition[0] = position;
            //AppEvent::Instance()->slot_saveIpcInfo();
            //DbQuery::addUserLog("", "设备日志", "机器人位置移动", QString("经纬度: %1").arg(position));

            //通知界面机器人移动
            emit moveDevice(0, lng, lat);
        }
    } else if (type == 0x02) {
        //更新底盘速度
        ui->tableWidgetMsg->item(2, 1)->setText(result + "%");
    } else if (type == 0x03) {
        //更新输送带+斜板抽屉+伸缩柜状态+运行模式
        if (results.count() == 4) {
            ui->tableWidgetMsg->item(3, 1)->setText(results.at(0));
            ui->tableWidgetMsg->item(4, 1)->setText(results.at(1));
            ui->tableWidgetMsg->item(5, 1)->setText(results.at(2));
            ui->tableWidgetMsg->item(6, 1)->setText(results.at(3));
        }
    } else if (type == 0x04) {
        //更新线性模组+大臂电动推杆+小臂电动推杆+斜板电动推杆位置
        if (results.count() == 4) {
            ui->tableWidgetMsg->item(7, 1)->setText(results.at(0) + "%");
            ui->tableWidgetMsg->item(8, 1)->setText(results.at(1) + "%");
            ui->tableWidgetMsg->item(9, 1)->setText(results.at(2) + "%");
            ui->tableWidgetMsg->item(10, 1)->setText(results.at(3) + "%");
        }
    } else {
        ui->tableWidgetMsg->item(11, 1)->setText(result);
    }
}
