#include "frmvideopaneltool.h"
#include "ui_frmvideopaneltool.h"
#include "widgetsound.h"
#include "qthelper.h"
#include "appinit.h"
#include "devicehelper.h"
#include "videobox.h"
#include "videomanage.h"
#include "frmvideopanel.h"

frmVideoPanelTool::frmVideoPanelTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoPanelTool)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
    QMetaObject::invokeMethod(this, "initVisible", Qt::QueuedConnection);
}

frmVideoPanelTool::~frmVideoPanelTool()
{
    delete ui;
}

bool frmVideoPanelTool::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut) {
        if (watched == widgetSound) {
            widgetSound->hide();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmVideoPanelTool::initForm()
{
    //生成音量条面板,上面音量条,底部静音
    widgetSound = new WidgetSound;
    connect(widgetSound, SIGNAL(soundValueChanged(int)), this, SLOT(soundValueChanged(int)));
    connect(widgetSound, SIGNAL(soundMutedChanged()), this, SLOT(soundMutedChanged()));
    widgetSound->installEventFilter(this);
    widgetSound->resize(50, 200);
    widgetSound->hide();

    //将通道按钮存入分组,更方便产生选中互相排斥效果
    QList<QPushButton *> btnChannels;
    btnChannels << ui->btnChannel1 << ui->btnChannel4 << ui->btnChannel6 << ui->btnChannel8 << ui->btnChannel9
                << ui->btnChannel13 << ui->btnChannel16 << ui->btnChannel25 << ui->btnChannel36 << ui->btnChannel64;
    btnGroup = new QButtonGroup(this);
    foreach (QPushButton *btn, btnChannels) {
        //设置按钮可选中
        btn->setCheckable(true);
        //设置属性值作为标记方便后面直接取属性进行判断
        QString objName = btn->objectName();
        objName.replace("btnChannel", "");
        btn->setProperty("channel", objName);
        //加入按钮组,产生排斥效果,一旦选中某个,组中其他自动取消选中
        btnGroup->addButton(btn);
    }

    //定时器执行报警图标闪烁
    timerAlarm = new QTimer(this);
    connect(timerAlarm, SIGNAL(timeout()), this, SLOT(doAlarm()));
    timerAlarm->setInterval(500);

    //绑定底部按钮信号槽
    QList<QPushButton *> btns = this->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
    }

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    //关联报警信号
    connect(AppEvent::Instance(), SIGNAL(startAlarm()), this, SLOT(startAlarm()));
    connect(AppEvent::Instance(), SIGNAL(stopAlarm()), this, SLOT(stopAlarm()));

    //绑定信号(选中通道/动态设置通道视频/交换通道)
    connect(AppEvent::Instance(), SIGNAL(selectVideo(int)), this, SLOT(selectVideo(int)));
    connect(AppEvent::Instance(), SIGNAL(loadVideo(int, QString)), this, SLOT(loadVideo(int, QString)));
    connect(AppEvent::Instance(), SIGNAL(changeVideo(int, int)), this, SLOT(changeVideo(int, int)));

    //绑定按键信号实现全屏切换
    connect(AppInit::Instance(), SIGNAL(keyPressed(QString)), this, SLOT(keyPressed(QString)));
    //绑定双击事件加载对应设备
    connect(AppEvent::Instance(), SIGNAL(itemDoubleClicked(QString, QString, bool)), this, SLOT(itemDoubleClicked(QString, QString, bool)));
}

void frmVideoPanelTool::initVisible()
{
    //根据设置哪些通道切换需要显示
    ui->btnChannel1->setVisible(AppConfig::EnableChannel1);
    ui->btnChannel4->setVisible(AppConfig::EnableChannel4);
    ui->btnChannel6->setVisible(AppConfig::EnableChannel6);
    ui->btnChannel8->setVisible(AppConfig::EnableChannel8);
    ui->btnChannel9->setVisible(AppConfig::EnableChannel9);
    ui->btnChannel13->setVisible(AppConfig::EnableChannel13);
    ui->btnChannel16->setVisible(AppConfig::EnableChannel16);
    ui->btnChannel25->setVisible(AppConfig::EnableChannel25);
    ui->btnChannel36->setVisible(AppConfig::EnableChannel36);
    ui->btnChannel64->setVisible(AppConfig::EnableChannel64);

#if 0
    //测试定时抓图抽帧
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), AppData::videoPanel, SLOT(snapshot_video_all()));
    timer->start(3000);
