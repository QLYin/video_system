#include "frmdemovideoosd.h"
#include "ui_frmdemovideoosd.h"
#include "qthelper.h"
#include "videourl.h"
#include "videohelper.h"
#include "widgethelper.h"

frmDemoVideoOsd::frmDemoVideoOsd(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoOsd)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoOsd::~frmDemoVideoOsd()
{
    delete ui;
}

void frmDemoVideoOsd::initForm()
{
    ui->cboxUrl->addItems(VideoUrl::getUrls(VideoUrl::Simple));
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoOsdUrl);

    //关联实时码率信号用于显示文字水印
    fontSize = 20;
    connect(ui->videoWidget, SIGNAL(sig_receiveKbps(qreal, int)), this, SLOT(receiveKbps(qreal, int)));
    connect(ui->videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
}

void frmDemoVideoOsd::receiveKbps(qreal kbps, int frameRate)
{
    OsdInfo osd;
    osd.name = "kbps";
    osd.color = "#FF0000";
    osd.fontSize = fontSize;
    osd.position = OsdPosition_RightTop;
    osd.text = QString("%1 kbps\n%2 fps").arg((int)kbps).arg(frameRate);
    ui->videoWidget->setOsd(osd);
}

void frmDemoVideoOsd::receivePlayStart(int time)
{
    //自动计算合适的文字大小(也可以手动设置)
    fontSize = ui->videoWidget->getVideoWidth() / 30;

    //左上角日期时间
    OsdInfo osd;
    osd.name = "datetime";
    osd.color = "#FFFFFF";
    osd.fontSize = fontSize;
    osd.position = OsdPosition_LeftTop;
    osd.format = OsdFormat_DateTimeMs;
    ui->videoWidget->setOsd(osd);
    on_btnUpdate_clicked();
}

void frmDemoVideoOsd::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoOsdUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();

        //设置解码结构体参数(具体含义参见结构体定义)
        VideoPara videoPara = ui->videoWidget->getVideoPara();
        videoPara.videoCore = VideoHelper::getVideoCore();
        //启用统计实时码率(默认关闭)
        videoPara.countKbps = true;
        if (AppConfig::VideoOsdUrl.startsWith("rtmp")) {
            videoPara.readTimeout = 0;
            videoPara.connectTimeout = 0;
            //videoPara.decodeType = DecodeType_Fastest;
        }

        ui->videoWidget->setVideoPara(videoPara);
        if (ui->videoWidget->open(AppConfig::VideoOsdUrl)) {
            ui->btnOpen->setText("关闭");
        }
    } else {
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
    }
}

void frmDemoVideoOsd::on_btnUpdate_clicked()
{
    //右下角指定文字
    OsdInfo osd;
    osd.name = "text";
    osd.color = "#F7E535";
    osd.fontSize = fontSize;
    osd.position = OsdPosition_RightBottom;
    osd.text = ui->txtText->text();
    //支持换行
    osd.text.replace("\\n", "\n");
    ui->videoWidget->setOsd(osd);
}
