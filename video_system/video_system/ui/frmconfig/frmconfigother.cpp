#include "frmconfigother.h"
#include "ui_frmconfigother.h"
#include "qthelper.h"
#include "commonkey.h"
#include "devicerobot.h"
#include "devicerobot2.h"
#include "videohelper.h"

frmConfigOther::frmConfigOther(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigOther)
{
    ui->setupUi(this);
    this->initForm();
}

frmConfigOther::~frmConfigOther()
{
    delete ui;
}

void frmConfigOther::initForm()
{
    //设置左侧右侧面板的宽度
    ui->widgetLeft->setFixedWidth(480);
    ui->widgetRight->setFixedWidth(450);

    this->initComConfig();
    this->initNetConfig();
}

void frmConfigOther::initComConfig()
{
    QStringList listPortName;
    for (int i = 1; i <= 15; ++i) {
        listPortName << QString("COM%1").arg(i);
    }

    listPortName << "ttyS0" << "ttyS1" << "ttyS2" << "ttyUSB0";

    ui->cboxPortNameA->addItems(listPortName);
    ui->cboxPortNameB->addItems(listPortName);
    ui->cboxPortNameC->addItems(listPortName);
    ui->cboxPortNameD->addItems(listPortName);

    ui->cboxPortNameA->setCurrentIndex(ui->cboxPortNameA->findText(AppConfig::PortNameA));
    ui->cboxPortNameB->setCurrentIndex(ui->cboxPortNameB->findText(AppConfig::PortNameB));
    ui->cboxPortNameC->setCurrentIndex(ui->cboxPortNameC->findText(AppConfig::PortNameC));
    ui->cboxPortNameD->setCurrentIndex(ui->cboxPortNameD->findText(AppConfig::PortNameD));

    connect(ui->cboxPortNameA, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxPortNameB, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxPortNameC, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxPortNameD, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));

    QStringList listBaudRate;
    listBaudRate << "4800" << "9600" << "115200";

    ui->cboxBaudRateA->addItems(listBaudRate);
    ui->cboxBaudRateB->addItems(listBaudRate);
    ui->cboxBaudRateC->addItems(listBaudRate);
    ui->cboxBaudRateD->addItems(listBaudRate);

    ui->cboxBaudRateA->setCurrentIndex(ui->cboxBaudRateA->findText(QString::number(AppConfig::BaudRateA)));
    ui->cboxBaudRateB->setCurrentIndex(ui->cboxBaudRateB->findText(QString::number(AppConfig::BaudRateB)));
    ui->cboxBaudRateC->setCurrentIndex(ui->cboxBaudRateC->findText(QString::number(AppConfig::BaudRateC)));
    ui->cboxBaudRateD->setCurrentIndex(ui->cboxBaudRateD->findText(QString::number(AppConfig::BaudRateD)));

    connect(ui->cboxBaudRateA, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxBaudRateB, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxBaudRateC, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
    connect(ui->cboxBaudRateD, SIGNAL(currentIndexChanged(int)), this, SLOT(saveComConfig()));
}

void frmConfigOther::saveComConfig()
{
    //可以动态应用串口号波特率
    QString portNameC = ui->cboxPortNameC->currentText();
    if (AppConfig::PortNameC != portNameC) {
        AppConfig::PortNameC = portNameC;
        DeviceRobot::Instance()->open();
    }

    int baudRateC = ui->cboxBaudRateC->currentText().toInt();
    if (AppConfig::BaudRateC != baudRateC) {
        AppConfig::BaudRateC = baudRateC;
        DeviceRobot::Instance()->open();
    }

    QString portNameA = ui->cboxPortNameA->currentText();
    if (AppConfig::PortNameA != portNameA) {
        AppConfig::PortNameA = portNameA;
        DeviceRobot2::Instance()->openA();
    }

    int baudRateA = ui->cboxBaudRateA->currentText().toInt();
    if (AppConfig::BaudRateA != baudRateA) {
        AppConfig::BaudRateA = baudRateA;
        DeviceRobot2::Instance()->openA();
    }

    QString portNameB = ui->cboxPortNameB->currentText();
    if (AppConfig::PortNameB != portNameB) {
        AppConfig::PortNameB = portNameB;
        DeviceRobot2::Instance()->openB();
    }

    int baudRateB = ui->cboxBaudRateB->currentText().toInt();
    if (AppConfig::BaudRateB != baudRateB) {
        AppConfig::BaudRateB = baudRateB;
        DeviceRobot2::Instance()->openB();
    }

    QString portNameD = ui->cboxPortNameD->currentText();
    if (AppConfig::PortNameD != portNameD) {
        AppConfig::PortNameD = portNameD;
        DeviceRobot2::Instance()->openD();
    }

    int baudRateD = ui->cboxBaudRateD->currentText().toInt();
    if (AppConfig::BaudRateD != baudRateD) {
        AppConfig::BaudRateD = baudRateD;
        DeviceRobot2::Instance()->openD();
    }

    AppConfig::writeConfig();
}

