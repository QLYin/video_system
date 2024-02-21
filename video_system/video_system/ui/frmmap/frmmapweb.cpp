#include "frmmapweb.h"
#include "ui_frmmapweb.h"
#include "frmmapdevice.h"

frmMapWeb::frmMapWeb(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapWeb)
{
    ui->setupUi(this);
    this->initForm();
}

frmMapWeb::~frmMapWeb()
{
    delete ui;
}

void frmMapWeb::initForm()
{
    //实例化公用的设备地图界面并加入到布局
    frmMapDevice *map = new frmMapDevice;
    ui->verticalLayout->addWidget(map);
    //设置地图模式 在线或者离线
    map->setMapLocal(false);
}
