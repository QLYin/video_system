#include "frmconfigsystem.h"
#include "ui_frmconfigsystem.h"
#include "qthelper.h"
#include "appstyle.h"
#include "savelog.h"
#include "saveruntime.h"
#include "dbconnthread.h"
#include "devicehelper.h"
#include "videoutil.h"
#include "videohelper.h"
#include "videowidgetx.h"

frmConfigSystem::frmConfigSystem(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigSystem)
{
    ui->setupUi(this);
    this->initForm();
}

frmConfigSystem::~frmConfigSystem()
{
    delete ui;
}

void frmConfigSystem::initForm()
{
    //设置左侧右侧面板的宽度
    ui->widgetLeft->setFixedWidth(480);
    ui->widgetRight->setFixedWidth(450);
    ui->line->setFixedHeight(1);

    //设置数据库设置面板的参数
    ui->widgetConfigDb->setConnFlag("video_system");
    //ui->widgetConfigDb->setBtnInRight(true);
    //ui->txtCopyright->setReadOnly(false);

    //在mac系统上要大一点才正常
#ifdef Q_OS_MAC
    ui->gridLayout1->setSpacing(15);
    ui->gridLayout2->setSpacing(15);
#endif

    this->initAppConfig1();
    this->initAppConfig2();
    this->initAppConfig3();

    this->initEnableConfig1();
    this->initEnableConfig2();

    this->initVideoConfig1();
    this->initVideoConfig2();
    this->initVideoConfig3();
    this->initVideoConfig4();

    this->initMapConfig();
    this->initColorConfig();
}

void frmConfigSystem::initAppConfig1()
{
    QStringList listWorkMode;
    listWorkMode << "视频监控" << "机器人监控" << "无人机监控" << "智慧校园";
    ui->cboxWorkMode->addItems(listWorkMode);
    ui->cboxWorkMode->setCurrentIndex(AppConfig::WorkMode);
    connect(ui->cboxWorkMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig1()));

    QStringList listNavStyle;
    listNavStyle << "左侧+左侧" << "左侧+上侧" << "上侧+左侧" << "上侧+上侧";
    ui->cboxNavStyle->addItems(listNavStyle);
    ui->cboxNavStyle->setCurrentIndex(AppConfig::NavStyle);
    connect(ui->cboxNavStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig1()));

    QStringList styleNames, styleFiles;
    GlobalStyle::getStyle(styleNames, styleFiles);
    for (int i = 0; i < styleNames.count(); ++i) {
        ui->cboxStyleName->addItem(styleNames.at(i), styleFiles.at(i));
    }

    ui->cboxStyleName->setCurrentIndex(ui->cboxStyleName->findData(AppConfig::StyleName));
    connect(ui->cboxStyleName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig1()));

    //图形字体图标
    QStringList icons;
    icons << ":/logo.svg" << "icon_0xea2a" << "icon_0xe9d0" << "icon_0xe9cf" << "icon_0xe9d5" << "icon_0xea07" << "icon_0xf1d1";
    ui->cboxLogoImage->addItems(icons);

    //从logo目录下加载所有图片
    QDir dir(QtHelper::appPath() + "/logo");
    QStringList fileNames = dir.entryList(QStringList() << "*.png");
    foreach (QString fileName, fileNames) {
        ui->cboxLogoImage->addItem(fileName.split(".").first());
    }

    int index = ui->cboxLogoImage->findText(AppConfig::LogoImage);
    ui->cboxLogoImage->setCurrentIndex(index < 0 ? 0 : index);
    connect(ui->cboxLogoImage, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig1()));

    ui->txtCopyright->setText(AppConfig::Copyright);
    connect(ui->txtCopyright, SIGNAL(textChanged(QString)), this, SLOT(saveAppConfig1()));

    ui->txtTitleCn->setText(AppConfig::TitleCn);
    connect(ui->txtTitleCn, SIGNAL(textChanged(QString)), this, SLOT(saveAppConfig1()));

    ui->txtTitleEn->setText(AppConfig::TitleEn);
    connect(ui->txtTitleEn, SIGNAL(textChanged(QString)), this, SLOT(saveAppConfig1()));
}

