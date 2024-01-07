#include "frmvideowidgetvolume.h"
#include "ui_frmvideowidgetvolume.h"
#include "qthelper.h"
#include "videowidgetx.h"

frmVideoWidgetVolume::frmVideoWidgetVolume(VideoWidget *videoWidget, QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWidgetVolume)
{
    ui->setupUi(this);

    //传入的视频控件插入到布局左侧
    this->videoWidget = videoWidget;
    ui->horizontalLayout->insertWidget(0, videoWidget);

    //关联音频振幅信号
    connect(videoWidget, SIGNAL(sig_receiveLevel(qreal, qreal)), this, SLOT(receiveLevel(qreal, qreal)));

    //打开和关闭后需要清空下(有可能残留上一次的数据)
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), ui->leftVolume, SLOT(clear()));
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), ui->rightVolume, SLOT(clear()));
    connect(videoWidget, SIGNAL(sig_receivePlayFinsh()), ui->leftVolume, SLOT(clear()));
    connect(videoWidget, SIGNAL(sig_receivePlayFinsh()), ui->rightVolume, SLOT(clear()));
}

frmVideoWidgetVolume::~frmVideoWidgetVolume()
{
    delete ui;
}

void frmVideoWidgetVolume::resizeEvent(QResizeEvent *)
{
    //根据高度动态设置音量格子数量(一般高度越小格子数越少)
    int height = 3;
    int step = this->height() / (2 * height);
    ui->leftVolume->setStep(step);
    ui->rightVolume->setStep(step);

    //根据宽度动态设置音量条宽度
    int width = this->width() * 0.05;
    width = width > 20 ? 20 : width;
    ui->leftVolume->setMaximumWidth(width);
    ui->rightVolume->setMaximumWidth(width);
}

void frmVideoWidgetVolume::receiveLevel(qreal leftLevel, qreal rightLevel)
{
    ui->leftVolume->setValue(leftLevel);
    ui->rightVolume->setValue(rightLevel);
}
