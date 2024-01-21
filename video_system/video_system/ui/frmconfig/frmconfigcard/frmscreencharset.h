#ifndef FRMSCREENCHARSET_H
#define FRMSCREENCHARSET_H

#include <QDialog>

namespace Ui {
class frmScreenCharSet;
}

class frmScreenCharSet : public QDialog
{
    Q_OBJECT

public:
    explicit frmScreenCharSet(QWidget *parent = 0);
    ~frmScreenCharSet();

private:
    Ui::frmScreenCharSet*ui;

public:

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

#endif // FRMSCREENCHARSET_H
