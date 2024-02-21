#include "frmrobotemulate.h"
#include "ui_frmrobotemulate.h"

frmRobotEmulate::frmRobotEmulate(QWidget *parent) : QWidget(parent), ui(new Ui::frmRobotEmulate)
{
    ui->setupUi(this);
}

frmRobotEmulate::~frmRobotEmulate()
{
    delete ui;
}
