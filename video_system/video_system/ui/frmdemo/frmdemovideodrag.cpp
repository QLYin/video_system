#include "frmdemovideodrag.h"
#include "ui_frmdemovideodrag.h"
#include "qthelper.h"
#include "videohelper.h"

frmDemoVideoDrag::frmDemoVideoDrag(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoDrag)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoDrag::~frmDemoVideoDrag()
{
    delete ui;
}

void frmDemoVideoDrag::initForm()
{
    ui->txtUrl->setText(AppConfig::VideoDragUrl);

    //设置解码内核
    VideoPara videoPara = ui->videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    ui->videoWidget->setVideoPara(videoPara);

    //关联拖曳信号进行处理
    connect(ui->videoWidget, SIGNAL(sig_fileDrag(QString)), this, SLOT(fileDrag(QString)));
    //也可以关联信号槽直接内部处理掉(此时界面上的打开按钮等文字更改需要关联打开和关闭信号进行处理)
    //connect(ui->videoWidget, SIGNAL(sig_fileDrag(QString)), ui->videoWidget, SLOT(open(QString)));
}

void frmDemoVideoDrag::fileDrag(const QString &url)
{
    ui->txtUrl->setText(url);
    //先执行打开按钮
    on_btnOpen_clicked();
    //如果现在还是打开说明刚才是关闭
    if (ui->btnOpen->text() == "打开") {
        on_btnOpen_clicked();
    }
}

void frmDemoVideoDrag::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoDragUrl = ui->txtUrl->text();
        AppConfig::writeConfig();
        if (ui->videoWidget->open(AppConfig::VideoDragUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoDrag::on_btnPause_clicked()
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
