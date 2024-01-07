#include "frmdemovideosave.h"
#include "ui_frmdemovideosave.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"

frmDemoVideoSave::frmDemoVideoSave(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoSave)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoSave::~frmDemoVideoSave()
{
    delete ui;
}

void frmDemoVideoSave::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::All));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoSaveUrl);

    //设置解码内核
    VideoPara videoPara = ui->videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    ui->videoWidget->setVideoPara(videoPara);
}

void frmDemoVideoSave::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoSaveUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();
        if (ui->videoWidget->open(AppConfig::VideoSaveUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
        ui->btnSave->setText("开始");
    }
}

void frmDemoVideoSave::on_btnSave_clicked()
{
    if (ui->btnSave->text() == "开始") {
        //每次设置不一样的文件名称(不然一样的话会覆盖)
        QString savePath = QString("%1/video_normal/%2").arg(QtHelper::appPath()).arg("test");
        QString fileName = QString("%1/%2.mp4").arg(savePath).arg(STRDATETIMEMS);
        ui->videoWidget->recordStart(fileName);
        ui->btnSave->setText("停止");
    } else {
        ui->videoWidget->recordStop();
        ui->btnSave->setText("开始");
    }
}
