#include "frmipcnetconfig.h"
#include "ui_frmipcnetconfig.h"
#include "qthelper.h"
#include "deviceonvif.h"

frmIpcNetConfig::frmIpcNetConfig(QWidget *parent) : QWidget(parent), ui(new Ui::frmIpcNetConfig)
{
    ui->setupUi(this);
    this->initForm();
    QtHelper::setFormInCenter(this);
}

frmIpcNetConfig::~frmIpcNetConfig()
{
    delete ui;
}

void frmIpcNetConfig::initForm()
{
    this->setWindowTitle("网络配置");
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setFixedSize(this->size());
}

void frmIpcNetConfig::on_btnGetNetConfig_clicked()
{
    //调用函数获取网络配置
    OnvifNetConfig netConfig = DeviceOnvif::getNetConfig(AppData::CurrentUrl);
    ui->txtHostName->setText(netConfig.hostName);
    ui->cboxIpDhcp->setCurrentIndex(netConfig.ipDhcp == "true" ? 1 : 0);
    ui->txtIpAddress->setText(netConfig.ipAddress);
    ui->txtMask->setText(netConfig.mask);
    ui->txtGateway->setText(netConfig.gateway);
    ui->txtDns->setText(netConfig.dns);
}

void frmIpcNetConfig::on_btnSetNetConfig_clicked()
{
    //挨个从界面取值
    OnvifNetConfig netConfig;
    netConfig.hostName = ui->txtHostName->text().trimmed();
    netConfig.ipDhcp = (ui->cboxIpDhcp->currentIndex() == 1 ? "true" : "false");
    netConfig.ipAddress = ui->txtIpAddress->text().trimmed();
    netConfig.mask = ui->txtMask->text().trimmed();
    netConfig.gateway = ui->txtGateway->text().trimmed();
    netConfig.dns = ui->txtDns->text().trimmed();
    if (DeviceOnvif::setNetConfig(AppData::CurrentUrl, netConfig)) {
        on_btnClearData_clicked();
        QtHelper::showMessageBoxInfo("设置网络参数成功, 请重新搜索设备并更新设备!");
    }
}

void frmIpcNetConfig::on_btnClearData_clicked()
{
    QList<QLineEdit *> txts = ui->frame->findChildren<QLineEdit *>();
    foreach (QLineEdit *txt, txts) {
        txt->clear();
    }
}

