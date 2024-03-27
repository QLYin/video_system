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
#include "frmdevicetree.h"
#include "frmwallsetdialog.h"
#include "class/appmisc/appmisc.h"
#include "class/devicemanager/tvwallmanager.h"
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
    btnReresh->setText(QString::fromLocal8Bit("刷新"));
    btnReresh->setFixedWidth(70);
    topLayout->addWidget(btnReresh);
    connect(btnReresh, &QPushButton::clicked, this, &frmTVWall::on_btnRereshClicked);

    btnCreateWall = new QPushButton(widgetTop);
    btnCreateWall->setObjectName(QString::fromUtf8("btnCreateWall"));
    btnCreateWall->setText(QString::fromLocal8Bit("创建幕墙"));
    btnCreateWall->setFixedWidth(70);
    topLayout->addWidget(btnCreateWall);
    connect(btnCreateWall, &QPushButton::clicked, this, &frmTVWall::on_btnCreateWallClicked);

    btnCallAll = new QPushButton(widgetTop);
    btnCallAll->setObjectName(QString::fromUtf8("btnCreateWall"));
    btnCallAll->setText(QString::fromLocal8Bit("全部调入"));
    btnCallAll->setFixedWidth(70);
    topLayout->addWidget(btnCallAll);
    connect(btnCallAll, &QPushButton::clicked, this, &frmTVWall::on_btnCallAllClicked);

    btnConnect = new QPushButton(widgetTop);
    btnConnect->setObjectName(QString::fromUtf8("btnCreateWall"));
    btnConnect->setText(QString::fromLocal8Bit("连接"));
    btnConnect->setFixedWidth(70);
    topLayout->addWidget(btnConnect);
    connect(btnConnect, &QPushButton::clicked, this, &frmTVWall::on_btnConnectClicked);
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
    m_tvWallWidget = new frmTVWallWidget(this);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(pTab);
    hLayout->addWidget(m_tvWallWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
    mainLayout->addWidget(widgetTop);
    mainLayout->addSpacing(2);
    mainLayout->addLayout(hLayout);

    TVWallManager::Instance()->initWallWidget(m_tvWallWidget, this);
    connect(TcpClient::Instance(), &TcpClient::socketDisconnect, this, [this]()
        {
            Indicator::showTopTip(QString::fromLocal8Bit("设备连接异常, 请重新连接"), nullptr);
        });
}

void frmTVWall::on_btnRereshClicked()
{
    QVariantMap param;
    param["type"] = 4;
    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
    CmdHandlerMgr::Instance()->sendCmd("nop");
    param["type"] = 1;
    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
    CmdHandlerMgr::Instance()->sendCmd("nop");
    Indicator::showLoading(true, nullptr);
    QTimer::singleShot(3000, this, [=]()
        {
            Indicator::showLoading(false, nullptr);

        });
    Indicator::showTopTip(QString::fromLocal8Bit("正在刷新，请稍后.."), nullptr);

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
    if (QtHelper::showMessageBoxQuestion("确定要全部调入") == QMessageBox::Yes)
    {
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdCallAllVideo);
        Indicator::showLoading(true, nullptr);
        QTimer::singleShot(3000, this, [=]()
            {
                Indicator::showLoading(false, nullptr);

            });
        Indicator::showTopTip(QString::fromLocal8Bit("正在调入，请稍后.."), nullptr);
  
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
        Indicator::showTopTip(QString::fromLocal8Bit("连接失败，请检查IP并确认设备已上线"), nullptr);
        return;
    }
    else
    {
        Indicator::showTopTip(QString::fromLocal8Bit("连接成功,正在刷新请稍后..."), nullptr);
        AppEvent::Instance()->slot_tcpConnected();
        //CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdUnlockDevice);
        QVariantMap param;

        param["type"] = 4;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        CmdHandlerMgr::Instance()->sendCmd("nop");
        param["type"] = 1;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
        CmdHandlerMgr::Instance()->sendCmd("nop");
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdWallSet);
        //TcpClientHelper::sendSceneInfo();
        Indicator::showLoading(false, nullptr);
        return;
    }
}

