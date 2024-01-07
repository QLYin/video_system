#ifndef FRMDEMOVIDEOVOLUME_H
#define FRMDEMOVIDEOVOLUME_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmDemoVideoVolume;
}

class frmDemoVideoVolume : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoVolume(QWidget *parent = 0);
    ~frmDemoVideoVolume();

private:
    Ui::frmDemoVideoVolume *ui;

    //视频控件
    VideoWidget *videoWidget;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();
};

#endif // FRMDEMOVIDEOVOLUME_H
