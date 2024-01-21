#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QDialog>

namespace Ui {
class frmSearchCard;
}

class frmSearchCard : public QDialog
{
    Q_OBJECT

public:
    explicit frmSearchCard(QWidget *parent = 0);
    ~frmSearchCard();

private:
    Ui::frmSearchCard*ui;

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
};

#endif // FRMLOGIN_H
