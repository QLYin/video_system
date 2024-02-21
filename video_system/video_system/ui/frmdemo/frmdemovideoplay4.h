#ifndef FRMDEMOVIDEOPLAY4_H
#define FRMDEMOVIDEOPLAY4_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmDemoVideoPlay4;
}

class frmDemoVideoPlay4 : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoPlay4(QWidget *parent = 0);
    ~frmDemoVideoPlay4();

private:
    Ui::frmDemoVideoPlay4 *ui;

    //视频控件集合
    QList<VideoWidget *> videoWidgets;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    //文件时长
    void receiveDuration(qint64 duration);
    //播放时长
    void receivePosition(qint64 position);

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();
    void on_sliderPosition_clicked();
    void on_sliderPosition_sliderMoved(int value);
};

#endif // FRMDEMOVIDEOPLAY4_H
