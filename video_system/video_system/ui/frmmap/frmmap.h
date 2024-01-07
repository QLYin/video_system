#ifndef FRMMAP_H
#define FRMMAP_H

#include <QWidget>

class QAbstractButton;

namespace Ui {
class frmMap;
}

class frmMap : public QWidget
{
    Q_OBJECT

public:
    explicit frmMap(QWidget *parent = 0);
    ~frmMap();

private:
    Ui::frmMap *ui;

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

#endif // FRMMAP_H
