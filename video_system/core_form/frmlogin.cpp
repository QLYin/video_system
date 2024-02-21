#include "frmlogin.h"
#include "ui_frmlogin.h"
#include "qthelper.h"
#include "dbquery.h"
#include "frmmain.h"
#include "class/appmisc/appmisc.h"
#include "../deviceconnect/tcpclient.h"
#include "../deviceconnect/cmdhandlermgr.h"
#include "ui/frmbase/Indicator.h"

frmLogin::frmLogin(QWidget *parent) : QDialog(parent), ui(new Ui::frmLogin)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmLogin::~frmLogin()
{
    TcpClient::Instance()->uninit();
    delete ui;
}

void frmLogin::setBanner(const QString &image)
{
    QString qss = QString("border-image:url(%1);").arg(image);
    ui->labName->setStyleSheet(qss);
}

void frmLogin::initStyle()
{
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(close()));
    this->setBanner(":/image/bg_banner.jpg");
}

void frmLogin::initTitle()
{
    //设置标题并支持标题换行
    QString titleCn = AppConfig::TitleCn;
    if (titleCn.contains("\\n")) {
        titleCn.replace("\\n", "\n");
    }

    ui->labName->setText(titleCn);
    this->setWindowTitle(ui->labTitle->text());
}

void frmLogin::initForm()
{
    //初始化IP
    ui->lineEditIP->setText(AppConfig::DeviceIP);
    //将对应用户的密码和类型存入链表,在用户登录时对比,比从数据库中查询速度要快.
    ui->cboxUserName->addItems(UserHelper::UserInfo_UserName);
    //设置最后登录的用户
    int index = ui->cboxUserName->findText(AppConfig::LastLoginer);
    ui->cboxUserName->setCurrentIndex(index);

    ui->txtUserPwd->setPlaceholderText("请输入密码");
    ui->txtUserPwd->setFocus();
    //设置按钮默认回车键触发
    ui->btnLogin->setDefault(true);

    //自动填入密码
    if (AppConfig::AutoPwd) {
        ui->txtUserPwd->setText(UserHelper::UserInfo_UserPwd.at(index));
    }

    //设置是否否选自动登录+记住密码
    ui->ckAutoPwd->setChecked(AppConfig::AutoPwd);
    ui->ckAutoLogin->setChecked(AppConfig::AutoLogin);

    //样式改变更新按钮图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmLogin::initIcon()
{
    //图片文件不存在则设置为图形字体
    QtHelper::setIconBtn(ui->btnLogin, ":/image/btn_ok.png", 0xf00c);
    QtHelper::setIconBtn(ui->btnClose, ":/image/btn_close.png", 0xf00d);
}

void frmLogin::on_btnLogin_clicked()
{
    //获取填入的密码,不区分大小写,全部转为大小比较
    QString userPwd = ui->txtUserPwd->text();
    userPwd = userPwd.toUpper();
    if (userPwd.isEmpty()) {
        QtHelper::showMessageBoxError("密码不能为空, 请重新输入!", 3, true);
        ui->txtUserPwd->setFocus();
        return;
    }

    //密码正确或者是超级密码则表示成功
    int index = ui->cboxUserName->currentIndex();
    if ((userPwd == UserHelper::UserInfo_UserPwd.at(index).toUpper()) || userPwd == "A") {
        UserHelper::CurrentUserName = ui->cboxUserName->currentText();
        UserHelper::getUserInfo();
        DbQuery::addUserLog("用户登录");

        //记录当前用户,写入到配置文件,下次启动时显示最后一次登录用户名
        AppConfig::AutoPwd = ui->ckAutoPwd->isChecked();
        AppConfig::AutoLogin = ui->ckAutoLogin->isChecked();
        AppConfig::LastLoginer = UserHelper::CurrentUserName;
        AppConfig::DeviceIP = ui->lineEditIP->text().trimmed();
        AppConfig::writeConfig();

        //如果是密码A则为超级管理员默认所有权限都有
        if (userPwd == "A") {
            int count = UserHelper::UserPermission.count();
            for (int i = 0; i < count; ++i) {
                UserHelper::UserPermission[i] = true;
            }
        }

        //隐藏当前界面弹出主界面
        this->initDeviceConnect();
        Indicator::showLoading(true, this);
        QTimer::singleShot(5000, this, [=]() 
            {
                if (!TcpClient::Instance()->isConnnected())
                {
                    Indicator::showLoading(false, this);
                    Indicator::showTopTip(QString::fromLocal8Bit("连接失败，请检查IP并确认设备已上线"), this);
                }
            });
    } else {
        //密码错误计数
        static int errorCount = 0;
        errorCount++;
        if (errorCount >= 3) {
            errorCount = 0;
            QtHelper::showMessageBoxError("密码输入错误次数超过三次, 系统将自动退出!", 5, true);
            exit(0);
        } else {
            QtHelper::showMessageBoxError("密码错误, 请重新输入!", 3, true);
            ui->txtUserPwd->setFocus();
        }
    }
}

void frmLogin::on_cboxUserName_activated(int)
{
    ui->txtUserPwd->clear();
    ui->txtUserPwd->setFocus();
}

void frmLogin::initDeviceConnect()
{
    TcpClient::Instance()->init();
    connect(TcpClient::Instance(), &TcpClient::socketConnected, this, [this]()
        {
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

            Indicator::showLoading(false, this);
            this->hide();
            frmMain* form = AppMisc::Instance()->mainWnd();
            form->show();
            form->activateWindow();
        });

    connect(TcpClient::Instance(), &TcpClient::socketError, this, [this](QAbstractSocket::SocketError err)
        {
            if (isVisible())
            {
                Indicator::showLoading(false, this);
                Indicator::showTopTip(QString::fromLocal8Bit("连接失败，请检查IP并确认设备已上线"), this);
            }
        });
}