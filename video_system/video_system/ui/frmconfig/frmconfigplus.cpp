#include "frmconfigplus.h"
#include "ui_frmconfigplus.h"
#include "qthelper.h"
#include "urlhelper.h"

SINGLETON_IMPL(frmConfigPlus)
frmConfigPlus::frmConfigPlus(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigPlus)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    QtHelper::setFormInCenter(this);
}

frmConfigPlus::~frmConfigPlus()
{
    delete ui;
}

void frmConfigPlus::initForm()
{
    this->setWindowTitle("批量添加");
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setFixedSize(this->size());
}

void frmConfigPlus::initConfig()
{
    ui->cboxPlusType->addItem("文件");
    ui->cboxPlusType->addItems(AppData::IpcTypes);
    ui->cboxPlusType->setCurrentIndex(AppConfig::PlusType);
    connect(ui->cboxPlusType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList listNumber;
    listNumber << "2" << "4" << "9" << "16" << "25" << "36" << "48" << "64" << "100";
    ui->cboxPlusNumber->addItems(listNumber);
    ui->cboxPlusNumber->lineEdit()->setText(QString::number(AppConfig::PlusNumber));
    connect(ui->cboxPlusNumber->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckPlusNvr->setChecked(AppConfig::PlusNvr);
    connect(ui->ckPlusNvr, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->txtPlusAddr->setText(AppConfig::PlusAddr);
    connect(ui->txtPlusAddr, SIGNAL(textEdited(QString)), this, SLOT(saveConfig()));

    ui->txtPlusMain->setText(AppConfig::PlusMain);
    connect(ui->txtPlusMain, SIGNAL(textChanged()), this, SLOT(saveConfig()));

    ui->txtPlusSub->setText(AppConfig::PlusSub);
    connect(ui->txtPlusSub, SIGNAL(textChanged()), this, SLOT(saveConfig()));
}

void frmConfigPlus::saveConfig()
{
    AppConfig::PlusType = ui->cboxPlusType->currentIndex();
    AppConfig::PlusNumber = ui->cboxPlusNumber->lineEdit()->text().toInt();
    AppConfig::PlusNvr = ui->ckPlusNvr->isChecked();
    AppConfig::PlusAddr = ui->txtPlusAddr->text().trimmed();
    AppConfig::PlusMain = ui->txtPlusMain->toPlainText();
    AppConfig::PlusSub = ui->txtPlusSub->toPlainText();
    AppConfig::writeConfig();
}

void frmConfigPlus::on_btnAddPlus_clicked()
{
    //替换对应的地址并自动递增
    QStringList rtspMains, rtspSubs;
    for (int i = 0; i < AppConfig::PlusNumber; ++i) {
        QString target = AppConfig::PlusAddr;
        QString rtspMain = AppConfig::PlusMain;
        QString rtspSub = AppConfig::PlusSub;        
        if (QtHelper::isIP(AppConfig::PlusAddr)) {
            //通道递增的另外格式只递增通道不递增地址
            if (AppConfig::PlusNvr) {
                QString userName = "admin";
                QString userPwd = "12345";
                int index = rtspMain.lastIndexOf("@");
                if (index > 0) {
                    QString userInfo = rtspMain.mid(0, index);
                    userInfo.replace("rtsp://", "");
                    QStringList list = userInfo.split(":");
                    userName = list.at(0);
                    userPwd = list.at(1);
                }

                int ch = i + 1;
                QString head = QString("rtsp://%1:%2").arg(userName).arg(userPwd);
                if (AppConfig::PlusType == 1) {
                    rtspMain = head + QString("@[Addr]:554/Streaming/Channels/%101?transportmode=unicast").arg(ch);
                    rtspSub = head + QString("@[Addr]:554/Streaming/Channels/%102?transportmode=unicast").arg(ch);
                } else if (AppConfig::PlusType == 2) {
                    rtspMain = head + QString("@[Addr]:554/cam/realmonitor?channel=%1&subtype=0&unicast=true&proto=Onvif").arg(ch);
                    rtspSub = head + QString("@[Addr]:554/cam/realmonitor?channel=%1&subtype=1&unicast=true&proto=Onvif").arg(ch);
                } else if (AppConfig::PlusType == 4) {
                    rtspMain = head + QString("@[Addr]/live?channel=%1&stream=0").arg(ch);
                    rtspSub = head + QString("@[Addr]/live?channel=%1&stream=1").arg(ch);
                }
            } else {
                QStringList list = AppConfig::PlusAddr.split(".");
                target = QString::number(i + list.at(3).toInt());
                target = QString("%1.%2.%3.%4").arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(target);
            }
        } else {
            target = QString::number(i + AppConfig::PlusAddr.toInt());            
        }

        rtspMain.replace("[Addr]", target);
        rtspSub.replace("[Addr]", target);
        rtspMains << rtspMain;
        rtspSubs << rtspSub;
    }

    emit addPlus(rtspMains, rtspSubs);
}

void frmConfigPlus::on_cboxPlusType_currentIndexChanged(int index)
{
    //切换类型自动填入对应的格式(可以自行增加其他格式)
    QString rtspMain, rtspSub;
    if (index == 0) {
        rtspMain = "f:/mp4/push/[Addr].mp4";
        rtspSub = "f:/mp4/push/[Addr].mp4";
    } else if (index >= 1 && index <= 4) {
        CompanyType companyType = (CompanyType)index;
        rtspMain = UrlHelper::getRtspUrl(companyType, 0, 0);
        rtspSub = UrlHelper::getRtspUrl(companyType, 0, 1);
    } else {
        rtspMain = UrlHelper::getRtspUrl(CompanyType_Normal, 0, 0);
        rtspSub = UrlHelper::getRtspUrl(CompanyType_Normal, 0, 1);
    }

    ui->txtPlusMain->setText(rtspMain);
    ui->txtPlusSub->setText(rtspSub);
}
