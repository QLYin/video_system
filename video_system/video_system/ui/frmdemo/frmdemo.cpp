#include "frmdemo.h"
#include "ui_frmdemo.h"
#include "qthelper.h"
#include "videohelper.h"

#include "frmdemohelper.h"
#include "frmdemovideoimage.h"
#include "frmdemovideowidget.h"
#include "frmdemovideosave.h"
#include "frmdemovideoplus.h"
#include "frmdemovideoplay4.h"
#include "frmdemovideovolume.h"
#include "frmdemovideodrag.h"
#include "frmdemovideochange.h"
#include "frmdemovideodraw.h"
#include "frmdemovideoosd.h"

frmDemo::frmDemo(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemo)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmDemo::~frmDemo()
{
    delete ui;
}

void frmDemo::closeEvent(QCloseEvent *)
{
    exit(0);
}

void frmDemo::initForm()
{
#if 0
    //作为独立的项目使用的时候设置
    QFont font;
    font.setFamily(QString::fromUtf8("微软雅黑"));
    font.setPixelSize(12);
    qApp->setFont(font);
    qApp->setPalette(QPalette(QColor(4, 7, 38)));
#endif

    ui->widgetLeft->setFixedWidth(120);
    ui->widgetLeft->setProperty("flag", "left");
    //this->setFixedSize(1050, 700);

    //关联全局换肤自动更新左侧导航栏的样式
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmDemo::initWidget()
{
    ui->stackedWidget->addWidget(new frmDemoHelper);
    ui->stackedWidget->addWidget(new frmDemoVideoImage);
    ui->stackedWidget->addWidget(new frmDemoVideoWidget);
    ui->stackedWidget->addWidget(new frmDemoVideoSave);
    ui->stackedWidget->addWidget(new frmDemoVideoPlus);
    ui->stackedWidget->addWidget(new frmDemoVideoPlay4);
    ui->stackedWidget->addWidget(new frmDemoVideoVolume);
    ui->stackedWidget->addWidget(new frmDemoVideoDrag);
    ui->stackedWidget->addWidget(new frmDemoVideoChange);
    ui->stackedWidget->addWidget(new frmDemoVideoDraw);
    ui->stackedWidget->addWidget(new frmDemoVideoOsd);
}

void frmDemo::initNav()
{
    //按钮文字集合
    QStringList names;
    names << "通用辅助" << "视频图片" << "视频控件" << "视频存储" << "视频叠加";
    names << "同步播放" << "音频频谱" << "拖曳播放" << "视频切换" << "标签图形";
    names << "水印文字";

    //自动生成按钮
    for (int i = 0; i < names.count(); ++i) {
        QToolButton *btn = new QToolButton;
        btn->setFont(qApp->font());
        //设置按钮的文字
        btn->setText(QString("%1. %2").arg(i + 1, 2, 10, QChar('0')).arg(names.at(i)));
        //设置按钮可选中按下类似复选框的功能
        btn->setCheckable(true);
        //设置按钮图标在左侧文字在右侧
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //设置按钮拉伸策略为横向填充
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        //关联按钮单击事件
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        //将按钮加入到布局
        ui->widgetLeft->layout()->addWidget(btn);
        //可以指定不同的图标
        icons << 0xf061;
        btns << btn;
    }

    //底部加个弹簧
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->widgetLeft->layout()->addItem(verticalSpacer);

    //根据配置文件存储的子窗体的索引触发按钮单击进行切换
    AppConfig::IndexDemo = AppConfig::IndexDemo >= btns.count() ? 0 : AppConfig::IndexDemo;
    btns.at(AppConfig::IndexDemo)->click();
}

void frmDemo::initIcon()
{
    //左侧导航样式,可以设置各种牛逼的参数,超级棒
    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    styleColor.btnHeight = 20;
    styleColor.iconSize = 15;
    styleColor.iconWidth = 16;
    styleColor.iconHeight = 15;
    styleColor.borderWidth = 3;
    styleColor.borderColor = GlobalConfig::HighColor;
    styleColor.setColor(GlobalConfig::NormalColorEnd, GlobalConfig::TextColor, GlobalConfig::PanelColor, GlobalConfig::TextColor);
    IconHelper::setStyle(ui->widgetLeft, btns, icons, styleColor);
}

void frmDemo::buttonClicked()
{
    QAbstractButton *b = (QAbstractButton *)sender();
    int count = btns.count();
    int index = btns.indexOf(b);
    ui->stackedWidget->setCurrentIndex(index);

    for (int i = 0; i < count; ++i) {
        QAbstractButton *btn = btns.at(i);
        btn->setChecked(btn == b);
    }

    //保存选中的子窗体的索引
    AppConfig::IndexDemo = index;
    AppConfig::writeConfig();
}
