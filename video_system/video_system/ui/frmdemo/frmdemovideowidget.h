#ifndef FRMDEMOVIDEOWIDGET_H
#define FRMDEMOVIDEOWIDGET_H

#include <QWidget>

namespace Ui {
class frmDemoVideoWidget;
}

class frmDemoVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoWidget(QWidget *parent = 0);
    ~frmDemoVideoWidget();

private:
    Ui::frmDemoVideoWidget *ui;

    //截图定时器
    QTimer *timerSnap;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();
    void on_btnSnap_clicked();
    void on_ckAutoSnap_stateChanged(int arg1);
};

#endif // FRMDEMOVIDEOWIDGET_H