void frmConfigSystem::saveAppConfig1()
{
    int workMode = ui->cboxWorkMode->currentIndex();
    if (AppConfig::WorkMode != workMode) {
        //先要发个退出信号出去通知我要准备退出啦
        AppEvent::Instance()->slot_exitAll();
        AppConfig::WorkMode = workMode;
        AppConfig::writeConfig();
        FormHelper::reboot();
    }

    int navStyle = ui->cboxNavStyle->currentIndex();
    if (AppConfig::NavStyle != navStyle) {
        //先要发个退出信号出去通知我要准备退出啦
        AppEvent::Instance()->slot_exitAll();
        AppConfig::NavStyle = navStyle;
        AppConfig::writeConfig();
        FormHelper::reboot();
    }

    int styleIndex = ui->cboxStyleName->currentIndex();
    QString styleName = ui->cboxStyleName->itemData(styleIndex).toString();
    if (AppConfig::StyleName != styleName) {
        AppConfig::StyleName = styleName;
        AppStyle::initStyle();
        AppEvent::Instance()->slot_changeStyle();
    }

    QString logoImage = ui->cboxLogoImage->currentText().trimmed();
    if (AppConfig::LogoImage != logoImage) {
        AppConfig::LogoImage = logoImage;
        AppEvent::Instance()->slot_changeLogo();
        //立即设置全局图标
        if (AppConfig::LogoImage.startsWith("icon_")) {
            QString icon = AppConfig::LogoImage.split("_").last();
            GlobalConfig::IconMain = icon.toInt(NULL, 16);
        }
    }

    QString company = ui->txtCopyright->text().trimmed();
    if (AppConfig::Copyright != company) {
        AppConfig::Copyright = company;
        AppEvent::Instance()->slot_changeTitleInfo();
    }

    QString titleCn = ui->txtTitleCn->text().trimmed();
    if (AppConfig::TitleCn != titleCn) {
        AppConfig::TitleCn = titleCn;
        AppEvent::Instance()->slot_changeTitleInfo();
    }

    QString titleEn = ui->txtTitleEn->text().trimmed();
    if (AppConfig::TitleEn != titleEn) {
        AppConfig::TitleEn = titleEn;
        AppEvent::Instance()->slot_changeTitleInfo();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initAppConfig2()
{
    QStringList listTime, listTimex;
    listTime << "0 秒钟" << "10 秒钟" << "20 秒钟" << "30 秒钟" << "1 分钟" << "2 分钟" << "5 分钟";
    listTimex << "0" << "10" << "20" << "30" << "60" << "120" << "300";

    int count = listTime.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxTimeHideCursor->addItem(listTime.at(i), listTimex.at(i));
        ui->cboxTimeAutoFull->addItem(listTime.at(i), listTimex.at(i));
    }

    ui->cboxTimeHideCursor->setCurrentIndex(ui->cboxTimeHideCursor->findData(AppConfig::TimeHideCursor));
    ui->cboxTimeAutoFull->setCurrentIndex(ui->cboxTimeAutoFull->findData(AppConfig::TimeAutoFull));
    connect(ui->cboxTimeHideCursor, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));
    connect(ui->cboxTimeAutoFull, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));

    QStringList listCount, listCountx;
    listCount << "0 次" << "1 次" << "3 次" << "5 次" << "10 次" << "20 次" << "50 次" << "100 次";
    listCountx << "0" << "1" << "3" << "5" << "10" << "20" << "50" << "100";

    count = listCount.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxAlarmSoundCount->addItem(listCount.at(i), listCountx.at(i));
    }

    ui->cboxAlarmSoundCount->setCurrentIndex(ui->cboxAlarmSoundCount->findData(AppConfig::AlarmSoundCount));
    connect(ui->cboxAlarmSoundCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));

    QStringList msgCount, msgCountx;
    msgCount << "0 行" << "1 行" << "3 行" << "5 行" << "10 行" << "15 行" << "20 行" << "25 行" << "30 行" << "50 行";
    msgCountx << "0" << "1" << "3" << "5" << "10" << "15" << "20" << "25" << "30" << "50";

    count = msgCount.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxMsgListCount->addItem(msgCount.at(i), msgCountx.at(i));
        ui->cboxMsgTableCount->addItem(msgCount.at(i), msgCountx.at(i));
    }

    ui->cboxMsgListCount->setCurrentIndex(ui->cboxMsgListCount->findData(AppConfig::MsgListCount));
    ui->cboxMsgTableCount->setCurrentIndex(ui->cboxMsgTableCount->findData(AppConfig::MsgTableCount));
    connect(ui->cboxMsgListCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));
    connect(ui->cboxMsgTableCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));

    //0=一直显示 1000=禁用
    QStringList listTipInterval, listTipIntervalx;
    listTipInterval << "0 秒钟" << "3 秒钟" << "5 秒钟" << "10 秒钟" << "20 秒钟" << "30 秒钟" << "1 分钟" << "2 分钟" << "禁用";
    listTipIntervalx << "0" << "3" << "5" << "10" << "20" << "30" << "60" << "120" << "10000";

    count = listTipInterval.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxTipInterval->addItem(listTipInterval.at(i), listTipIntervalx.at(i));
    }

    ui->cboxTipInterval->setCurrentIndex(ui->cboxTipInterval->findData(AppConfig::TipInterval));
    connect(ui->cboxTipInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAppConfig2()));

    ui->labWindowOpacityValue->setText(QString::number(AppConfig::WindowOpacity));
    ui->sliderWindowOpacity->setValue(AppConfig::WindowOpacity);
    ui->sliderWindowOpacity->setRange(10, 100);
    connect(ui->sliderWindowOpacity, SIGNAL(valueChanged(int)), this, SLOT(saveAppConfig2()));
}

