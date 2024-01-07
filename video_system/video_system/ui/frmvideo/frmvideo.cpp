#include "frmvideo.h"
#include "ui_frmvideo.h"
#include "qthelper.h"
#include "devicehelper.h"

#include "frmvideoplaylocal.h"
#include "frmvideoplayweb.h"
#include "frmvideoplayrtsp.h"
#include "frmvideoplayimage.h"
#include "frmvideoupload.h"

frmVideo::frmVideo(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideo)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmVideo::~frmVideo()
{
    delete ui;
}

void frmVideo::initForm()
{
    ui->widgetLeft->setProperty("flag", "btnNavLeft");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
}

void frmVideo::initWidget()
{
    videoPlayLocal = new frmVideoPlayLocal;
    ui->stackedWidget->addWidget(videoPlayLocal);

    videoPlayWeb = new frmVideoPlayWeb;
    ui->stackedWidget->addWidget(videoPlayWeb);

    videoPlayRtsp = new frmVideoPlayRtsp;
    ui->stackedWidget->addWidget(videoPlayRtsp);

    videoPlayImage = new frmVideoPlayImage;
    ui->stackedWidget->addWidget(videoPlayImage);

    ui->stackedWidget->addWidget(new frmVideoUpload);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeVideoConfig()), this, SLOT(changeVideoConfig()));
}

void frmVideo::initNav()
{
    QList<QString> names, texts;
    names << "btnVideoPlayLocal" << "btnVideoPlayWeb" << "btnVideoPlayRtsp" << "btnVideoPlayImage" << "btnVideoUpload";
    texts << "本 地 回 放" << "网 络 回 放" << "远 程 回 放" << "图 片 回 放" << "视 频 上 传";
    icons << 0xea04 << 0xe9f3 << 0xe9f2 << 0xea5c << 0xea5a;

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
    int index = names.indexOf(AppConfig::LastFormVideo);
    index >= 0 ? btns.at(index)->click() : btns.first()->click();

    //根据配置启用哪些模块
    btns.at(1)->setVisible(AppConfig::EnableVideoPlayWeb);
    btns.at(2)->setVisible(AppConfig::EnableVideoPlayNvr);
    btns.at(3)->setVisible(AppConfig::EnableVideoPlayImage);
    btns.at(4)->setVisible(AppConfig::EnableVideoUpload);
}

void frmVideo::initIcon()
{
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        CommonNav::initNavBtnIcon(btns.at(i), icons.at(i), true);
    }
}

void frmVideo::buttonClicked()
{
    //切换到当前窗体
    QAbstractButton *btn = (QAbstractButton *)sender();
    ui->stackedWidget->setCurrentIndex(btns.indexOf(btn));

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormVideo = btn->objectName();
    AppConfig::writeConfig();
}

void frmVideo::changeVideoConfig()
{
    //其他几个界面的视频控件更改视频参数一起放在这里
    QList<VideoWidget *> videoWidgets;
    videoWidgets << videoPlayLocal->getVideoWidget();
    videoWidgets << videoPlayWeb->getVideoWidgets();
    videoWidgets << videoPlayRtsp->getVideoWidgets();
    videoWidgets << videoPlayImage->getVideoWidget();
    foreach (VideoWidget *videoWidget, videoWidgets) {
        DeviceHelper::initVideoWidget(videoWidget);
    }
}
