#include "frmdata.h"
#include "ui_frmdata.h"
#include "qthelper.h"

#include "frmdatauser.h"
#include "frmdatadevice.h"

frmData::frmData(QWidget *parent) : QWidget(parent), ui(new Ui::frmData)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmData::~frmData()
{
    delete ui;
}

void frmData::initForm()
{
    ui->widgetLeft->setProperty("flag", "btnNavLeft");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
}

void frmData::initWidget()
{
    ui->stackedWidget->addWidget(new frmDataUser);
    ui->stackedWidget->addWidget(new frmDataDevice);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));        
}

void frmData::initNav()
{
    QList<QString> names, texts;
    names << "btnDataUser" << "btnDataDevice";
    texts << "本 地 日 志" << "设 备 日 志";
    icons << 0xe6ac << 0xe6ae;

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
    int index = names.indexOf(AppConfig::LastFormData);
    index >= 0 ? btns.at(index)->click() : btns.first()->click();
}

void frmData::initIcon()
{
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        CommonNav::initNavBtnIcon(btns.at(i), icons.at(i), true);
    }
}

void frmData::buttonClicked()
{
    //切换到当前窗体
    QAbstractButton *btn = (QAbstractButton *)sender();
    ui->stackedWidget->setCurrentIndex(btns.indexOf(btn));

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormData = btn->objectName();
    AppConfig::writeConfig();
}
