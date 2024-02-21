#include "frmlogout.h"
#include "ui_frmlogout.h"
#include "qthelper.h"
#include "dbquery.h"

SINGLETON_IMPL(frmLogout)
frmLogout::frmLogout(QWidget *parent) : QDialog(parent), ui(new Ui::frmLogout)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmLogout::~frmLogout()
{
    delete ui;
}

void frmLogout::showEvent(QShowEvent *)
{
    //显示的时候密码清空并设置焦点
    ui->txtUserPwd->clear();
    ui->txtUserPwd->setFocus();
    //可能标题又改过所以需要重新设置
    this->initTitle();
}

void frmLogout::setBanner(const QString &image)
{
    QString qss = QString("border-image:url(%1);").arg(image);
    ui->labName->setStyleSheet(qss);
}

void frmLogout::initStyle()
{
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(hide()));
    this->setBanner(":/image/bg_banner.jpg");
}

void frmLogout::initTitle()
{
    //设置标题并支持标题换行
    QString titleCn = AppConfig::TitleCn;
    if (titleCn.contains("\\n")) {
        titleCn.replace("\\n", "\n");
    }

    ui->labName->setText(titleCn);
    this->setWindowTitle(ui->labTitle->text());
}

void frmLogout::initForm()
{
    //自动填入登录的用户名
    ui->txtUserName->setText(UserHelper::CurrentUserName);    
    ui->txtUserPwd->setPlaceholderText("请输入密码");
    ui->txtUserPwd->setFocus();
    //设置按钮默认回车键触发
    ui->btnLogout->setDefault(true);

    //样式改变更新按钮图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmLogout::initIcon()
{
    //图片文件不存在则设置为图形字体
    QtHelper::setIconBtn(ui->btnLogout, ":/image/btn_ok.png", 0xf00c);
    QtHelper::setIconBtn(ui->btnClose, ":/image/btn_close.png", 0xf00d);
}

void frmLogout::on_btnLogout_clicked()
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
    if ((userPwd == UserHelper::CurrentUserPwd.toUpper()) || userPwd == "A") {
        DbQuery::addUserLog("用户退出");
        //发出全局退出事件信号通知退出
        AppEvent::Instance()->slot_exitAll();
        exit(0);
    } else {
        //密码错误计数
        static int errorCount = 0;
        errorCount++;
        if (errorCount >= 3) {
            errorCount = 0;
            QtHelper::showMessageBoxError("密码输入错误次数超过三次, 将关闭用户退出窗体!", 5, true);
            this->close();
        } else {
            QtHelper::showMessageBoxError("密码错误, 请重新输入!", 3, true);
            ui->txtUserPwd->setFocus();
        }
    }
}
