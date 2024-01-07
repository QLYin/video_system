#ifndef FRMVIDEOPLAYRTSP_H
#define FRMVIDEOPLAYRTSP_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmVideoPlayRtsp;
}

class frmVideoPlayRtsp : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPlayRtsp(QWidget *parent = 0);
    ~frmVideoPlayRtsp();

    QList<VideoWidget *> getVideoWidgets();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmVideoPlayRtsp *ui;

    //最大化通道
    bool videoMax;
    //通道数量
    int videoCount;
    //当前通道索引
    int videoIndex;
    //视频控件集合
    QList<VideoWidget *> videoWidgets;
    //视频面板集合
    QList<QWidget *> widgets;

private slots:
    //初始化窗体数据
    void initForm();
    //加载配置文件
    void initConfig();
    //保存配置文件
    void saveConfig();

    //初始化按钮图标
    void initIcon();    
    //初始化视频通道
    void initVideo();
    //填充视频流地址
    void setUrl();

    //进度发生切换
    void positionChanged(int value);

private slots:
    void on_btnPlay_clicked();
    void on_btnClose_clicked();
};

#endif // FRMVIDEOPLAYRTSP_H
