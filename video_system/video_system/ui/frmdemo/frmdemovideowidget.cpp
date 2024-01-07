#include "frmdemovideowidget.h"
#include "ui_frmdemovideowidget.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"

frmDemoVideoWidget::frmDemoVideoWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoWidget)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoWidget::~frmDemoVideoWidget()
{
    delete ui;
}

void frmDemoVideoWidget::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::All));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoWidgetUrl);

    //这里设置个独立的名称用于演示如何单独设置视频控件的样式
    //ui->videoWidget->setObjectName("videoWidgetLocal");

    //设置窗体结构体参数(具体含义参见结构体定义)
    WidgetPara widgetPara = ui->videoWidget->getWidgetPara();
    //widgetPara.borderWidth = 5;
    widgetPara.bgImage = QImage(QString("%1/config/bg_novideo.png").arg(QtHelper::appPath()));
    widgetPara.bannerEnable = true;
    widgetPara.scaleMode = ScaleMode_Auto;
    widgetPara.videoMode = VideoMode_Hwnd;
    ui->videoWidget->setWidgetPara(widgetPara);

    //设置解码结构体参数(具体含义参见结构体定义)
    VideoPara videoPara = ui->videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    videoPara.decodeType = DecodeType_Fast;
    videoPara.hardware = "none";
    videoPara.transport = "tcp";
    videoPara.playRepeat = false;
    videoPara.readTimeout = 0;
    videoPara.connectTimeout = 1000;
    ui->videoWidget->setVideoPara(videoPara);

    //定时器截图
    timerSnap = new QTimer(this);
    connect(timerSnap, SIGNAL(timeout()), this, SLOT(on_btnSnap_clicked()));
    timerSnap->setInterval(100);
}

void frmDemoVideoWidget::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoWidgetUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();
        if (ui->videoWidget->open(AppConfig::VideoWidgetUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoWidget::on_btnPause_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        return;
    }

    if (ui->btnPause->text() == "暂停") {
        ui->videoWidget->pause();
        ui->btnPause->setText("继续");
    } else {
        ui->videoWidget->next();
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoWidget::on_btnSnap_clicked()
{
    QString snapPath = QtHelper::appPath() + "/image_normal/" + QDATE;
    QString fileName = QString("%1/ch%2_%3.jpg").arg(snapPath).arg("01").arg(STRDATETIMEMS);
    ui->videoWidget->snap(fileName);
}

void frmDemoVideoWidget::on_ckAutoSnap_stateChanged(int arg1)
{
    if (arg1 == 0) {
        timerSnap->stop();
    } else {
        timerSnap->start();
    }
}
