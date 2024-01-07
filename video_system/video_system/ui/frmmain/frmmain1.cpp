#include "frmmain1.h"
#include "ui_frmmain1.h"
#include "frmrobotlog.h"
#include "qthelper.h"

frmMain1::frmMain1(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain1)
{
    ui->setupUi(this);
    this->initForm();
}

frmMain1::~frmMain1()
{
    delete ui;
}

void frmMain1::initForm()
{
    if (AppConfig::WorkMode == 1) {
        frmRobotLog *log = new frmRobotLog;
        this->layout()->addWidget(log);
    }
}
