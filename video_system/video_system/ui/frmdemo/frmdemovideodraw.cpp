#include "frmdemovideodraw.h"
#include "ui_frmdemovideodraw.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"
#include "widgethelper.h"

frmDemoVideoDraw::frmDemoVideoDraw(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoDraw)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoDraw::~frmDemoVideoDraw()
{
    delete ui;
}

void frmDemoVideoDraw::initForm()
{
    osdIndex = 0;
    graphIndex = 0;

    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::Simple));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoDrawUrl);
}

void frmDemoVideoDraw::on_btnOpen_clicked()
{
    //复位和清空标签及图形相关(也可以不清空这样相当于可以提前设置)
    osdIndex = 0;
    graphIndex = 0;
    ui->videoWidget->clearOsd();
    ui->videoWidget->clearGraph();

    //设置解码结构体参数(具体含义参见结构体定义)
    VideoPara videoPara = ui->videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    ui->videoWidget->setVideoPara(videoPara);

    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoDrawUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();
        if (ui->videoWidget->open(AppConfig::VideoDrawUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
    }
}

void frmDemoVideoDraw::on_btnOsd_clicked()
{
    WidgetHelper::addOsd(ui->videoWidget, osdIndex);
}

void frmDemoVideoDraw::on_btnGraph_clicked()
{
    WidgetHelper::addGraph(ui->videoWidget, graphIndex);
}
