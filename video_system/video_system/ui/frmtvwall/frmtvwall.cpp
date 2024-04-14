#include "frmtvwall.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QDrag>
#include <QMimeData>

#include "qthelper.h"
#include "frmtvwallwidget.h"
#include "frmscenewidget.h"
#include "frmdevicetree.h"
#include "frmwallsetdialog.h"
#include "class/appmisc/appmisc.h"
#include "class/devicemanager/tvwallmanager.h"
#include "class/devicemanager/scenemanager.h"
#include "ui/frmbase/Indicator.h"
#include "../deviceconnect/tcpclient.h"

struct Camera {
    int id;
    QString name;
    QString rtsp_url;
    QString ipaddr;
};

frmTVWall::frmTVWall(QWidget* parent) : QWidget(parent)
{
    this->initForm();
}

frmTVWall::~frmTVWall()
{
}

void frmTVWall::initForm()
{
    auto widgetTop = new QWidget(this);
    widgetTop->setFixedHeight(30);
    widgetTop->setObjectName(QString::fromUtf8("widgetTop"));
    auto topLayout = new QHBoxLayout(widgetTop);
    topLayout->setSpacing(6);
    topLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    topLayout->setContentsMargins(0, 0, 0, 0);
    btnReresh = new QPushButton(widgetTop);
    btnReresh->setObjectName(QString::fromUtf8("btnReresh"));
    btnReresh->setText(QString::fromLocal8Bit("ˢ��"));
    btnReresh->setFixedWidth(70);
    topLayout->addWidget(btnReresh);
    connect(btnReresh, &QPushButton::clicked, this, &frmTVWall::on_btnRereshClicked);

    btnCreateWall = new QPushButton(widgetTop);
    btnCreateWall->setObjectName(QString::fromUtf8("btnCreateWall"));
    btnCreateWall->setText(QString::fromLocal8Bit("����Ļǽ"));
    btnCreateWall->setFixedWidth(70);
    topLayout->addWidget(btnCreateWall);
    connect(btnCreateWall, &QPushButton::clicked, this, &frmTVWall::on_btnCreateWallClicked);

    btnCallAll = new QPushButton(widgetTop);
    btnCallAll->setObjectName(QString::fromUtf8("btnCallAll"));
    btnCallAll->setText(QString::fromLocal8Bit("ȫ������"));
    btnCallAll->setFixedWidth(70);
    topLayout->addWidget(btnCallAll);
    connect(btnCallAll, &QPushButton::clicked, this, &frmTVWall::on_btnCallAllClicked);

    btnCloseAll = new QPushButton(widgetTop);
    btnCloseAll->setObjectName(QString::fromUtf8("btnCloseAll"));
    btnCloseAll->setText(QString::fromLocal8Bit("ȫ���ر�"));
    btnCloseAll->setFixedWidth(70);
    topLayout->addWidget(btnCloseAll);
    connect(btnCloseAll, &QPushButton::clicked, this, &frmTVWall::on_btnCloseAllClicked);

    btnConnect = new QPushButton(widgetTop);
    btnConnect->setObjectName(QString::fromUtf8("btnCreateWall"));
    btnConnect->setText(QString::fromLocal8Bit("����"));
    btnConnect->setFixedWidth(70);
    topLayout->addWidget(btnConnect);
    connect(btnConnect, &QPushButton::clicked, this, &frmTVWall::on_btnConnectClicked);

    btnScene = new QPushButton(widgetTop);
    btnScene->setObjectName(QString::fromUtf8("btnScene"));
    btnScene->setText(QString::fromLocal8Bit("������ѯ"));
    btnScene->setCheckable(true);
    btnScene->setChecked(false);
    btnScene->setFixedWidth(70);
    topLayout->addWidget(btnScene);
    connect(btnScene, &QPushButton::clicked, this, &frmTVWall::on_btnSceneClicked);
    topLayout->addStretch();

    QWidget* pGroup1 = new QWidget;
    auto treeWidget = new frmDeviceTree(pGroup1, "tvwall");
    QVBoxLayout* pLayout1 = new QVBoxLayout(pGroup1);
    pLayout1->setMargin(0);
    pLayout1->setAlignment(Qt::AlignmentFlag::AlignLeft);
    pLayout1->addWidget(treeWidget);

    QWidget* pGroup2 = new QWidget;
    QTabWidget* pTab = new QTabWidget(this);
    pTab->setFixedWidth(205);
    pTab->addTab(pGroup1, "ipc");
    pTab->addTab(pGroup2, "telephone");

    QWidget* rightContainer = new QWidget(this);
    m_tvWallWidget = new frmTVWallWidget(rightContainer);
    m_sceneWidget = new frmSceneWidget(rightContainer);
    m_sceneWidget->setFixedHeight(72);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(2);
    rightLayout->setMargin(4);
    rightLayout->addWidget(m_tvWallWidget);
    rightLayout->addWidget(m_sceneWidget);
    m_sceneWidget->setVisible(btnScene->isChecked());

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(pTab);
    hLayout->addWidget(rightContainer);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
    mainLayout->addWidget(widgetTop);
    mainLayout->addSpacing(2);
    mainLayout->addLayout(hLayout);

    TVWallManager::Instance()->initWallWidget(m_tvWallWidget, this);
    SceneManager::Instance()->initSceneWidget(m_sceneWidget);
    connect(TcpClient::Instance(), &TcpClient::socketDisconnect, this, [this]()
        {
            Indicator::showTopTip(QString::fromLocal8Bit("�豸�����쳣, ����������"), nullptr);
        });
}

