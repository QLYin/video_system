#include "frmwelcome.h"
#include "ui_frmwelcome.h"
#include "qthelper.h"

frmWelcome::frmWelcome(QWidget *parent) : QWidget(parent), ui(new Ui::frmWelcome)
{
    ui->setupUi(this);
    this->initForm();
}

frmWelcome::~frmWelcome()
{
    delete ui;
}

void frmWelcome::initForm()
{
    ui->lcdNumber->setFormat("HH:mm:ss");
    ui->lcdNumber->setStyleSheet("border:none;");

    QFont font;
    //font.setBold(true);
    font.setPixelSize(GlobalConfig::FontSize + 2);
    ui->labWelcome->setFont(font);
    ui->labWelcome->setText("欢迎您: " + UserHelper::CurrentUserName);
}