#endif
}

void frmVideoPanelTool::initIcon()
{
    //设置图形字体
    iconSize = 20;
    IconHelper::setIcon(ui->btnChannel1, 0xeb00, iconSize);
    IconHelper::setIcon(ui->btnChannel4, 0xeb01, iconSize);
    IconHelper::setIcon(ui->btnChannel6, 0xeb02, iconSize);
    IconHelper::setIcon(ui->btnChannel8, 0xeb03, iconSize);
    IconHelper::setIcon(ui->btnChannel9, 0xeb04, iconSize);
    IconHelper::setIcon(ui->btnChannel13, 0xeb05, iconSize);
    IconHelper::setIcon(ui->btnChannel16, 0xeb06, iconSize);
    IconHelper::setIcon(ui->btnChannel25, 0xeb07, iconSize);
    IconHelper::setIcon(ui->btnChannel36, 0xeb08, iconSize);
    IconHelper::setIcon(ui->btnChannel64, 0xeb0a, iconSize);
    IconHelper::setIcon(ui->btnSnap, 0xeb15, iconSize);
    IconHelper::setIcon(ui->btnFull, 0xe668, iconSize);
    IconHelper::setIcon(ui->btnAlarm, 0xe609, iconSize);
    this->initSound();
}

void frmVideoPanelTool::initSound()
{
    if (AppConfig::SoundMuted) {
        IconHelper::setIcon(ui->btnSound, 0xe667, iconSize);
        IconHelper::setIcon(widgetSound->getBtnMute(), 0xe667, iconSize);
    } else {
        IconHelper::setIcon(ui->btnSound, 0xe674, iconSize);
        IconHelper::setIcon(widgetSound->getBtnMute(), 0xe674, iconSize);
    }
}

void frmVideoPanelTool::buttonClicked()
{
    QPushButton *btn = (QPushButton *)sender();
    if (btn == ui->btnChannel1) {
        AppData::videoPanel->videoBox->change_video_1(0);
        AppConfig::VideoType = "0_1";
        AppConfig::VideoMax = true;
    } else if (btn == ui->btnChannel4) {
        AppData::videoPanel->videoBox->change_video_4(0);
        AppConfig::VideoType = "1_4";
    } else if (btn == ui->btnChannel6) {
        AppData::videoPanel->videoBox->change_video_6(0);
        AppConfig::VideoType = "1_6";
    } else if (btn == ui->btnChannel8) {
        AppData::videoPanel->videoBox->change_video_8(0);
        AppConfig::VideoType = "1_8";
    } else if (btn == ui->btnChannel9) {
        AppData::videoPanel->videoBox->change_video_9(0);
        AppConfig::VideoType = "1_9";
    } else if (btn == ui->btnChannel13) {
        AppData::videoPanel->videoBox->change_video_13(0);
        AppConfig::VideoType = "1_13";
    } else if (btn == ui->btnChannel16) {
        AppData::videoPanel->videoBox->change_video_16(0);
        AppConfig::VideoType = "1_16";
    } else if (btn == ui->btnChannel25) {
        AppData::videoPanel->videoBox->change_video_25(0);
        AppConfig::VideoType = "1_25";
    } else if (btn == ui->btnChannel36) {
        AppData::videoPanel->videoBox->change_video_36(0);
        AppConfig::VideoType = "1_36";
    } else if (btn == ui->btnChannel64) {
        AppData::videoPanel->videoBox->change_video_64(0);
        AppConfig::VideoType = "1_64";
    } else if (btn == ui->btnSnap) {
        AppData::videoPanel->snapshot_video_one();
    } else if (btn == ui->btnSound) {
        //弹出音量条进行控制
        //先计算音量按钮的全局坐标
        QPoint pos = this->mapToGlobal(ui->btnSound->pos());
        pos.setX(pos.x() - 18);
        pos.setY(pos.y() - widgetSound->height());
        widgetSound->move(pos);
        widgetSound->show();
        widgetSound->activateWindow();
        widgetSound->setFocus();
    } else if (btn == ui->btnAlarm) {
        //打开弹出报警详情面板
    } else if (btn == ui->btnFull) {
        AppData::videoPanel->full();
    }

    //重新设置画面类型
    AppData::videoPanel->videoBox->setVideoType(AppConfig::VideoType);
    AppConfig::writeConfig();
}

