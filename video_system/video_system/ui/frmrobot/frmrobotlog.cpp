#include "frmrobotlog.h"
#include "ui_frmrobotlog.h"

frmRobotLog::frmRobotLog(QWidget *parent) : QWidget(parent), ui(new Ui::frmRobotLog)
{
    ui->setupUi(this);
}

frmRobotLog::~frmRobotLog()
{
    delete ui;
}
