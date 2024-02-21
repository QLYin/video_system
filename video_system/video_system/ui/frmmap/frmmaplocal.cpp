#include "frmmaplocal.h"
#include "ui_frmmaplocal.h"
#include "frmmapdevice.h"

frmMapLocal::frmMapLocal(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapLocal)
{
    ui->setupUi(this);
    this->initForm();
}

frmMapLocal::~frmMapLocal()
{
    delete ui;
}

void frmMapLocal::initForm()
{
    //实例化公用的设备地图界面并加入到布局
    frmMapDevice *map = new frmMapDevice(this);
    ui->verticalLayout->addWidget(map);
    //设置地图模式 在线或者离线
    map->setMapLocal(true);
}
