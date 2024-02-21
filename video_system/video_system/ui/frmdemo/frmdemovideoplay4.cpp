#include "frmdemovideoplay4.h"
#include "ui_frmdemovideoplay4.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "videowidgetx.h"
#include "videohelper.h"

frmDemoVideoPlay4::frmDemoVideoPlay4(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoPlay4)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoPlay4::~frmDemoVideoPlay4()
{
    delete ui;
}

void frmDemoVideoPlay4::initForm()
{
    //实例化4个视频控件同时播放
    for (int i = 0; i < 4; ++i) {
        VideoWidget *videoWidget = new VideoWidget;
        videoWidget->setObjectName(QString("video%1").arg(i + 1));
        videoWidget->setBgText(QString("通道 %1").arg(i + 1));
        videoWidget->setVideoFlag(videoWidget->objectName());
        videoWidget->setScaleMode(ScaleMode_Fill);
        //videoWidget->setVideoMode(VideoMode_Painter);
        VideoPara videoPara = videoWidget->getVideoPara();
        videoPara.videoCore = VideoHelper::getVideoCore();
        videoWidget->setVideoPara(videoPara);
        videoWidgets << videoWidget;
    }

    //加入到布局
    ui->gridLayout->addWidget(videoWidgets.at(0), 0, 0);
    ui->gridLayout->addWidget(videoWidgets.at(1), 0, 1);
    ui->gridLayout->addWidget(videoWidgets.at(2), 1, 0);
    ui->gridLayout->addWidget(videoWidgets.at(3), 1, 1);
}

void frmDemoVideoPlay4::receiveDuration(qint64 duration)
{
    //设置进度条最大进度以及总时长
    ui->sliderPosition->setValue(0);
    ui->sliderPosition->setMaximum(duration);
    ui->labDuration->setText(QtHelper::getTimeString(duration));
}

void frmDemoVideoPlay4::receivePosition(qint64 position)
{
    //设置当前进度及已播放时长
    ui->sliderPosition->setValue(position);
    ui->labPosition->setText(QtHelper::getTimeString(position));
}

void frmDemoVideoPlay4::on_btnOpen_clicked()
{
    //可以将地址改成自己的
    QStringList urls;
    urls << AppConfig::VideoPlayUrl1 << AppConfig::VideoPlayUrl2 << AppConfig::VideoPlayUrl3 << AppConfig::VideoPlayUrl4;

    int count = urls.count();
    if (ui->btnOpen->text() == "打开") {
        ui->btnOpen->setText("关闭");
        for (int i = 0; i < count; ++i) {
            VideoWidget *videoWidget = videoWidgets.at(i);
            if (videoWidget->open(urls.at(i))) {
                //假定视频文件时长一样,这里以第一个视频为准
                if (i == 0) {
                    VideoThread *thread = videoWidget->getVideoThread();
                    connect(thread, SIGNAL(receiveDuration(qint64)), this, SLOT(receiveDuration(qint64)));
                    connect(thread, SIGNAL(receivePosition(qint64)), this, SLOT(receivePosition(qint64)));
                }
            }
        }
    } else {
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
        foreach (VideoWidget *videoWidget, videoWidgets) {
            videoWidget->stop();
        }
    }
}

void frmDemoVideoPlay4::on_btnPause_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        return;
    }

    if (ui->btnPause->text() == "暂停") {
        ui->btnPause->setText("继续");
        foreach (VideoWidget *videoWidget, videoWidgets) {
            videoWidget->pause();
        }
    } else {
        ui->btnPause->setText("暂停");
        foreach (VideoWidget *videoWidget, videoWidgets) {
            videoWidget->next();
        }
    }
}

void frmDemoVideoPlay4::on_sliderPosition_clicked()
{
    int value = ui->sliderPosition->value();
    on_sliderPosition_sliderMoved(value);
}

void frmDemoVideoPlay4::on_sliderPosition_sliderMoved(int value)
{
    foreach (VideoWidget *videoWidget, videoWidgets) {
        videoWidget->setPosition(value);
    }
}
