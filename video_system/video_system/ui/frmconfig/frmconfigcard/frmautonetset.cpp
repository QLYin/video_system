#include "frmautonetset.h"
#include "ui_frmautonetset.h"
#include "qthelper.h"
#include "dbquery.h"
#include "frmmain.h"

frmAutoNetSet::frmAutoNetSet(QWidget *parent) : QDialog(parent), ui(new Ui::frmAutoNetSet)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmAutoNetSet::~frmAutoNetSet()
{
    delete ui;
}

void frmAutoNetSet::initStyle()
{
    resize(400, 200);
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void frmAutoNetSet::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmAutoNetSet::initForm()
{
    
}

void frmAutoNetSet::initIcon()
{
}
