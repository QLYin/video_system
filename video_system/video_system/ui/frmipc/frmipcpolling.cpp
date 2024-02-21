#include "frmipcpolling.h"
#include "ui_frmipcpolling.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "videobox.h"
#include "videowidgetx.h"
#include "frmvideopanel.h"

frmIpcPolling::frmIpcPolling(QWidget *parent) : QWidget(parent), ui(new Ui::frmIpcPolling)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmIpcPolling::~frmIpcPolling()
{
    delete ui;
}

void frmIpcPolling::showEvent(QShowEvent *)
{
    //重新加载轮询分组并自动选中上次的轮询分组
    ui->listWidget->clear();
    QStringList list = AppConfig::PollGroupNames.split("|");
    ui->listWidget->addItems(list);
    int index = list.indexOf(AppConfig::PollGroupLast);
    if (index >= 0) {
        ui->listWidget->setCurrentRow(index);
    }
}

void frmIpcPolling::hideEvent(QHideEvent *)
{
    //自动保存最后选中的轮询分组
    if (ui->listWidget->count() > 0) {
        AppConfig::PollGroupLast = ui->listWidget->currentItem()->text();
        AppConfig::writeConfig();
    }
}

void frmIpcPolling::initForm()
{
    //轮询定时器
    pollIndex = 0;
    pollCount = 0;
    timerPoll = new QTimer(this);
    connect(timerPoll, SIGNAL(timeout()), this, SLOT(polling()));
    timerPoll->setInterval(30 * 1000);

    //独立的轮询提示信息悬浮在整个程序上
    QFont font;
    font.setPixelSize(GlobalConfig::FontSize + 20);
    label.setFont(font);
    label.setAlignment(Qt::AlignCenter);
    label.setWindowOpacity((qreal)AppConfig::WindowOpacity / 100);
    label.setGeometry(AppConfig::PollGeometry);
    QtHelper::setFramelessForm(&label, true, true, false);
}

