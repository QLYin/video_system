#ifndef FRMCONFIGOTHER_H
#define FRMCONFIGOTHER_H

#include <QWidget>

namespace Ui {
class frmConfigOther;
}

class frmConfigOther : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigOther(QWidget *parent = 0);
    ~frmConfigOther();

private:
    Ui::frmConfigOther *ui;

private slots:
    void initForm();
    //加载串口配置参数
    void initComConfig();
    //保存串口配置参数
    void saveComConfig();

    //加载网络配置参数
    void initNetConfig();
    //保存网络配置参数
    void saveNetConfig();

private slots:
    void on_btnComImage_clicked();
    void on_btnInfo_clicked();
};

#endif // FRMCONFIGOTHER_H
