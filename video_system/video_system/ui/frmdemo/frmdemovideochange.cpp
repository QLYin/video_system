#include "frmdemovideochange.h"
#include "ui_frmdemovideochange.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"
#include "videowidgetx.h"

frmDemoVideoChange::frmDemoVideoChange(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoChange)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoChange::~frmDemoVideoChange()
{
    delete ui;
}

void frmDemoVideoChange::showEvent(QShowEvent *)
{
    static bool isLoad = false;
    if (!isLoad) {
        isLoad = true;
        on_btnChange_clicked();
    }
}

void frmDemoVideoChange::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::HttpVideo | VideoUrl::TvStation));
    ui->cboxUrl2->addItems(VideoUrl::getUrls(VideoUrl::HttpVideo | VideoUrl::TvStation));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoChangeUrl1);
    ui->cboxUrl2->lineEdit()->setText(AppConfig::VideoChangeUrl2);

    //实例化视频控件(这里放两个用于交替无缝切换)
    videoWidget1 = new VideoWidget;
    videoWidget2 = new VideoWidget;
    this->initVideo(videoWidget1);
    this->initVideo(videoWidget2);

    currentIndex = 1;
    videoWidget1->setVisible(true);
}

void frmDemoVideoChange::initVideo(VideoWidget *videoWidget)
{
    //设置窗体参数
    WidgetPara widgetPara = videoWidget->getWidgetPara();
    widgetPara.fillColor = "#110F11";
    widgetPara.scaleMode = ScaleMode_Aspect;
    videoWidget->setWidgetPara(widgetPara);

    //设置解码内核
    VideoPara videoPara = videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    videoWidget->setVideoPara(videoPara);

    //加入到布局并关联打开成功信号
    videoWidget->setVisible(false);
    ui->gridLayout->addWidget(videoWidget);
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
}

void frmDemoVideoChange::receivePlayStart(int time)
{
    ui->btnChange->setEnabled(true);
    if (currentIndex == 0) {
        videoWidget2->setVisible(false);
        videoWidget1->setVisible(true);
        videoWidget2->stop();
    } else if (currentIndex == 1) {
        videoWidget1->setVisible(false);
        videoWidget2->setVisible(true);
        videoWidget1->stop();
    }
}

void frmDemoVideoChange::on_btnChange_clicked()
{
    ui->btnChange->setEnabled(false);
    currentIndex = (currentIndex == 0 ? 1 : 0);
    AppConfig::VideoChangeUrl1 = ui->cboxUrl->currentText();
    AppConfig::VideoChangeUrl2 = ui->cboxUrl2->currentText();
    AppConfig::writeConfig();

    //先后台默默的打开不显示(等待打开成功后再显示)
    if (currentIndex == 0) {
        videoWidget1->open(AppConfig::VideoChangeUrl1);
    } else if (currentIndex == 1) {
        videoWidget2->open(AppConfig::VideoChangeUrl2);
    }
}
