#ifndef FRMDEMOVIDEOCHANGE_H
#define FRMDEMOVIDEOCHANGE_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmDemoVideoChange;
}

class frmDemoVideoChange : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoChange(QWidget *parent = 0);
    ~frmDemoVideoChange();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmDemoVideoChange *ui;

    //当前视频索引
    int currentIndex;
    //视频控件1
    VideoWidget *videoWidget1;
    //视频控件2
    VideoWidget *videoWidget2;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化控件
    void initVideo(VideoWidget *videoWidget);
    //播放成功
    void receivePlayStart(int time);

private slots:
    void on_btnChange_clicked();
};

#endif // FRMDEMOVIDEOCHANGE_H
