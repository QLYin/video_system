#include "frmvideowidgetslider.h"
#include "ui_frmvideowidgetslider.h"
#include "qthelper.h"
#include "videowidgetx.h"

frmVideoWidgetSlider::frmVideoWidgetSlider(VideoWidget *videoWidget, QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWidgetSlider)
{
    ui->setupUi(this);

    //传入的视频控件插入到布局上侧
    this->videoWidget = videoWidget;
    ui->verticalLayout->insertWidget(0, videoWidget);
    ui->sliderPosition->setRange(0, 0);

    iconSize = AppData::BtnMinWidth / 3.0;
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);

    //关联播放开始和完成信号
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
    connect(videoWidget, SIGNAL(sig_receivePlayFinsh()), this, SLOT(receivePlayFinsh()));
}

frmVideoWidgetSlider::~frmVideoWidgetSlider()
{
    delete ui;
}

VideoWidget *frmVideoWidgetSlider::getVideoWidget()
{
    return videoWidget;
}

void frmVideoWidgetSlider::receivePlayStart(int)
{
    //关联采集线程信号槽
    VideoThread *videoThread = videoWidget->getVideoThread();
    connect(videoThread, SIGNAL(receiveDuration(qint64)), this, SLOT(receiveDuration(qint64)));
    connect(videoThread, SIGNAL(receivePosition(qint64)), this, SLOT(receivePosition(qint64)));

    //主动获取一次
    if (videoThread->getIsFile()) {
        this->receiveDuration(videoThread->getDuration());
    }

    //切换播放图标
    IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
}

void frmVideoWidgetSlider::receivePlayFinsh()
{
    ui->labDuration->setText("00:00");
    ui->labPosition->setText("00:00");
    ui->sliderPosition->setValue(0);
    ui->sliderPosition->setRange(0, 0);
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
}

void frmVideoWidgetSlider::receiveDuration(qint64 duration)
{
    //设置进度条最大进度以及总时长
    ui->sliderPosition->setValue(0);
    ui->sliderPosition->setMaximum(duration);
    ui->labDuration->setText(QtHelper::getTimeString(duration));
}

void frmVideoWidgetSlider::receivePosition(qint64 position)
{
    //设置当前进度及已播放时长
    ui->sliderPosition->setValue(position);
    ui->labPosition->setText(QtHelper::getTimeString(position));
}

void frmVideoWidgetSlider::on_btnPlay_clicked()
{
    //运行阶段取真实值
    static bool pause = true;
    if (videoWidget->getIsRunning()) {
        pause = videoWidget->getIsPause();
    }

    if (pause) {
        videoWidget->next();
        IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
    } else {
        videoWidget->pause();
        IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    }

    pause = !pause;
}

void frmVideoWidgetSlider::on_sliderPosition_clicked()
{
    int value = ui->sliderPosition->value();
    on_sliderPosition_sliderMoved(value);
}

void frmVideoWidgetSlider::on_sliderPosition_sliderMoved(int value)
{
    videoWidget->setPosition(value);
    emit positionChanged(value);
}
