#ifndef FRMDEMOVIDEOPLUS_H
#define FRMDEMOVIDEOPLUS_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmDemoVideoPlus;
}

class frmDemoVideoPlus : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoPlus(QWidget *parent = 0);
    ~frmDemoVideoPlus();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmDemoVideoPlus *ui;

    //视频控件1
    VideoWidget *videoWidget1;
    //视频控件2
    VideoWidget *videoWidget2;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();
};

#endif // FRMDEMOVIDEOPLUS_H
