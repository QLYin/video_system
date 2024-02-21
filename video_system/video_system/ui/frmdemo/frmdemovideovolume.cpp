#include "frmdemovideovolume.h"
#include "ui_frmdemovideovolume.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"
#include "videowidgetx.h"
#include "frmvideowidgetvolume.h"

frmDemoVideoVolume::frmDemoVideoVolume(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoVolume)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoVolume::~frmDemoVideoVolume()
{
    delete ui;
}

void frmDemoVideoVolume::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::All));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoAudioUrl);

    //设置解码内核
    videoWidget = new VideoWidget;
    VideoPara videoPara = videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    videoPara.audioLevel = true;
    videoWidget->setVideoPara(videoPara);

    //将视频控件添加到组合的视频窗体控件中
    frmVideoWidgetVolume *widget = new frmVideoWidgetVolume(videoWidget);
    widget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->gridLayout->addWidget(widget);
}

void frmDemoVideoVolume::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoAudioUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();
        if (videoWidget->open(AppConfig::VideoAudioUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        videoWidget->stop();
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoVolume::on_btnPause_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        return;
    }

    if (ui->btnPause->text() == "暂停") {
        videoWidget->pause();
        ui->btnPause->setText("继续");
    } else {
        videoWidget->next();
        ui->btnPause->setText("暂停");
    }
}
