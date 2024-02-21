#ifndef FRMIPCNETCONFIG_H
#define FRMIPCNETCONFIG_H

#include <QWidget>

namespace Ui {
class frmIpcNetConfig;
}

class frmIpcNetConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmIpcNetConfig(QWidget *parent = 0);
    ~frmIpcNetConfig();

private:
    Ui::frmIpcNetConfig *ui;

private slots:
    //初始化窗体数据
    void initForm();
    void on_btnGetNetConfig_clicked();
    void on_btnSetNetConfig_clicked();
    void on_btnClearData_clicked();
};

#endif // FRMIPCNETCONFIG_H