void frmVideoPanelTool::startAlarm()
{
    //启动报警闪烁定时器并立即执行一次
    timerAlarm->start();
    doAlarm();
}

void frmVideoPanelTool::stopAlarm()
{
    //停止报警闪烁定时器并恢复按钮的样式
    timerAlarm->stop();
    QString qss;// = QString("QPushButton{color:%1;}").arg(GlobalConfig::TextColor);
    qss += QString("QPushButton:hover{color:%1;}").arg(GlobalConfig::DarkColorStart);
    qss += QString("QPushButton:pressed{color:%1;}").arg(GlobalConfig::HighColor);
    ui->btnAlarm->setStyleSheet(qss);
}

void frmVideoPanelTool::doAlarm()
{
    //根据报警状态改变对应的样式
    static bool isAlarm = false;
    QString color = isAlarm ? GlobalConfig::TextColor : AppConfig::ColorIconAlarm;
    QString qss = QString("QPushButton{color:%1;}").arg(color);
    ui->btnAlarm->setStyleSheet(qss);
    isAlarm = !isAlarm;
}

void frmVideoPanelTool::soundValueChanged(int value)
{
    AppConfig::SoundValue = value;
    AppConfig::writeConfig();

    //调整所有视频控件的音量大小
    AppData::videoPanel->changeVideoConfig();
    for (int i = 0; i < AppConfig::VideoCount; ++i) {
        AppData::videoPanel->videoWidgets.at(i)->setVolume(AppConfig::SoundValue);
    }
}

void frmVideoPanelTool::soundMutedChanged()
{
    AppConfig::SoundMuted = !AppConfig::SoundMuted;
    AppConfig::writeConfig();
    this->initSound();

    //调整所有视频控件的静音状态
    AppData::videoPanel->changeVideoConfig();
    for (int i = 0; i < AppConfig::VideoCount; ++i) {
        AppData::videoPanel->videoWidgets.at(i)->setMuted(AppConfig::SoundMuted);
    }
}

void frmVideoPanelTool::changeVideo(int channel)
{
    //从通道按钮组中找到对应的按钮设置选中
    QAbstractButton *btn = 0;
    QList<QAbstractButton *> btns = btnGroup->buttons();
    foreach (QAbstractButton *b, btns) {
        int ch = b->property("channel").toInt();
        if (ch == channel) {
            btn = b;
            break;
        }
    }

    if (btn) {
        btn->setChecked(true);
    }
}

void frmVideoPanelTool::selectVideo(int channel)
{
    VideoWidget *videoWidget = AppData::videoPanel->videoWidgets.at(channel);
    if (!videoWidget->hasFocus()) {
        videoWidget->setFocus();
    }
}

void frmVideoPanelTool::loadVideo(int channel, const QString &url)
{
    VideoWidget *videoWidget = AppData::videoPanel->videoWidgets.at(channel);
    QString ch = videoWidget->getBgText();
    videoWidget->open(url);

    AppData::VideoUrls[channel] = url;
    DeviceHelper::saveUrls();
    DeviceHelper::addMsg(QString("%1 动态设置视频").arg(ch), 0, true);
}

