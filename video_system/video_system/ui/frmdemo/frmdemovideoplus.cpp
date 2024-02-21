#include "frmdemovideoplus.h"
#include "ui_frmdemovideoplus.h"
#include "qthelper.h"
#include "videourl.h"
#include "videowidgetx.h"
#include "videohelper.h"

frmDemoVideoPlus::frmDemoVideoPlus(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoPlus)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoPlus::~frmDemoVideoPlus()
{
    delete ui;
}

void frmDemoVideoPlus::showEvent(QShowEvent *)
{
    static bool isLoad = false;
    if (!isLoad) {
        isLoad = true;
        videoWidget2->setBorderWidth(0);
        //移动到合适位置
        int border = videoWidget1->getBorderWidth() / 2;
        videoWidget2->move(border, border);
    }
}

void frmDemoVideoPlus::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::HttpVideo));
    ui->cboxUrl2->addItems(VideoUrl::getUrls(VideoUrl::HttpVideo));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoPlusUrl1);
    ui->cboxUrl2->lineEdit()->setText(AppConfig::VideoPlusUrl2);

    //实例化主控件并加入布局
    videoWidget1 = new VideoWidget;
    videoWidget1->setObjectName("widget1");
    videoWidget1->setVideoFlag(videoWidget1->objectName());
    videoWidget1->setScaleMode(ScaleMode_Fill);
    ui->gridLayout->addWidget(videoWidget1);

    //实例化子控件
    videoWidget2 = new VideoWidget;
    videoWidget2->setObjectName("videoWidget2");
    videoWidget2->setVideoFlag(videoWidget2->objectName());
    videoWidget2->setScaleMode(ScaleMode_Fill);
    videoWidget2->setBgText("");
    videoWidget2->setMinimumSize(400, 300);

    //设置解码内核
    VideoPara para1 = videoWidget1->getVideoPara();
    VideoPara para2 = videoWidget2->getVideoPara();
    para1.videoCore = VideoHelper::getVideoCore();
    para2.videoCore = VideoHelper::getVideoCore();
    videoWidget1->setVideoPara(para1);
    videoWidget2->setVideoPara(para2);

    //设置主控件的布局
    QGridLayout *gridLayout = new QGridLayout(videoWidget1);
    //gridLayout->setSpacing(0);
    int margin = 2;
    gridLayout->setContentsMargins(margin, margin, margin, margin);
    QSpacerItem *hSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

    //可以自行调整位置 0-可拖动 1-左上角 2-右上角 3-左下角 4-右下角
    int type = 1;
    if (type == 1) {
        gridLayout->addItem(hSpacer, 0, 1);
        gridLayout->addItem(vSpacer, 1, 0);
        gridLayout->addWidget(videoWidget2, 0, 0);
    } else if (type == 2) {
        gridLayout->addItem(hSpacer, 0, 0);
        gridLayout->addItem(vSpacer, 1, 1);
        gridLayout->addWidget(videoWidget2, 0, 1);
    } else if (type == 3) {
        gridLayout->addItem(vSpacer, 0, 0);
        gridLayout->addItem(hSpacer, 1, 1);
        gridLayout->addWidget(videoWidget2, 1, 0);
    } else if (type == 4) {
        gridLayout->addItem(hSpacer, 1, 0);
        gridLayout->addItem(vSpacer, 0, 1);
        gridLayout->addWidget(videoWidget2, 1, 1);
    }
}

void frmDemoVideoPlus::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoPlusUrl1 = ui->cboxUrl->currentText();
        AppConfig::VideoPlusUrl2 = ui->cboxUrl2->currentText();
        AppConfig::writeConfig();

        ui->btnOpen->setText("关闭");
        videoWidget1->open(AppConfig::VideoPlusUrl1);
        videoWidget2->open(AppConfig::VideoPlusUrl2);
    } else {
        videoWidget1->stop();
        videoWidget2->stop();
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoPlus::on_btnPause_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        return;
    }

    if (ui->btnPause->text() == "暂停") {
        videoWidget1->pause();
        videoWidget2->pause();
        ui->btnPause->setText("继续");
    } else {
        videoWidget1->next();
        videoWidget2->next();
        ui->btnPause->setText("暂停");
    }
}