void frmConfigOther::initNetConfig()
{
    ui->txtTcpServerHost->setText(AppConfig::TcpServerHost);
    connect(ui->txtTcpServerHost, SIGNAL(textChanged(QString)), this, SLOT(saveNetConfig()));

    ui->txtTcpServerPort->setText(QString::number(AppConfig::TcpServerPort));
    connect(ui->txtTcpServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveNetConfig()));

    ui->txtTcpListenPort->setText(QString::number(AppConfig::TcpListenPort));
    connect(ui->txtTcpListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveNetConfig()));

    ui->txtUdpListenPort->setText(QString::number(AppConfig::UdpListenPort));
    connect(ui->txtUdpListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveNetConfig()));
}

void frmConfigOther::saveNetConfig()
{
    AppConfig::TcpServerHost = ui->txtTcpServerHost->text().trimmed();
    AppConfig::TcpServerPort = ui->txtTcpServerPort->text().toInt();
    AppConfig::TcpListenPort = ui->txtTcpListenPort->text().toInt();
    AppConfig::UdpListenPort = ui->txtUdpListenPort->text().toInt();
    AppConfig::writeConfig();
}

void frmConfigOther::on_btnComImage_clicked()
{
    QString fileName = QtHelper::appPath() + "/config/device/device_com.png";
    QPixmap pix(fileName);

    //如果图片分辨率大于设定的分辨率则等比缩放
    int width = 1100;
    int height = 700;
    if (pix.width() > width || pix.height() > height) {
        pix = pix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    static QLabel *labImage = 0;
    if (!labImage) {
        labImage = new QLabel;
    }

    labImage->setPixmap(pix);
    labImage->setWindowFlags(Qt::WindowStaysOnTopHint);
    labImage->setWindowTitle("串口通信接口数据传输示意图");
    labImage->setPalette(Qt::white);
    labImage->setFixedSize(width, height);
    labImage->show();
    QtHelper::setFormInCenter(labImage);
}

void frmConfigOther::on_btnInfo_clicked()
{
    //浏览器内核字符串
    QString coreWebview = "none";
    {
#ifdef webminiblink
        coreWebview = "miniblink";
#elif webengine
        coreWebview = "webengine";
#elif webkit
        coreWebview = "webkit";
#endif
    }

    //监控内核字符串
    QString coreVideo = VideoHelper::getVersion();
    QString compilerString = QtHelper::getCompilerString();
    QString version = QString("%1 / %2 %3 bit").arg(qVersion()).arg(compilerString).arg(QString::number(QSysInfo::WordSize));

    //弹框显示版本号等信息
    QStringList list;
    list << QString("版本: %1 / %2 x %3").arg(AppData::Version).arg(QtHelper::deskWidth()).arg(QtHelper::deskHeight());
    //list << QString("框架版本: 基于Qt %1").arg(QT_VERSION_STR);
    list << QString("框架: 基于Qt %1").arg(version);
    list << QString("内核: 监控(%1) 网页(%2)").arg(coreVideo).arg(coreWebview);

    QString key1 = "数量(无限制)";
    if (CommonKey::commonKey->keyUseCount) {
        key1 = QString("数量(%1)").arg(CommonKey::commonKey->keyCount);
    }

    QString key2 = "运行(无限制)";
    if (CommonKey::commonKey->keyUseRun) {
        key2 = QString("运行(%1分钟)").arg(CommonKey::commonKey->keyRun);
    }

    QString key3 = "到期(无限制)";
    if (CommonKey::commonKey->keyUseDate) {
        key3 = QString("到期(%1)").arg(CommonKey::commonKey->keyDate);
    }

    list << QString("秘钥: %1 %2 %3").arg(key1).arg(key2).arg(key3);
    QtHelper::showMessageBoxInfo(list.join("\n"));
}