void frmConfigSystem::saveAppConfig2()
{
    AppConfig::TimeHideCursor = ui->cboxTimeHideCursor->itemData(ui->cboxTimeHideCursor->currentIndex()).toInt();
    AppConfig::TimeAutoFull = ui->cboxTimeAutoFull->itemData(ui->cboxTimeAutoFull->currentIndex()).toInt();
    AppConfig::AlarmSoundCount = ui->cboxAlarmSoundCount->itemData(ui->cboxAlarmSoundCount->currentIndex()).toInt();
    AppConfig::MsgListCount = ui->cboxMsgListCount->itemData(ui->cboxMsgListCount->currentIndex()).toInt();
    AppConfig::MsgTableCount = ui->cboxMsgTableCount->itemData(ui->cboxMsgTableCount->currentIndex()).toInt();
    AppConfig::TipInterval = ui->cboxTipInterval->itemData(ui->cboxTipInterval->currentIndex()).toInt();

    int windowOpacity = ui->sliderWindowOpacity->value();
    if (AppConfig::WindowOpacity != windowOpacity) {
        AppConfig::WindowOpacity = windowOpacity;
        ui->labWindowOpacityValue->setText(QString::number(AppConfig::WindowOpacity));
        AppEvent::Instance()->slot_changeWindowOpacity();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initAppConfig3()
{
    //从配置文件设置对应值到控件
    ui->btnAutoRun->setChecked(AppConfig::AutoRun);
    ui->btnAutoLogin->setChecked(AppConfig::AutoLogin);
    ui->btnAutoPwd->setChecked(AppConfig::AutoPwd);
    ui->btnSaveLog->setChecked(AppConfig::SaveLog);
    ui->btnSaveRunTime->setChecked(AppConfig::SaveRunTime);
    ui->btnFullScreen->setChecked(AppConfig::FullScreen);
    ui->btnRightInfo->setChecked(AppConfig::RightInfo);
    ui->btnSaveUrl->setChecked(AppConfig::SaveUrl);

    //绑定变化事件自动保存配置文件
    connect(ui->btnAutoRun, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnAutoLogin, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnAutoPwd, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnSaveLog, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnSaveRunTime, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnFullScreen, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnRightInfo, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));
    connect(ui->btnSaveUrl, SIGNAL(checkedChanged(bool)), this, SLOT(saveAppConfig3()));

    //日志记录设置
    SaveLog::Instance()->setPath(QtHelper::appPath() + "/log");
    if (AppConfig::SaveLog) {
        SaveLog::Instance()->start();
    }

    //运行时间设置
    SaveRunTime::Instance()->setPath(QtHelper::appPath() + "/log");
    if (AppConfig::SaveRunTime) {
        SaveRunTime::Instance()->start();
    }
}

void frmConfigSystem::saveAppConfig3()
{
    AppConfig::AutoRun = ui->btnAutoRun->getChecked();
    AppConfig::AutoLogin = ui->btnAutoLogin->getChecked();
    AppConfig::AutoPwd = ui->btnAutoPwd->getChecked();
    AppConfig::FullScreen = ui->btnFullScreen->getChecked();

    //立即开启或者停止调试日志记录
    bool saveLog = ui->btnSaveLog->getChecked();
    if (AppConfig::SaveLog != saveLog) {
        AppConfig::SaveLog = saveLog;
        AppConfig::SaveLog ? SaveLog::Instance()->start() : SaveLog::Instance()->stop();
    }

    //立即开启或者停止运行时间记录
    bool saveRunTime = ui->btnSaveRunTime->getChecked();
    if (AppConfig::SaveRunTime != saveRunTime) {
        AppConfig::SaveRunTime = saveRunTime;
        AppConfig::SaveRunTime ? SaveRunTime::Instance()->start() : SaveRunTime::Instance()->stop();
    }

    bool rightInfo = ui->btnRightInfo->getChecked();
    if (AppConfig::RightInfo != rightInfo) {
        AppConfig::RightInfo = rightInfo;
        AppEvent::Instance()->slot_changeTitleInfo();
    }

    AppConfig::SaveUrl = ui->btnSaveUrl->getChecked();
    AppConfig::writeConfig();
}

void frmConfigSystem::initVideoConfig1()
{
    ui->btnSharedData->setChecked(AppConfig::SharedData);
    connect(ui->btnSharedData, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig1()));

    VideoUtil::loadScaleMode(ui->cboxScaleMode);
    ui->cboxScaleMode->setCurrentIndex(AppConfig::ScaleMode);
    connect(ui->cboxScaleMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig1()));

    VideoUtil::loadVideoMode(ui->cboxVideoMode);
    ui->cboxVideoMode->setCurrentIndex(AppConfig::VideoMode);
    connect(ui->cboxVideoMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig1()));

    QStringList listBorderWidth, listBorderWidthx;
    listBorderWidth << "0 像素" << "2 像素" << "4 像素" << "6 像素" << "8 像素";
    listBorderWidthx << "0" << "2" << "4" << "6" << "8";

    int count = listBorderWidth.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxBorderWidth->addItem(listBorderWidth.at(i), listBorderWidthx.at(i));
    }

    ui->cboxBorderWidth->setCurrentIndex(ui->cboxBorderWidth->findData(AppConfig::BorderWidth));
    connect(ui->cboxBorderWidth, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig1()));

    ui->cboxBannerPosition->setCurrentIndex(AppConfig::BannerPosition);
    connect(ui->cboxBannerPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig1()));
}

