#include "frmipceditdialog.h"
#include "ui_frmipceditdialog.h"
#include "qthelper.h"
#include "dbquery.h"
#include "ui/frmbase/Indicator.h"

frmIpcEditDialog::frmIpcEditDialog(QWidget *parent) : QDialog(parent), ui(new Ui::frmIpcEditDialog)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmIpcEditDialog::~frmIpcEditDialog()
{
    delete ui;
}

void frmIpcEditDialog::initStyle()
{
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancle, SIGNAL(clicked()), this, SLOT(close()));
}

void frmIpcEditDialog::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmIpcEditDialog::initForm()
{
    QStringList types;
    types << "枪机" << "球机" ;
    ui->cbBoxType->addItems(types);
    ui->cbBoxType->setCurrentIndex(0);

    QStringList detects;
    detects << "开启" << "关闭";
    ui->cbBoxDetect->addItems(detects);
    ui->cbBoxDetect->setCurrentIndex(1);

    QStringList mainResolutions;
    mainResolutions << "自动" << "1200W" << "800W" << "600W" << "500W" << "400W" << "300W" << "1080P" << "960P" << "720P" << "D1" << "CIF";
    ui->cbBoxMainStream->addItems(mainResolutions);
    ui->cbBoxMainStream->setCurrentIndex(7);


    QStringList subResolutions;
    subResolutions << "自动" << "D1" << "CIF";
    ui->cbBoxSubStream->addItems(subResolutions);
    ui->cbBoxSubStream->setCurrentIndex(1);

    connect(ui->btnConfirm, &QPushButton::clicked, this, &frmIpcEditDialog::onConfirmClicked);
}

void frmIpcEditDialog::initIcon()
{
}

void frmIpcEditDialog::initData(QStringList data)
{
    if (data.isEmpty())
    {
        return;
    }

    m_id = data.at(0).trimmed().toInt();
    ui->lineEditID->setText(data.at(0).trimmed());
    ui->lineEditName->setText(data.at(1).trimmed());
    ui->lineEditIP->setText(data.at(2).trimmed());
    ui->lineEditUsrName->setText(data.at(3).trimmed());
    ui->lineEditPwd->setText(data.at(4).trimmed());
    ui->cbBoxMainStream->setCurrentText(data.at(5).trimmed());
    ui->cbBoxSubStream->setCurrentText(data.at(6).trimmed());

}

void frmIpcEditDialog::onConfirmClicked()
{
    QStringList deviceInfo;
    deviceInfo << ui->lineEditID->text().trimmed();
    deviceInfo << ui->lineEditName->text().trimmed();
    deviceInfo << ui->lineEditIP->text().trimmed();
    deviceInfo << ui->lineEditUsrName->text().trimmed();
    deviceInfo << ui->lineEditPwd->text().trimmed();
    deviceInfo << ui->cbBoxMainStream->currentText().trimmed();
    deviceInfo << ui->cbBoxSubStream->currentText().trimmed();
    deviceInfo << ui->lineEditPTZ->text().trimmed();

    emit editDevice(m_id, deviceInfo);
    hide();
}