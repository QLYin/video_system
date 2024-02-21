#include "frmmap.h"
#include "ui_frmmap.h"
#include "qthelper.h"
#include "devicehelper.h"

#include "frmmapimage.h"
#include "frmmaplocal.h"
#include "frmmapweb.h"
#include "frmmaproute.h"

frmMap::frmMap(QWidget *parent) : QWidget(parent), ui(new Ui::frmMap)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmMap::~frmMap()
{
    delete ui;
}

void frmMap::initForm()
{
    ui->widgetLeft->setProperty("flag", "btnNavLeft");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
}

void frmMap::initWidget()
{
    ui->stackedWidget->addWidget(new frmMapImage);
    ui->stackedWidget->addWidget(new frmMapLocal);
    ui->stackedWidget->addWidget(new frmMapWeb);
    ui->stackedWidget->addWidget(new frmMapRoute);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmMap::initNav()
{
    QList<QString> names, texts;
    names << "btnMapImage" << "btnMapLocal" << "btnMapWeb" << "btnMapRoute";
    texts << "图 片 地 图" << "离 线 地 图" << "在 线 地 图" << "路 径 规 划";
    icons << 0xe757 << 0xe9c0 << 0xe6a3 << 0xe9c2;

    //根据设定实例化导航按钮对象
    for (int i = 0; i < texts.count(); ++i) {
        QToolButton *btn = new QToolButton;
        CommonNav::initNavBtn(btn, names.at(i), texts.at(i), true);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        ui->layoutNav->addWidget(btn);
        btns << btn;        
    }

    //底部加上弹簧顶上去
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->layoutNav->addItem(verticalSpacer);

    //自动打开上次的窗体
    int index = names.indexOf(AppConfig::LastFormMap);
    index >= 0 ? btns.at(index)->click() : btns.first()->click();

    //根据配置启用哪些模块
    btns.at(0)->setVisible(AppConfig::EnableMapImage);
}

void frmMap::initIcon()
{
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        CommonNav::initNavBtnIcon(btns.at(i), icons.at(i), true);
    }
}

void frmMap::buttonClicked()
{
    //切换到当前窗体
    QAbstractButton *btn = (QAbstractButton *)sender();
    ui->stackedWidget->setCurrentIndex(btns.indexOf(btn));

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormMap = btn->objectName();
    AppConfig::writeConfig();
}
