#ifndef FRMDEMO_H
#define FRMDEMO_H

#include <QWidget>
class QAbstractButton;

namespace Ui {
class frmDemo;
}

class frmDemo : public QWidget
{
    Q_OBJECT

public:    
    explicit frmDemo(QWidget *parent = 0);
    ~frmDemo();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::frmDemo *ui;

    //左侧导航栏图标和按钮集合
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

#endif // FRMDEMO_H