void frmVideoPanelTool::changeVideo(int channel1, int channel2)
{
    //找到对应索引的控件
    VideoWidget *widget1 = AppData::videoPanel->videoWidgets.at(channel1);
    VideoWidget *widget2 = AppData::videoPanel->videoWidgets.at(channel2);
    //设置焦点加深突出显示
    widget2->setFocus();
    //重新设置新的控件
    AppData::videoPanel->videoWidgets[channel2] = widget1;
    AppData::videoPanel->videoWidgets[channel1] = widget2;

    //重新保存url
    AppData::VideoUrls[channel1] = widget2->getVideoPara().videoUrl;
    AppData::VideoUrls[channel2] = widget1->getVideoPara().videoUrl;
    DeviceHelper::saveUrls();

    //重新设置背景文字
    QString text1 = widget1->getBgText();
    QString text2 = widget2->getBgText();
    widget1->setBgText(text2);
    widget2->setBgText(text1);

    //打印信息
    DeviceHelper::addMsg(QString("%1 交换到 %2").arg(text1).arg(text2), 0, true);

    //需要重新设置并重新显示
    AppData::videoPanel->initVideoManage();
    AppData::videoPanel->setWidgets();
}

void frmVideoPanelTool::keyPressed(const QString &keys)
{
    //识别快捷键处理全屏切换
    if (keys == "esc") {
        AppData::videoPanel->doFull(false);
    } else if (keys == "alt+enter") {
        AppData::videoPanel->doFull(true);
    }
}

int frmVideoPanelTool::getFreeChannel()
{
    //找到末尾一个有视频的通道
    int index = -1;
    for (int i = 0; i < AppConfig::VideoCount; ++i) {
        QString url = AppData::VideoUrls.at(i);
        if (!url.isEmpty()) {
            index = i;
        }
    }

    //最后一个没有视频的通道=末尾一个有视频的通道+1
    return index + 1;
}

void frmVideoPanelTool::itemDoubleClicked(const QString &url, const QString &text, bool isNvr)
{
    if (url.isEmpty()) {
        return;
    }

    if (isNvr) {
        int index = 0;
        if (AppConfig::OpenNvrPolicy == 0) {
            index = getFreeChannel();
        } else {
            //清空所有的视频
            AppData::videoPanel->delete_video_all();
        }

        //找到当前父节点对应的所有子节点视频
        int channel = btnGroup->checkedButton()->property("channel").toInt();
        for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
            if (DbData::IpcInfo_NvrName.at(i) == text) {
                //最大通道数量=通道总数/当前几画面/低于16以16为准超过以当前几画面为准
                int maxCount = AppConfig::VideoCount;
                if (AppConfig::OpenMaxCount == 1) {
                    maxCount = channel;
                } else if (AppConfig::OpenMaxCount == 2) {
                    maxCount = channel < 16 ? 16 : channel;
                }

                if (index >= maxCount) {
                    break;
                }

                QString rtspAddr = DbData::getRtspAddr(i);
                AppData::VideoUrls[index] = rtspAddr;
                AppData::videoPanel->videoWidgets.at(index)->open(rtspAddr);
                //如果电脑性能好可以不延时(不延迟可能部分场景下会崩溃)
                QtHelper::sleep(500, false);
                index++;
            }
        }

        //保存视频地址,重新打开所有视频
        DeviceHelper::saveUrls();
        DeviceHelper::addMsg(QString("打开 %1").arg(text), 0, true);
    } else if (AppData::videoPanel->videoSelect) {
        //根据不同策略找到对应的通道控件
        VideoWidget *videoWidget = AppData::videoPanel->videoSelect;
        if (AppConfig::OpenIpcPolicy == 0) {
            videoWidget = AppData::videoPanel->videoWidgets.at(getFreeChannel());
        }

        //打开双击子节点的视频
        QString ch = videoWidget->getBgText();
        videoWidget->open(url);

        int index = AppData::videoPanel->videoWidgets.indexOf(videoWidget);
        AppData::VideoUrls[index] = url;
        DeviceHelper::saveUrls();
        DeviceHelper::addMsg(QString("%1 打开 %2").arg(ch).arg(text), 0, true);

        //主动移动到下一个通道并赋值给当前通道
        if (index < AppConfig::VideoCount - 1) {
            AppData::videoPanel->videoSelect = AppData::videoPanel->videoWidgets.at(index + 1);
            AppData::videoPanel->videoSelect->setFocus();
        }
    }
}
