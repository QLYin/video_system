#include "frmvideoplayrtsp.h"
#include "ui_frmvideoplayrtsp.h"
#include "qthelper.h"
#include "urlhelper.h"
#include "devicehelper.h"
#include "videowidgetx.h"
#include "frmvideowidgetslider.h"

frmVideoPlayRtsp::frmVideoPlayRtsp(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoPlayRtsp)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    this->initIcon();
    this->initVideo();
    this->setUrl();
}

frmVideoPlayRtsp::~frmVideoPlayRtsp()
{
    delete ui;
}

QList<VideoWidget *> frmVideoPlayRtsp::getVideoWidgets()
{
    return this->videoWidgets;
}

bool frmVideoPlayRtsp::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (watched->inherits("QWidget")) {
            QWidget *widget = (QWidget *) watched;
            videoIndex = widget->property("index").toInt();
            ui->labTip->setText(QString("当前选中 %1").arg(videoWidgets.at(videoIndex)->getBgText()));
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        if (watched->inherits("QWidget")) {
            VideoWidget *videoWidget = (VideoWidget *) watched;
            QWidget *widget = videoWidget;
            if (videoWidget->parent()->inherits("frmVideoWidgetSlider")) {
                widget = (QWidget *)videoWidget->parent();
            }

            if (!videoMax) {
                foreach (QWidget *w, widgets) {
                    w->setVisible(false);
                }
                videoMax = true;
                widget->setVisible(true);
            } else {
                foreach (QWidget *w, widgets) {
                    w->setVisible(true);
                }
                videoMax = false;
            }
            videoWidget->setFocus();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmVideoPlayRtsp::initForm()
{
    ui->widgetRight->setFixedWidth(AppData::RightWidth);
    ui->txtUserPwd->setShowPwdButton(true);
    //ui->cboxDeviceIP->addItems(DbData::NvrInfo_NvrIP);

    QtHelper::initDataTimeEdit(ui->dateTimeStart, 0);
    QtHelper::initDataTimeEdit(ui->dateTimeEnd, 1);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmVideoPlayRtsp::initConfig()
{
    ui->cboxCompanyType->addItems(AppData::NvrTypes);
    ui->cboxCompanyType->setCurrentIndex(AppConfig::NvrCompanyType);
    connect(ui->cboxCompanyType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxVideoType->addItem("实时视频");
    ui->cboxVideoType->addItem("回放视频");
    ui->cboxVideoType->setCurrentIndex(AppConfig::NvrVideoType);
    connect(ui->cboxVideoType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtUserName->setText(AppConfig::NvrUserName);
    connect(ui->txtUserName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserPwd->setText(AppConfig::NvrUserPwd);
    connect(ui->txtUserPwd, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->cboxDeviceIP->lineEdit()->setText(AppConfig::NvrDeviceIP);
    connect(ui->cboxDeviceIP->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSyncPosition->setChecked(AppConfig::NvrSyncPosition);
    connect(ui->ckSyncPosition, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoCh->setChecked(AppConfig::NvrAutoCh);
    connect(ui->ckAutoCh, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    for (int i = 1; i <= 64; ++i) {
        ui->cboxChannel->addItem(QString("通道 %1").arg(i, 2, 10, QChar('0')));
    }
    connect(ui->cboxChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxStreamType->addItem("主码流");
    ui->cboxStreamType->addItem("子码流");
    ui->cboxStreamType->addItem("三码流");
    ui->cboxStreamType->setCurrentIndex(AppConfig::NvrStreamType);
    connect(ui->cboxStreamType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->dateTimeStart->setDateTime(QDateTime::fromString(AppConfig::NvrDateTimeStart, AppConfig::NvrDateTimeFormat));
    ui->dateTimeEnd->setDateTime(QDateTime::fromString(AppConfig::NvrDateTimeEnd, AppConfig::NvrDateTimeFormat));
    connect(ui->dateTimeStart, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(saveConfig()));
    connect(ui->dateTimeEnd, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(saveConfig()));
}

void frmVideoPlayRtsp::saveConfig()
{
    AppConfig::NvrCompanyType = ui->cboxCompanyType->currentIndex();
    AppConfig::NvrVideoType = ui->cboxVideoType->currentIndex();
    AppConfig::NvrUserName = ui->txtUserName->text().trimmed();
    AppConfig::NvrUserPwd = ui->txtUserPwd->text().trimmed();
    AppConfig::NvrDeviceIP = ui->cboxDeviceIP->currentText().trimmed();
    AppConfig::NvrSyncPosition = ui->ckSyncPosition->isChecked();
    AppConfig::NvrAutoCh = ui->ckAutoCh->isChecked();
    AppConfig::NvrStreamType = ui->cboxStreamType->currentIndex();
    AppConfig::NvrDateTimeStart = ui->dateTimeStart->dateTime().toString(AppConfig::NvrDateTimeFormat);
    AppConfig::NvrDateTimeEnd = ui->dateTimeEnd->dateTime().toString(AppConfig::NvrDateTimeFormat);
    AppConfig::writeConfig();
    this->setUrl();
}

void frmVideoPlayRtsp::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);
}

void frmVideoPlayRtsp::initVideo()
{
    videoMax = false;
    videoCount = 4;
    videoIndex = 0;

    for (int i = 0; i < videoCount; ++i) {
        VideoWidget *videoWidget = new VideoWidget;
        videoWidget->setProperty("index", i);
        videoWidget->setObjectName(QString("video%1").arg(i + 1));
        videoWidget->installEventFilter(this);
        videoWidget->setBgText(QString("通道 %1").arg(i + 1));
        DeviceHelper::initVideoWidget(videoWidget, 100, false, true);
        videoWidgets << videoWidget;

        //自定义带进度条的视频面板
        frmVideoWidgetSlider *videoSlider = new frmVideoWidgetSlider(videoWidget);
        connect(videoSlider, SIGNAL(positionChanged(int)), this, SLOT(positionChanged(int)));
        widgets << videoSlider;
    }

    //加入到布局中
    ui->gridLayout->addWidget(widgets.at(0), 0, 0);
    ui->gridLayout->addWidget(widgets.at(1), 0, 1);
    ui->gridLayout->addWidget(widgets.at(2), 1, 0);
    ui->gridLayout->addWidget(widgets.at(3), 1, 1);
}

void frmVideoPlayRtsp::setUrl()
{
    UrlPara urlPara;
    urlPara.deviceIP = AppConfig::NvrDeviceIP;
    urlPara.userName = AppConfig::NvrUserName;
    urlPara.userPwd = AppConfig::NvrUserPwd;
    //通道从1开始标号
    urlPara.channel = ui->cboxChannel->currentIndex() + 1;
    urlPara.streamType = AppConfig::NvrStreamType;
    urlPara.companyType = (CompanyType)AppConfig::NvrCompanyType;
    urlPara.videoType = AppConfig::NvrVideoType;
    urlPara.dateTimeStart = ui->dateTimeStart->dateTime();
    urlPara.dateTimeEnd = ui->dateTimeEnd->dateTime();
    ui->txtUrl->setText(UrlHelper::getRtspUrl(urlPara));
}

void frmVideoPlayRtsp::positionChanged(int value)
{
    if (AppConfig::NvrSyncPosition) {
        //对其他几个通道全部同步播放进度
        foreach (VideoWidget *videoWidget, videoWidgets) {
            if (videoWidget->parent() != sender()) {
                videoWidget->setPosition(value);
            }
        }
    }
}

void frmVideoPlayRtsp::on_btnPlay_clicked()
{
    QString url = ui->txtUrl->toPlainText();
    if (url.isEmpty()) {
        return;
    }

    VideoWidget *videoWidget = videoWidgets.at(videoIndex);
    videoWidget->open(url);
    videoWidget->setFocus();

    //立即切换到下一个通道
    if (videoIndex < 3) {
        videoIndex = videoIndex + 1;
        videoWidgets.at(videoIndex)->setFocus();
        ui->labTip->setText(QString("当前选中 %1").arg(videoWidgets.at(videoIndex)->getBgText()));
        if (AppConfig::NvrAutoCh) {
            ui->cboxChannel->setCurrentIndex(ui->cboxChannel->currentIndex() + 1);
        }
    }
}

void frmVideoPlayRtsp::on_btnClose_clicked()
{
    foreach (VideoWidget *videoWidget, videoWidgets) {
        videoWidget->stop();
    }
}
