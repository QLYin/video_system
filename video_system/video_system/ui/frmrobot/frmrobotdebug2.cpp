#include "frmrobotdebug2.h"
#include "ui_frmrobotdebug2.h"
#include "qthelper.h"
#include "qthelperother.h"
#include "devicerobot.h"
#include "devicerobot2.h"

frmRobotDebug2::frmRobotDebug2(QWidget *parent) : QWidget(parent), ui(new Ui::frmRobotDebug2)
{
    ui->setupUi(this);
    this->initForm();
}

frmRobotDebug2::~frmRobotDebug2()
{
    delete ui;
}

void frmRobotDebug2::initForm()
{
    //字体放大 具体要多大可以自己改
    QFont font;
    font.setPixelSize(GlobalConfig::FontSize + 8);
    ui->txtMain->setFont(font);

    //关联机器人数据收发
    connect(DeviceRobot::Instance(), SIGNAL(sendData(QString, QByteArray)),
            this, SLOT(sendData(QString, QByteArray)));
    connect(DeviceRobot::Instance(), SIGNAL(receiveData(QString, QByteArray)),
            this, SLOT(receiveData(QString, QByteArray)));
    connect(DeviceRobot2::Instance(), SIGNAL(sendData(QString, QByteArray)),
            this, SLOT(sendData(QString, QByteArray)));
    connect(DeviceRobot2::Instance(), SIGNAL(receiveData(QString, QByteArray)),
            this, SLOT(receiveData(QString, QByteArray)));

    //消息类型映射表
    QtHelperOther::resetMsgMap();
    QtHelperOther::msgTypes << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7;
    QtHelperOther::msgKeys << "发送" << "发送" << "发送" << "发送" << "接收" << "接收" << "接收" << "接收";
    QtHelperOther::msgColors << "#00B17D" << "#47A4E9" << "#22A3A9" << "#56BEC1" << "#D64D54" << "#A279C5" << "#FD8B28" << "#D98078";

    //测试数据
    QByteArray data = QtHelper::hexStrToByteArray("11 22 66 88");
    sendData(AppConfig::PortNameA, data);
    sendData(AppConfig::PortNameB, data);
    sendData(AppConfig::PortNameC, data);
    sendData(AppConfig::PortNameD, data);
    receiveData(AppConfig::PortNameA, data);
    receiveData(AppConfig::PortNameB, data);
    receiveData(AppConfig::PortNameC, data);
    receiveData(AppConfig::PortNameD, data);
}

void frmRobotDebug2::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    bool pause = ui->ckPause->isChecked();
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, pause);
}

bool frmRobotDebug2::checkNotPrint(const QString &portName, int &type, QString &flag)
{
    type = 0;
    flag = QString("串口A");
    if (portName == AppConfig::PortNameB) {
        type = 1;
        flag = QString("串口B");
    } else if (portName == AppConfig::PortNameC) {
        type = 2;
        flag = QString("串口C");
    } else if (portName == AppConfig::PortNameD) {
        type = 3;
        flag = QString("串口D");
    }

    //没有选中的串口不用打印
    QString port = ui->cboxPort->currentText();
    bool notPrintA = (portName == AppConfig::PortNameA && flag != port);
    bool notPrintB = (portName == AppConfig::PortNameB && flag != port);
    bool notPrintC = (portName == AppConfig::PortNameC && flag != port);
    bool notPrintD = (portName == AppConfig::PortNameD && flag != port);
    if (port == "所有串口") {
        notPrintA = notPrintB = notPrintC = notPrintD = false;
    }
    return (notPrintA || notPrintB || notPrintC || notPrintD);
}

void frmRobotDebug2::sendData(const QString &portName, const QByteArray &data)
{
    int type;
    QString flag;
    if (checkNotPrint(portName, type, flag)) {
        return;
    }

    QString info = QString("%1 %2").arg(flag).arg(QtHelper::byteArrayToHexStr(data));
    append(0 + type, info);
}

void frmRobotDebug2::receiveData(const QString &portName, const QByteArray &data)
{
    int type;
    QString flag;
    if (checkNotPrint(portName, type, flag)) {
        return;
    }

    QString info = QString("%1 %2").arg(flag).arg(QtHelper::byteArrayToHexStr(data));
    append(4 + type, info);
}
