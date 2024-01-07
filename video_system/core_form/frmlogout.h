#ifndef FRMLOGOUT_H
#define FRMLOGOUT_H

#include <QDialog>
#include "singleton.h"

namespace Ui {
class frmLogout;
}

class frmLogout : public QDialog
{
    Q_OBJECT SINGLETON_DECL(frmLogout)

public:
    explicit frmLogout(QWidget *parent = 0);
    ~frmLogout();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmLogout *ui;

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

private slots:
    void on_btnLogout_clicked();
};

#endif // FRMLOGOUT_H
