#include "frmipcadddialog.h"
#include "ui_frmipcadddialog.h"
#include "qthelper.h"
#include "dbquery.h"
#include "ui/frmbase/Indicator.h"

frmIpcAddDialog::frmIpcAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::frmIpcAddDialog)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmIpcAddDialog::~frmIpcAddDialog()
{
    delete ui;
}

void frmIpcAddDialog::initStyle()
{
    //resize(360, 400);
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancle, SIGNAL(clicked()), this, SLOT(close()));
}

void frmIpcAddDialog::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmIpcAddDialog::initForm()
{
    QStringList brands;
    brands << "海康" << "华为" << "大华" << "雄迈" << "天视通";
    ui->cbBoxIpcBrand->addItems(brands);
    ui->cbBoxIpcBrand->setCurrentIndex(1);

    ui->lineEditPort->setText("554");

    QStringList mainResolutions;
    mainResolutions <<  "1200W" << "800W" << "600W" << "500W" << "400W" << "300W" << "1080P" << "960P" << "720P" << "D1" << "CIF";
    ui->cbBoxMainResolution->addItems(mainResolutions);
    ui->cbBoxMainResolution->setCurrentIndex(6);

    QStringList subResolutions;
    subResolutions << "自动" << "D1" << "CIF"; 
    ui->cbBoxSubResolution->addItems(subResolutions);
    ui->cbBoxSubResolution->setCurrentIndex(1);

    connect(ui->btnAddIpc, SIGNAL(clicked()), this, SLOT(onBtnAddIpcClicked()));
}

void frmIpcAddDialog::initIcon()
{
}


void frmIpcAddDialog::onBtnAddIpcClicked()
{
    //先校验数据合法性
    QString startIp = ui->lineEditStartIP->text().trimmed();
    QString endIp = ui->lineEditEndIP->text().trimmed();
    if (!QtHelper::isIP(startIp) || !QtHelper::isIP(endIp))
    {
        Indicator::showTopTip(QString::fromLocal8Bit("非法的ip地址"), this);
        return;
    }

    QString port = ui->lineEditPort->text().trimmed();
    if (port.toInt() < 0 || port.toInt() > 65537)
    {
        Indicator::showTopTip(QString::fromLocal8Bit("非法的端口号"), this);
        return;
    }

    // 添加逻辑
    deviceInfos.clear();

    quint32 startIpAddr = QtHelper::ipv4StringToInt(startIp);
    quint32 endIpAddr = QtHelper::ipv4StringToInt(endIp);

    for (quint32 ip = startIpAddr; ip <= endIpAddr; ++ip) {
        QString ipString = QString("%1.%2.%3.%4")
            .arg((ip >> 24) & 0xFF)
            .arg((ip >> 16) & 0xFF)
            .arg((ip >> 8) & 0xFF)
            .arg(ip & 0xFF);

        // db已有的过滤
        for (int j = 0; j < DbData::IpcInfo_Count; ++j) {
            QString addr = DbData::IpcInfo_IpAddr.at(j);
            if (addr == ipString)
            {
                continue;
            }

        }

        QStringList item;
        item << ui->lineEditUsrName->text().trimmed() << ui->lineEditPwd->text().trimmed() << "";
        item << ipString;
        item << QString("http://%1/onvif/device_service").arg(ipString);
        item << "" << "";
        item << QString("rtsp://%1:%2/h264/ch1/main/av_stream").arg(ipString).arg(port);
        item << QString("rtsp://%1:%2/h264/ch1/sub/av_stream").arg(ipString).arg(port);
        item << ui->cbBoxMainResolution->currentText().trimmed() << ui->cbBoxSubResolution->currentText().trimmed();
        deviceInfos.append(item);
    }

    emit addDevices(deviceInfos);
    hide();
}