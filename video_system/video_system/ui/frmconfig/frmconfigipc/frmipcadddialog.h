#ifndef FRMIPCADDDIALOG_H
#define FRMIPCADDDIALOG_H

#include <QDialog>

namespace Ui {
class frmIpcAddDialog;
}

class frmIpcAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit frmIpcAddDialog(QWidget *parent = 0);
    ~frmIpcAddDialog();

private:
    Ui::frmIpcAddDialog* ui;
    QList<QStringList> deviceInfos;

signals:
    //发出批量添加的设备信号
    void addDevices(const QList<QStringList>& deviceInfos);

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
    void onBtnAddIpcClicked();
};

#endif // FRMIPCADDDIALOG_H
