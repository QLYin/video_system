#include "frmvideoplayweb.h"
#include "ui_frmvideoplayweb.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "videowidgetx.h"

frmVideoPlayWeb::frmVideoPlayWeb(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoPlayWeb)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
    this->initData();
    this->initVideo();
    this->initVideoCh();
    //QTimer::singleShot(1000, this, SLOT(playVideo()));
}

frmVideoPlayWeb::~frmVideoPlayWeb()
{
    delete ui;
}

QList<VideoWidget *> frmVideoPlayWeb::getVideoWidgets()
{
    return this->videoWidgets;
}

void frmVideoPlayWeb::showEvent(QShowEvent *)
{
    this->initData();
}

bool frmVideoPlayWeb::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (watched->inherits("QWidget")) {
            VideoWidget *videoWidget = (VideoWidget *) watched;
            if (!videoMax) {
                for (int i = 0; i < videoCount; ++i) {
                    videoWidgets.at(i)->setVisible(false);
                }
                videoMax = true;
                videoWidget->setVisible(true);
            } else {
                for (int i = 0; i < videoCount; ++i) {
                    videoWidgets.at(i)->setVisible(true);
                }
                videoMax = false;
            }
            videoWidget->setFocus();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmVideoPlayWeb::initForm()
{
    ui->widgetRight->setFixedWidth(AppData::RightWidth);
    ui->cboxNvr->addItems(DbData::NvrInfo_NvrName);

    ui->cboxType->addItem("所有录像");
    ui->cboxType->addItem("普通录像");
    ui->cboxType->addItem("报警录像");
    ui->cboxType->addItem("动检录像");
    ui->cboxType->addItem("智能录像");

    ui->cboxRtsp->addItem("主码流");
    ui->cboxRtsp->addItem("子码流");

    QStringList speeds;
    speeds << "半倍速度" << "正常速度" << "二倍速度" << "四倍速度" << "八倍速度" << "十六倍速度";
    ui->cboxSpeed->addItems(speeds);
    ui->cboxSpeed->setCurrentIndex(1);

    QtHelper::initDataTimeEdit(ui->dateTimeStart, -1);
    QtHelper::initDataTimeEdit(ui->dateTimeEnd, 0);

    //测试时间
    //ui->dateTimeStart->setDateTime(QDateTime::fromString("2021-03-10 16:12:00", "yyyy-MM-dd HH:mm:ss"));
    //ui->dateTimeEnd->setDateTime(QDateTime::fromString("2021-03-10 16:18:00", "yyyy-MM-dd HH:mm:ss"));

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmVideoPlayWeb::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);
}

void frmVideoPlayWeb::initData()
{
    ui->listWidget->clear();
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 10; ++i) {
        now = now.addSecs(30 * 60);
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setText(QString("%1 %2").arg(now.toString("yyyy-MM-dd HH:mm:ss")).arg("30分"));
        item->setCheckState(Qt::Checked);
    }
}

void frmVideoPlayWeb::initVideo()
{
    videoMax = false;
    videoCount = 4;

    for (int i = 0; i < videoCount; ++i) {
        VideoWidget *videoWidget = new VideoWidget;
        DeviceHelper::initVideoWidget(videoWidget);
        //设置背景文字
        videoWidget->setBgText(QString("通道 %1").arg(i + 1));
        videoWidget->installEventFilter(this);
        videoWidget->setProperty("index", i);
        videoWidget->setObjectName(QString("video%1").arg(i + 1));
        videoWidgets << videoWidget;
    }

    //加入到布局中
    ui->gridLayout->addWidget(videoWidgets.at(0), 0, 0);
    ui->gridLayout->addWidget(videoWidgets.at(1), 0, 1);
    ui->gridLayout->addWidget(videoWidgets.at(2), 1, 0);
    ui->gridLayout->addWidget(videoWidgets.at(3), 1, 1);
}

void frmVideoPlayWeb::initVideoCh()
{
    //设置时间轴
    VideoCh::VideoData data1, data2, data3, data4;

    data1.startX = 30;
    data1.length = 150;
    data2.startX = 200;
    data2.length = 50;
    data3.startX = 280;
    data3.length = 100;
    ui->videoPlayback->setDatas1(QList<VideoCh::VideoData>() << data1 << data2 << data3);

    data4.startX = 30;
    data4.length = 580;
    ui->videoPlayback->setDatas2(QList<VideoCh::VideoData>() << data4);
    ui->videoPlayback->setDatas3(QList<VideoCh::VideoData>() << data4);
    ui->videoPlayback->setDatas4(QList<VideoCh::VideoData>() << data4);
}

void frmVideoPlayWeb::playVideo()
{
    //举例数据(用户可以自行改成自己的)
    videoWidgets.at(0)->open("f:/mp4/asf/1.asf");
    videoWidgets.at(1)->open("f:/mp4/asf/2.asf");
    videoWidgets.at(2)->open("f:/mp4/asf/3.asf");
    videoWidgets.at(3)->open("f:/mp4/asf/4.asf");
}

void frmVideoPlayWeb::on_btnSelect_clicked()
{

}

void frmVideoPlayWeb::on_btnDownload_clicked()
{

}

void frmVideoPlayWeb::on_btnDelete_clicked()
{
    ui->listWidget->clear();
}

void frmVideoPlayWeb::on_btnClear_clicked()
{
    ui->listWidget->clear();
}

void frmVideoPlayWeb::on_btnSelectAll_clicked()
{
    int count = ui->listWidget->count();
    for (int row = 0; row < count; row++) {
        ui->listWidget->item(row)->setCheckState(Qt::Checked);
    }
}

void frmVideoPlayWeb::on_btnSelectNone_clicked()
{
    int count = ui->listWidget->count();
    for (int row = 0; row < count; row++) {
        ui->listWidget->item(row)->setCheckState(Qt::Unchecked);
    }
}

void frmVideoPlayWeb::on_cboxNvr_currentIndexChanged(int)
{
    ui->cboxCh->clear();
    QString arg1 = ui->cboxNvr->currentText();
    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        if (DbData::IpcInfo_NvrName.at(i) == arg1) {
            ui->cboxCh->addItem(DbData::IpcInfo_IpcName.at(i));
        }
    }
}

void frmVideoPlayWeb::on_listWidget_itemPressed(QListWidgetItem *item)
{
    bool checked = (item->checkState() == Qt::Checked);
    item->setCheckState(checked ? Qt::Unchecked : Qt::Checked);
}
