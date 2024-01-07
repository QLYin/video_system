#ifndef FRMVIDEO_H
#define FRMVIDEO_H

#include <QWidget>

class QAbstractButton;
class frmVideoPlayLocal;
class frmVideoPlayWeb;
class frmVideoPlayRtsp;
class frmVideoPlayImage;

namespace Ui {
class frmVideo;
}

class frmVideo : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideo(QWidget *parent = 0);
    ~frmVideo();

private:
    Ui::frmVideo *ui;

    //导航按钮+图标集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

    //单独拿出来用于批量设置对应的视频控件属性
    frmVideoPlayLocal *videoPlayLocal;
    frmVideoPlayWeb *videoPlayWeb;
    frmVideoPlayRtsp *videoPlayRtsp;
    frmVideoPlayImage *videoPlayImage;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化子窗体
    void initWidget();
    //初始化导航按钮
    void initNav();
    //初始化导航按钮图标
    void initIcon();
    //导航按钮单击事件
    void buttonClicked();

    //改变视频配置参数
    void changeVideoConfig();
};

#endif // FRMVIDEO_H