void frmConfigSystem::saveVideoConfig1()
{
    bool sharedData = ui->btnSharedData->getChecked();
    if (AppConfig::SharedData != sharedData) {
        AppConfig::SharedData = sharedData;
        AppEvent::Instance()->slot_changeVideoConfig();
    }

    int scaleMode = ui->cboxScaleMode->currentIndex();
    if (AppConfig::ScaleMode != scaleMode) {
        AppConfig::ScaleMode = scaleMode;
        AppEvent::Instance()->slot_changeVideoConfig();
    }

    AppConfig::VideoMode = ui->cboxVideoMode->currentIndex();

    int borderWidth = ui->cboxBorderWidth->itemData(ui->cboxBorderWidth->currentIndex()).toInt();
    if (AppConfig::BorderWidth != borderWidth) {
        AppConfig::BorderWidth = borderWidth;
        AppEvent::Instance()->slot_changeVideoConfig();
    }

    int bannerPosition = ui->cboxBannerPosition->currentIndex();
    if (AppConfig::BannerPosition != bannerPosition) {
        AppConfig::BannerPosition = bannerPosition;
        AppEvent::Instance()->slot_changeVideoConfig();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initVideoConfig2()
{
    VideoCore videoCore = VideoHelper::getVideoCore();

    ui->btnAudioLevel->setChecked(AppConfig::AudioLevel);
    connect(ui->btnAudioLevel, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig2()));

    ui->btnPlayAudio->setChecked(AppConfig::PlayAudio);
    connect(ui->btnPlayAudio, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadHardware(ui->cboxHardware, videoCore, AppConfig::Hardware);
    connect(ui->cboxHardware, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadTransport(ui->cboxTransport);
    ui->cboxTransport->setCurrentIndex(ui->cboxTransport->findText(AppConfig::Transport));
    connect(ui->cboxTransport, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadDecodeType(ui->cboxDecodeType);
    ui->cboxDecodeType->setCurrentIndex(AppConfig::DecodeType);
    connect(ui->cboxDecodeType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadEncodeType(ui->cboxEncodeType);
    ui->cboxEncodeType->setCurrentIndex(AppConfig::EncodeType);
    connect(ui->cboxEncodeType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadCaching(ui->cboxCaching, videoCore, AppConfig::Caching);
    connect(ui->cboxCaching, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadReadTimeout(ui->cboxReadTimeout);
    ui->cboxReadTimeout->setCurrentIndex(ui->cboxReadTimeout->findData(QString::number(AppConfig::ReadTimeout)));
    connect(ui->cboxReadTimeout, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));

    VideoUtil::loadConnectTimeout(ui->cboxConnectTimeout);
    ui->cboxConnectTimeout->setCurrentIndex(ui->cboxConnectTimeout->findData(QString::number(AppConfig::ConnectTimeout)));
    connect(ui->cboxConnectTimeout, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig2()));
}

void frmConfigSystem::saveVideoConfig2()
{
    AppConfig::AudioLevel = ui->btnAudioLevel->getChecked();
    AppConfig::PlayAudio = ui->btnPlayAudio->getChecked();
    AppConfig::Hardware = ui->cboxHardware->currentText();
    AppConfig::Transport = ui->cboxTransport->currentText();
    AppConfig::DecodeType = ui->cboxDecodeType->currentIndex();
    AppConfig::EncodeType = ui->cboxEncodeType->currentIndex();
    AppConfig::Caching = ui->cboxCaching->itemData(ui->cboxCaching->currentIndex()).toInt();
    AppConfig::ReadTimeout = ui->cboxReadTimeout->itemData(ui->cboxReadTimeout->currentIndex()).toInt();
    AppConfig::ConnectTimeout = ui->cboxConnectTimeout->itemData(ui->cboxConnectTimeout->currentIndex()).toInt();
    AppConfig::writeConfig();
}

void frmConfigSystem::initVideoConfig3()
{
    ui->btnVideoDrag->setChecked(AppConfig::VideoDrag);
    connect(ui->btnVideoDrag, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig3()));

    ui->btnSaveVideo->setChecked(AppConfig::SaveVideo);
    connect(ui->btnSaveVideo, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig3()));

    ui->btnVideoSmart->setChecked(AppConfig::VideoSmart);
    connect(ui->btnVideoSmart, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig3()));

    ui->btnOnvifEvent->setChecked(AppConfig::OnvifEvent);
    connect(ui->btnOnvifEvent, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig3()));

    ui->btnOnvifNtp->setChecked(AppConfig::OnvifNtp);
    connect(ui->btnOnvifNtp, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig3()));
}

