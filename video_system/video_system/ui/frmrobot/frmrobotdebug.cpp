#include "frmrobotdebug.h"
#include "ui_frmrobotdebug.h"
#include "qthelper.h"
#include "devicerobot.h"

frmRobotDebug::frmRobotDebug(QWidget *parent) : QWidget(parent), ui(new Ui::frmRobotDebug)
{
    ui->setupUi(this);
    this->initForm();
}

frmRobotDebug::~frmRobotDebug()
{
    delete ui;
}

void frmRobotDebug::initForm()
{
    ui->txtLngLat->setText(AppConfig::MapCenter);

    //关联机器人数据收发
    connect(DeviceRobot::Instance(), SIGNAL(sendData(QString, QByteArray)),
            this, SLOT(sendData(QString, QByteArray)));
    connect(DeviceRobot::Instance(), SIGNAL(receiveData(QString, QByteArray)),
            this, SLOT(receiveData(QString, QByteArray)));

    //关联机器人经纬度改变信号
    connect(AppEvent::Instance(), SIGNAL(receiveLngLat(QString)), this, SLOT(receiveLngLat(QString)));
}

void frmRobotDebug::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    bool pause = ui->ckPause->isChecked();
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, pause);
}

void frmRobotDebug::sendData(const QString &portName, const QByteArray &data)
{
    append(0, QtHelper::byteArrayToHexStr(data));
}

void frmRobotDebug::receiveData(const QString &portName, const QByteArray &data)
{
    append(1, QtHelper::byteArrayToHexStr(data));
}

void frmRobotDebug::receiveLngLat(const QString &lnglat)
{
    ui->txtLngLat->setText(lnglat);
}

void frmRobotDebug::on_btnSend_clicked()
{
    //先发送经度再发送纬度数据
    DeviceRobot::Instance()->testData(ui->txtLng->text());
    DeviceRobot::Instance()->testData(ui->txtLat->text());
}

void frmRobotDebug::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmRobotDebug::on_txtLngLat_textChanged(const QString &arg1)
{
    if (!arg1.contains(",")) {
        return;
    }

    //根据规则自动计算要发送的数据
    //7F 09 11 4E 1D 37 9D 2C B2
    //0x11=纬度信息 信息包5字节 北纬或南纬 纬度值 3字节小数值
    //0x12=经度信息 信息包5字节 西经或东经 经度值 3字节小数值
    QStringList list = arg1.split(",");
    //("116", "358539") ("39", "986091")
    QString lng = list.at(0);
    QString lat = list.at(1);
    QStringList listLng = lng.split(".");
    QStringList listLat = lat.split(".");

    //取出整数和小数值,不足补零
    QString lngValue = QString::number(listLng.first().toInt(), 16);
    QString latValue = QString::number(listLat.first().toInt(), 16);
    QString lngDecimal = QString::number(listLng.last().toInt(), 16);
    QString latDecimal = QString::number(listLat.last().toInt(), 16);
    lngValue = QString("%1").arg(lngValue, 2, QChar('0'));
    latValue = QString("%1").arg(latValue, 2, QChar('0'));
    lngDecimal = QString("%1").arg(lngDecimal, 6, QChar('0'));
    latDecimal = QString("%1").arg(latDecimal, 6, QChar('0'));

    QString dataLng = QString("7F 09 12 57 %1 %2 %3 %4 B2").arg(lngValue)
                      .arg(lngDecimal.mid(0, 2)).arg(lngDecimal.mid(2, 2)).arg(lngDecimal.mid(4, 2));
    QString dataLat = QString("7F 09 11 4E %1 %2 %3 %4 B2").arg(latValue)
                      .arg(latDecimal.mid(0, 2)).arg(latDecimal.mid(2, 2)).arg(latDecimal.mid(4, 2));
    dataLng = dataLng.toUpper();
    dataLat = dataLat.toUpper();
    ui->txtLng->setText(dataLng);
    ui->txtLat->setText(dataLat);
    //qDebug() << TIMEMS << lngValue << lngDecimal << dataLng;
    //qDebug() << TIMEMS << latValue << latDecimal << dataLat;
}
