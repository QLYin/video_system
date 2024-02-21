#ifndef FRMWELCOME_H
#define FRMWELCOME_H

#include <QWidget>

namespace Ui {
class frmWelcome;
}

class frmWelcome : public QWidget
{
    Q_OBJECT

public:
    explicit frmWelcome(QWidget *parent = 0);
    ~frmWelcome();

private:
    Ui::frmWelcome *ui;

private slots:
    //初始化窗体数据
    void initForm();
};

#endif // FRMWELCOME_H