void frmTVWall::on_btnRereshClicked()
{
    QVariantMap param;
    param["type"] = 1;
    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
    //CmdHandlerMgr::Instance()->sendCmd("nop");
    param["type"] = 4;
    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
    //CmdHandlerMgr::Instance()->sendCmd("nop");

    Indicator::showLoading(true, nullptr);
    QTimer::singleShot(3000, this, [=]()
        {
            Indicator::showLoading(false, nullptr);

        });
    Indicator::showTopTip(QString::fromLocal8Bit("����ˢ�£����Ժ�.."), nullptr);

}

void frmTVWall::on_btnCreateWallClicked()
{
    frmWallSetDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        auto userData = dialog.userData();
        int row = userData["rows"].toInt();
        int col = userData["cols"].toInt();
        m_tvWallWidget->createTVWall(row, col);
        TVWallManager::Instance()->sendWallJoint(row, col);
    }
}

void frmTVWall::on_btnCallAllClicked()
{
    if (QtHelper::showMessageBoxQuestion("ȷ��Ҫȫ������") == QMessageBox::Yes)
    {
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdCallAllVideo);
        Indicator::showLoading(true, nullptr);
        QTimer::singleShot(3000, this, [=]()
            {
                Indicator::showLoading(false, nullptr);

            });
        Indicator::showTopTip(QString::fromLocal8Bit("���ڵ��룬���Ժ�.."), nullptr);
  
        QVariantMap param;
        param["type"] = 4;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        CmdHandlerMgr::Instance()->sendCmd("nop");
    }
}

void frmTVWall::on_btnCloseAllClicked()
{
    if (QtHelper::showMessageBoxQuestion("ȷ��Ҫȫ���ر�") == QMessageBox::Yes)
    {
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdCloseAllVideo);
        Indicator::showLoading(true, nullptr);
        QTimer::singleShot(3000, this, [=]()
            {
                Indicator::showLoading(false, nullptr);

            });
        Indicator::showTopTip(QString::fromLocal8Bit("���ڹرգ����Ժ�.."), nullptr);

        QVariantMap param;
        param["type"] = 4;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        CmdHandlerMgr::Instance()->sendCmd("nop");
    }
}

void frmTVWall::on_btnConnectClicked()
{
    Indicator::showLoading(true, nullptr);
    TcpClient::Instance()->uninit();
    if (!TcpClient::Instance()->init())
    {
        Indicator::showLoading(false, nullptr);
        Indicator::showTopTip(QString::fromLocal8Bit("����ʧ�ܣ�����IP��ȷ���豸������"), nullptr);
        return;
    }
    else
    {
        Indicator::showTopTip(QString::fromLocal8Bit("���ӳɹ�,����ˢ�����Ժ�..."), nullptr);
        AppEvent::Instance()->slot_tcpConnected();
        //CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdUnlockDevice);
        QVariantMap param;

        param["type"] = 1;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        //CmdHandlerMgr::Instance()->sendCmd("nop");
        param["type"] = 4;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        //CmdHandlerMgr::Instance()->sendCmd("nop");
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdWallSet);
        //TcpClientHelper::sendSceneInfo();
        Indicator::showLoading(false, nullptr);
        return;
    }
}

void frmTVWall::on_btnSceneClicked()
{
    m_sceneWidget->setVisible(btnScene->isChecked());
}

