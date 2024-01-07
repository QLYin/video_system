#include "frmconfig.h"
#include "ui_frmconfig.h"
#include "qthelper.h"
#include "devicehelper.h"

#include "frmconfigsystem.h"
#include "frmconfignvr.h"
#include "frmconfigipc.h"
#include "frmconfigpoll.h"
#include "frmconfiguser.h"
#include "frmconfigrecord.h"
#include "frmconfigother.h"

frmConfig::frmConfig(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfig)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmConfig::~frmConfig()
{
    delete ui;
}

void frmConfig::initForm()
{
    ui->widgetLeft->setProperty("flag", "btnNavLeft");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
}

void frmConfig::initWidget()
{
    ui->stackedWidget->addWidget(new frmConfigSystem);

    //录像机管理+摄像机管理 单独new出来是为了方便这里绑定信号槽
    frmConfigNvr *configNvr = new frmConfigNvr;
    ui->stackedWidget->addWidget(configNvr);

    frmConfigIpc *configIpc = new frmConfigIpc;
    ui->stackedWidget->addWidget(configIpc);

    ui->stackedWidget->addWidget(new frmConfigPoll);
    ui->stackedWidget->addWidget(new frmConfigUser);
    ui->stackedWidget->addWidget(new frmConfigRecord);
    ui->stackedWidget->addWidget(new frmConfigOther);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(configNvr, SIGNAL(nvrNameChanged()), configIpc, SLOT(nvrNameChanged()));
}

void frmConfig::initNav()
{
    QList<QString> names, texts;
    names << "btnConfigSystem" << "btnConfigNvr" << "btnConfigIpc" << "btnConfigPoll" << "btnConfigUser" << "btnConfigRecord" << "btnConfigOther";
    texts << "基 本 设 置" << "录像机管理" << "摄像机管理" << "轮 询 配 置" << "用 户 管 理" << "录 像 计 划" << "其 他 设 置";
    icons << 0xf085 << 0xe9e8 << 0xe9f5 << 0xea14 << 0xea59 << 0xe6da << 0xf013;

    if (AppConfig::WorkMode == 3) {
        texts.clear();
        texts << "基 本 设 置" << "服务器管理" << "实训仪管理" << "实 训 计 划" << "用 户 管 理" << "录 像 计 划" << "其 他 设 置";
    }

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
    int index = names.indexOf(AppConfig::LastFormConfig);
    index >= 0 ? btns.at(index)->click() : btns.first()->click();

    //根据配置启用哪些模块
    btns.at(3)->setVisible(AppConfig::EnableConfigPoll);
    btns.at(4)->setVisible(AppConfig::EnableConfigUser);
    btns.at(5)->setVisible(AppConfig::EnableConfigRecord);
    btns.at(6)->setVisible(AppConfig::EnableConfigOther);
}

void frmConfig::initIcon()
{
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        CommonNav::initNavBtnIcon(btns.at(i), icons.at(i), true);
    }
}

void frmConfig::buttonClicked()
{
    //判断是否有对应模块的权限
    QAbstractButton *btn = (QAbstractButton *)sender();
    if (!UserHelper::checkPermission(btn->text())) {
        btn->setChecked(false);
        //切换到永远有权限的页面
        QMetaObject::invokeMethod(btns.at(0), "clicked");
        return;
    }

    //切换到当前窗体
    ui->stackedWidget->setCurrentIndex(btns.indexOf(btn));

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormConfig = btn->objectName();
    AppConfig::writeConfig();
}