void frmConfigSystem::saveVideoConfig3()
{
    //存储录像开启与关闭都要更新录像计划
    bool saveVideo = ui->btnSaveVideo->getChecked();
    if (AppConfig::SaveVideo != saveVideo) {
        AppConfig::SaveVideo = saveVideo;
        AppEvent::Instance()->slot_changeVideoManage();
    }

    AppConfig::VideoDrag = ui->btnVideoDrag->getChecked();
    AppConfig::VideoSmart = ui->btnVideoSmart->getChecked();
    AppConfig::OnvifEvent = ui->btnOnvifEvent->getChecked();
    AppConfig::OnvifNtp = ui->btnOnvifNtp->getChecked();
    AppConfig::writeConfig();
}

void frmConfigSystem::initVideoConfig4()
{
    ui->btnCheckOffline->setChecked(AppConfig::CheckOffline);
    connect(ui->btnCheckOffline, SIGNAL(checkedChanged(bool)), this, SLOT(saveVideoConfig4()));

    ui->cboxRtspType->setCurrentIndex(AppConfig::RtspType);
    connect(ui->cboxRtspType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxAlarmSaveTime->addItem("10 秒", 10);
    ui->cboxAlarmSaveTime->addItem("30 秒", 30);
    ui->cboxAlarmSaveTime->addItem("60 秒", 60);
    ui->cboxAlarmSaveTime->setCurrentIndex(ui->cboxAlarmSaveTime->findData(QString::number(AppConfig::AlarmSaveTime)));
    connect(ui->cboxAlarmSaveTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxOpenIpcPolicy->addItem("最后空白通道");
    ui->cboxOpenIpcPolicy->addItem("最后按下通道");
    ui->cboxOpenIpcPolicy->setCurrentIndex(AppConfig::OpenIpcPolicy);
    connect(ui->cboxOpenIpcPolicy, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxOpenNvrPolicy->addItem("最后空白通道");
    ui->cboxOpenNvrPolicy->addItem("从头清空通道");
    ui->cboxOpenNvrPolicy->setCurrentIndex(AppConfig::OpenNvrPolicy);
    connect(ui->cboxOpenNvrPolicy, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxOpenMaxCount->addItem("最大通道数量");
    ui->cboxOpenMaxCount->addItem("当前通道数量");
    ui->cboxOpenMaxCount->addItem("自动调整数量");
    ui->cboxOpenMaxCount->setCurrentIndex(AppConfig::OpenMaxCount);
    connect(ui->cboxOpenMaxCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxTableDataPolicy->addItem("单选按下编辑");
    ui->cboxTableDataPolicy->addItem("多选双击编辑");
    ui->cboxTableDataPolicy->setCurrentIndex(AppConfig::TableDataPolicy);
    connect(ui->cboxTableDataPolicy, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));

    ui->cboxChannelBgText->addItem("0-通道1");
    ui->cboxChannelBgText->addItem("1-通道01");
    ui->cboxChannelBgText->addItem("2-设备名称");
    ui->cboxChannelBgText->setCurrentIndex(AppConfig::ChannelBgText);
    connect(ui->cboxChannelBgText, SIGNAL(currentIndexChanged(int)), this, SLOT(saveVideoConfig4()));
}

void frmConfigSystem::saveVideoConfig4()
{
    AppConfig::CheckOffline = ui->btnCheckOffline->getChecked();
    AppConfig::RtspType = ui->cboxRtspType->currentIndex();
    AppConfig::AlarmSaveTime = ui->cboxAlarmSaveTime->itemData(ui->cboxAlarmSaveTime->currentIndex()).toInt();
    AppConfig::OpenIpcPolicy = ui->cboxOpenIpcPolicy->currentIndex();
    AppConfig::OpenNvrPolicy = ui->cboxOpenNvrPolicy->currentIndex();
    AppConfig::OpenMaxCount = ui->cboxOpenMaxCount->currentIndex();
    AppConfig::TableDataPolicy = ui->cboxTableDataPolicy->currentIndex();
    AppConfig::ChannelBgText = ui->cboxChannelBgText->currentIndex();
    AppConfig::writeConfig();
}

void frmConfigSystem::initEnableConfig1()
{
    ui->ckEnableChannelx->setChecked(AppConfig::EnableChannelx);
    connect(ui->ckEnableChannelx, SIGNAL(stateChanged(int)), this, SLOT(saveEnableConfig1()));

    ui->ckEnableChannel1->setChecked(AppConfig::EnableChannel1);
    ui->ckEnableChannel4->setChecked(AppConfig::EnableChannel4);
    ui->ckEnableChannel6->setChecked(AppConfig::EnableChannel6);
    ui->ckEnableChannel8->setChecked(AppConfig::EnableChannel8);
    ui->ckEnableChannel9->setChecked(AppConfig::EnableChannel9);
    ui->ckEnableChannel13->setChecked(AppConfig::EnableChannel13);
    ui->ckEnableChannel16->setChecked(AppConfig::EnableChannel16);
    ui->ckEnableChannel25->setChecked(AppConfig::EnableChannel25);
    ui->ckEnableChannel36->setChecked(AppConfig::EnableChannel36);
    ui->ckEnableChannel64->setChecked(AppConfig::EnableChannel64);

    //懒得一个个写直接循环遍历就行
    QList<QCheckBox *> ckboxs = ui->widgetEnable1->findChildren<QCheckBox *>();
    foreach (QCheckBox *ckbox, ckboxs) {
        connect(ckbox, SIGNAL(stateChanged(int)), this, SLOT(saveEnableConfig1()));
    }
}

void frmConfigSystem::saveEnableConfig1()
{
    AppConfig::EnableChannelx = ui->ckEnableChannelx->isChecked();
    AppConfig::EnableChannel1 = ui->ckEnableChannel1->isChecked();
    AppConfig::EnableChannel4 = ui->ckEnableChannel4->isChecked();
    AppConfig::EnableChannel6 = ui->ckEnableChannel6->isChecked();
    AppConfig::EnableChannel8 = ui->ckEnableChannel8->isChecked();
    AppConfig::EnableChannel9 = ui->ckEnableChannel9->isChecked();
    AppConfig::EnableChannel13 = ui->ckEnableChannel13->isChecked();
    AppConfig::EnableChannel16 = ui->ckEnableChannel16->isChecked();
    AppConfig::EnableChannel25 = ui->ckEnableChannel25->isChecked();
    AppConfig::EnableChannel36 = ui->ckEnableChannel36->isChecked();
    AppConfig::EnableChannel64 = ui->ckEnableChannel64->isChecked();
    AppConfig::writeConfig();
}

void frmConfigSystem::initEnableConfig2()
{
    ui->ckEnableMain1->setChecked(AppConfig::EnableMain1);
    ui->ckEnableWeather->setChecked(AppConfig::EnableWeather);
    ui->ckEnableVideoPanelTool->setChecked(AppConfig::EnableVideoPanelTool);
    ui->ckEnableMapImage->setChecked(AppConfig::EnableMapImage);
    ui->ckEnableVideoPlayWeb->setChecked(AppConfig::EnableVideoPlayWeb);
    ui->ckEnableVideoPlayNvr->setChecked(AppConfig::EnableVideoPlayNvr);
    ui->ckEnableVideoPlayImage->setChecked(AppConfig::EnableVideoPlayImage);
    ui->ckEnableVideoUpload->setChecked(AppConfig::EnableVideoUpload);
    ui->ckEnableConfigPoll->setChecked(AppConfig::EnableConfigPoll);
    ui->ckEnableConfigUser->setChecked(AppConfig::EnableConfigUser);
    ui->ckEnableConfigRecord->setChecked(AppConfig::EnableConfigRecord);
    ui->ckEnableConfigOther->setChecked(AppConfig::EnableConfigOther);
    ui->ckEnableTitleMenu->setChecked(AppConfig::EnableTitleMenu);
    ui->ckEnableModelMenu->setChecked(AppConfig::EnableModelMenu);

    //懒得一个个写直接循环遍历就行
    QList<QCheckBox *> ckboxs = ui->widgetEnable2->findChildren<QCheckBox *>();
    foreach (QCheckBox *ckbox, ckboxs) {
        connect(ckbox, SIGNAL(stateChanged(int)), this, SLOT(saveEnableConfig2()));
    }
}

void frmConfigSystem::saveEnableConfig2()
{
    AppConfig::EnableMain1 = ui->ckEnableMain1->isChecked();
    AppConfig::EnableWeather = ui->ckEnableWeather->isChecked();
    AppConfig::EnableVideoPanelTool = ui->ckEnableVideoPanelTool->isChecked();
    AppConfig::EnableMapImage = ui->ckEnableMapImage->isChecked();
    AppConfig::EnableVideoPlayWeb = ui->ckEnableVideoPlayWeb->isChecked();
    AppConfig::EnableVideoPlayNvr = ui->ckEnableVideoPlayNvr->isChecked();
    AppConfig::EnableVideoPlayImage = ui->ckEnableVideoPlayImage->isChecked();
    AppConfig::EnableVideoUpload = ui->ckEnableVideoUpload->isChecked();
    AppConfig::EnableConfigPoll = ui->ckEnableConfigPoll->isChecked();
    AppConfig::EnableConfigUser = ui->ckEnableConfigUser->isChecked();
    AppConfig::EnableConfigRecord = ui->ckEnableConfigRecord->isChecked();
    AppConfig::EnableConfigOther = ui->ckEnableConfigOther->isChecked();
    AppConfig::EnableTitleMenu = ui->ckEnableTitleMenu->isChecked();
    AppConfig::EnableModelMenu = ui->ckEnableModelMenu->isChecked();
    AppConfig::writeConfig();
}

void frmConfigSystem::initMapConfig()
{
    ui->txtMapVersionKey->setText(AppConfig::MapVersionKey);
    connect(ui->txtMapVersionKey, SIGNAL(textChanged(QString)), this, SLOT(saveMapConfig()));

    ui->txtMapCenter->setText(AppConfig::MapCenter);
    connect(ui->txtMapCenter, SIGNAL(textChanged(QString)), this, SLOT(saveMapConfig()));

    for (int i = 6; i <= 20; ++i) {
        ui->cboxMapZoom->addItem(QString("缩放级别 %1").arg(i), i);
    }

    ui->cboxMapZoom->setCurrentIndex(ui->cboxMapZoom->findData(AppConfig::MapZoom));
    connect(ui->cboxMapZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(saveMapConfig()));
}

void frmConfigSystem::saveMapConfig()
{
    AppConfig::MapVersionKey = ui->txtMapVersionKey->text().trimmed();
    AppConfig::MapCenter = ui->txtMapCenter->text().trimmed();
    AppConfig::MapZoom = ui->cboxMapZoom->itemData(ui->cboxMapZoom->currentIndex()).toInt();
    AppConfig::writeConfig();
}

void frmConfigSystem::initColorConfig()
{
    ui->cboxColorTextNormal->setColorName(AppConfig::ColorTextNormal);
    connect(ui->cboxColorTextNormal, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));

    ui->cboxColorTextWarn->setColorName(AppConfig::ColorTextWarn);
    connect(ui->cboxColorTextWarn, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));

    ui->cboxColorTextAlarm->setColorName(AppConfig::ColorTextAlarm);
    connect(ui->cboxColorTextAlarm, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));

    ui->cboxColorIconAlarm->setColorName(AppConfig::ColorIconAlarm);
    connect(ui->cboxColorIconAlarm, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));

    ui->cboxColorMsgWait->setColorName(AppConfig::ColorMsgWait);
    connect(ui->cboxColorMsgWait, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));

    ui->cboxColorMsgSolved->setColorName(AppConfig::ColorMsgSolved);
    connect(ui->cboxColorMsgSolved, SIGNAL(colorChanged(QString)), this, SLOT(saveColorConfig()));
}

void frmConfigSystem::saveColorConfig()
{
    AppConfig::ColorTextNormal = ui->cboxColorTextNormal->getColorName();
    AppConfig::ColorTextWarn = ui->cboxColorTextWarn->getColorName();
    AppConfig::ColorTextAlarm = ui->cboxColorTextAlarm->getColorName();
    AppConfig::ColorIconAlarm = ui->cboxColorIconAlarm->getColorName();
    AppConfig::ColorMsgWait = ui->cboxColorMsgWait->getColorName();
    AppConfig::ColorMsgSolved = ui->cboxColorMsgSolved->getColorName();
    AppConfig::writeConfig();
}
