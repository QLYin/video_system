#ifndef FRMDEMOHELPER_H
#define FRMDEMOHELPER_H

#include <QWidget>

namespace Ui {
class frmDemoHelper;
}

class frmDemoHelper : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoHelper(QWidget *parent = 0);
    ~frmDemoHelper();

private:
    Ui::frmDemoHelper *ui;

private slots:
    void on_btnShowInfo_clicked();
    void on_btnShowError_clicked();
    void on_btnShowQuestion_clicked();
    void on_btnShowInput_clicked();
    void on_btnShowDate_clicked();

    void on_btnShowTip_clicked();
    void on_btnHideTip_clicked();
    void on_btnShowSplash_clicked();
    void on_btnShowAbout_clicked();
};

#endif // FRMDEMOHELPER_H
