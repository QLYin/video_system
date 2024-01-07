#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <QWidget>

class QAbstractButton;

namespace Ui {
class frmConfig;
}

class frmConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfig(QWidget *parent = 0);
    ~frmConfig();

private:
    Ui::frmConfig *ui;

    //导航按钮+图标集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化子窗体
    void initWidget();
    //初始化导航按钮
    void initNav();
    //初始化导航按钮图标
    void initIcon();
    //导航按钮单击事件
    void buttonClicked();
};

#endif // FRMCONFIG_H
