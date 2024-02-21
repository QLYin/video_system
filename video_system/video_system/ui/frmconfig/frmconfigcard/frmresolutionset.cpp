#include "frmresolutionset.h"
#include "ui_frmresolutionset.h"
#include "qthelper.h"
#include "dbquery.h"
#include "frmmain.h"

frmResolutionSet::frmResolutionSet(QWidget *parent) : QDialog(parent), ui(new Ui::frmResolutionSet)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmResolutionSet::~frmResolutionSet()
{
    delete ui;
}

void frmResolutionSet::initStyle()
{
    resize(360, 400);
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancle, SIGNAL(clicked()), this, SLOT(close()));
}

void frmResolutionSet::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmResolutionSet::initForm()
{
  
}

void frmResolutionSet::initIcon()
{
}
