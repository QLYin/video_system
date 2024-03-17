#ifndef FRMIPCEDITDIALOG_H
#define FRMIPCEDITDIALOG_H

#include <QDialog>

namespace Ui {
class frmIpcEditDialog;
}

class frmIpcEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit frmIpcEditDialog(QWidget *parent = 0);
    ~frmIpcEditDialog();

signals:
    //发出批量添加的设备信号
    void editDevice(int id , const QStringList& deviceInfo);

private:
    Ui::frmIpcEditDialog* ui;
    int m_id;

public:
    void initData(QStringList data);

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
    void onConfirmClicked();
};

#endif // FRMIPCEDITDIALOG_H
