#include "frmscreencharset.h"
#include "ui_frmscreencharset.h"
#include "qthelper.h"
#include "dbquery.h"
#include "frmmain.h"

frmScreenCharSet::frmScreenCharSet(QWidget *parent) : QDialog(parent), ui(new Ui::frmScreenCharSet)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmScreenCharSet::~frmScreenCharSet()
{
    delete ui;
}

void frmScreenCharSet::initStyle()
{
    resize(400, 200);
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void frmScreenCharSet::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmScreenCharSet::initForm()
{
    
}

void frmScreenCharSet::initIcon()
{
}
