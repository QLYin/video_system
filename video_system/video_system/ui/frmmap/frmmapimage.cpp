#include "frmmapimage.h"
#include "ui_frmmapimage.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "devicebutton.h"

frmMapImage::frmMapImage(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapImage)
{
    ui->setupUi(this);
    this->initForm();
    if (AppConfig::EnableMapImage) {
        this->initImage();
    }
}

frmMapImage::~frmMapImage()
{
    delete ui;
}

void frmMapImage::hideEvent(QHideEvent *)
{
    //隐藏界面的时候自动保存按钮位置
    DeviceHelper::saveDeviceButtonPosition();
}

void frmMapImage::initForm()
{
    ui->listWidget->setFixedWidth(AppData::RightWidth);
    connect(AppEvent::Instance(), SIGNAL(doubleClicked(DeviceButton *)), this, SLOT(doubleClicked(DeviceButton *)));
}

void frmMapImage::initImage()
{
    //初始化地图缩略图
    DeviceHelper::setListWidget(ui->listWidget);
    DeviceHelper::initDeviceMap();

    //初始化设备按钮
    DeviceHelper::setLabel(ui->label);
    DeviceHelper::initDeviceButton();

    //默认选中一张图
    ui->listWidget->setCurrentRow(AppData::MapNames.indexOf(AppData::CurrentImage));
    on_listWidget_pressed();
}

void frmMapImage::doubleClicked(DeviceButton *btn)
{
    //取出视频流地址打开
    QString url = btn->property("rtspSub").toString();
    DeviceHelper::showVideo(url);
}

void frmMapImage::on_listWidget_pressed()
{
    int row = ui->listWidget->currentRow();
    if (row < 0) {
        return;
    }

    DeviceHelper::initDeviceMapCurrent(AppData::MapNames.at(row));
}
