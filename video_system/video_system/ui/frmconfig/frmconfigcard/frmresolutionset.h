#ifndef FRMRESOLUTIONSET_H
#define FRMRESOLUTIONSET_H

#include <QDialog>

namespace Ui {
class frmResolutionSet;
}

class frmResolutionSet : public QDialog
{
    Q_OBJECT

public:
    explicit frmResolutionSet(QWidget *parent = 0);
    ~frmResolutionSet();

private:
    Ui::frmResolutionSet* ui;

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

#endif // FRMRESOLUTIONSET_H
