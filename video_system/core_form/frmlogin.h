#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QDialog>

namespace Ui {
class frmLogin;
}

class frmLogin : public QDialog
{
    Q_OBJECT

public:
    explicit frmLogin(QWidget *parent = 0);
    ~frmLogin();

private:
    Ui::frmLogin *ui;

public:
    //设置顶部横幅背景图片
    void setBanner(const QString &image);

private slots:
    //初始化无边框窗体
    void initStyle();
    //初始化标题
    void initTitle();
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();

    void initDeviceConnect();

private slots:
    void on_btnLogin_clicked();
    void on_cboxUserName_activated(int);
};

#endif // FRMLOGIN_H