void frmIpcPolling::initConfig()
{
    ui->btnAutoPoll->setChecked(AppConfig::AutoPoll);
    connect(ui->btnAutoPoll, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig()));

    QStringList listPollInterval;
    listPollInterval << "5秒钟" << "10秒钟" << "15秒钟" << "30秒钟" << "60秒钟" << "80秒钟" << "120秒钟";
    ui->cboxPollInterval->addItems(listPollInterval);
    QString pollInterval = QString("%1秒钟").arg(AppConfig::PollInterval);
    ui->cboxPollInterval->setCurrentIndex(ui->cboxPollInterval->findText(pollInterval));
    connect(ui->cboxPollInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList listPollType;
    listPollType << "1画面" << "4画面" << "9画面" << "16画面";
    ui->cboxPollType->addItems(listPollType);
    QString pollType = QString("%1画面").arg(AppConfig::PollType);
    ui->cboxPollType->setCurrentIndex(ui->cboxPollType->findText(pollType));
    connect(ui->cboxPollType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList listPollRtsp;
    listPollRtsp << "主码流" << "子码流";
    ui->cboxPollRtsp->addItems(listPollRtsp);
    ui->cboxPollRtsp->setCurrentIndex(AppConfig::PollRtsp);
    connect(ui->cboxPollRtsp, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
}

void frmIpcPolling::saveConfig()
{
    AppConfig::AutoPoll = ui->btnAutoPoll->getChecked();

    QString pollInterval = ui->cboxPollInterval->currentText();
    pollInterval = pollInterval.mid(0, pollInterval.length() - 2);
    AppConfig::PollInterval = pollInterval.toInt();

    QString pollType = ui->cboxPollType->currentText();
    pollType = pollType.mid(0, pollType.length() - 2);
    AppConfig::PollType = pollType.toInt();

    AppConfig::PollRtsp = ui->cboxPollRtsp->currentIndex();
    AppConfig::writeConfig();
}

void frmIpcPolling::polling()
{
    //如果摄像头列表数量为空则停止定时器
    int count = pollUrls.count();
    if (count == 0) {
        timerPoll->stop();
        setText();
        return;
    }

    if (pollIndex == count) {
        pollIndex = 0;
    }

    QStringList list = pollUrls.at(pollIndex).split(";");
    pollIndex++;
    pollCount++;
    setText();

    //先清空所有通道
    AppData::videoPanel->delete_video_all();
    qApp->processEvents();
    for (int i = 0; i < list.count(); ++i) {
        QString url = list.at(i);
        AppData::videoPanel->videoWidgets.at(i)->open(url);
        //AppData::videoPanel->videoWidgets.at(i)->open(url, 1000 + i * 100);

        AppData::VideoUrls[i] = url;
        DeviceHelper::saveUrls();
    }
}

void frmIpcPolling::setText()
{
    //统计轮询信息,如果不需要可以不显示
    int count = pollUrls.count();
    QString info = QString("第 %1 次 / 第 %2 组 / 共 %3 组").arg(pollCount).arg(pollIndex).arg(count);
    if (!timerPoll->isActive()) {
        info = QString("第 %1 次 / 第 %2 组 / 共 %3 组").arg(0).arg(0).arg(0);
    }

    ui->labTip->setText(info);
    label.setText(info);
}

void frmIpcPolling::getPollUrls()
{
    //保存最后选中的轮询分组
    AppConfig::PollGroupLast = ui->listWidget->currentItem()->text();
    AppConfig::writeConfig();

    //根据轮询画面数计算每次的url地址
    pollUrls.clear();
    QStringList list;
    for (int i = 0; i < DbData::PollInfo_Count; ++i) {
        //不是当前选中的轮询分组则跳过
        if (DbData::PollInfo_PollGroup.at(i) != AppConfig::PollGroupLast) {
            continue;
        }

        QString rtspMain = DbData::PollInfo_RtspMain.at(i);
        QString rtspSub = DbData::PollInfo_RtspSub.at(i);
        list << (AppConfig::PollRtsp == 0 ? rtspMain : rtspSub);
        if (list.count() == AppConfig::PollType) {
            pollUrls << list.join(";");
            list.clear();
        }
    }

    //没有达到轮询画面数的在这里添加
    if (list.count() > 0) {
        pollUrls << list.join(";");
    }
}

void frmIpcPolling::on_btnPollStart_clicked()
{
    //轮询期间不保存视频
    ui->btnPollPause->setText("暂停轮询");
    AppConfig::PollGeometry = label.geometry();
    if (AppData::videoPanel->actionPoll->text() == "启动轮询视频") {
        getPollUrls();
        if (pollUrls.count() == 0) {
            QtHelper::showMessageBoxError("当前分组没有视频需要轮询, 请在轮询配置中添加!", 3);
            return;
        }

        AppData::videoPanel->actionPoll->setText("停止轮询视频");
        ui->btnPollStart->setText("停止轮询");
        DeviceHelper::addMsg("所有通道 启动轮询");
        label.show();
    } else {
        AppData::videoPanel->actionPoll->setText("启动轮询视频");
        ui->btnPollStart->setText("启动轮询");
        DeviceHelper::addMsg("所有通道 停止轮询");
        label.hide();
    }

    //启动和停止轮询
    if (!AppConfig::Polling) {
        //切换到对应的轮询通道画面数
        AppConfig::VideoType = QString("1_%1").arg(AppConfig::PollType);
        //1通道需要特殊处理
        if (AppConfig::PollType == 1) {
            AppConfig::VideoType = "0_1";
        }
        AppConfig::writeConfig();
        AppData::videoPanel->videoBox->setVideoType(AppConfig::VideoType);
        AppData::videoPanel->videoBox->show_video(AppConfig::PollType, 0);

        //立马启动轮询,并执行一次
        pollIndex = 0;
        pollCount = 0;
        timerPoll->start(AppConfig::PollInterval * 1000);
        QMetaObject::invokeMethod(this, "polling");
        //QTimer::singleShot(1000, this, SLOT(polling()));
    } else {
        timerPoll->stop();
        setText();
    }

    AppConfig::Polling = !AppConfig::Polling;
    AppConfig::writeConfig();
}

void frmIpcPolling::on_btnPollPause_clicked()
{
    //轮询阶段则停止否则继续
    if (timerPoll->isActive()) {
        timerPoll->stop();
        ui->btnPollPause->setText("继续轮询");
        DeviceHelper::addMsg("所有通道 暂停轮询");
    } else if (AppConfig::Polling) {
        timerPoll->start();
        ui->btnPollPause->setText("暂停轮询");
        DeviceHelper::addMsg("所有通道 继续轮询");
    }
}
