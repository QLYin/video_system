#ifndef FRMVIDEOPLAYWEB_H
#define FRMVIDEOPLAYWEB_H

#include <QWidget>
class VideoWidget;
class QListWidgetItem;

namespace Ui {
class frmVideoPlayWeb;
}

class frmVideoPlayWeb : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPlayWeb(QWidget *parent = 0);
    ~frmVideoPlayWeb();

    QList<VideoWidget *> getVideoWidgets();

protected:
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmVideoPlayWeb *ui;

    //最大化通道
    bool videoMax;
    //视频通道数量
    int videoCount;
    //视频控件集合
    QList<VideoWidget *> videoWidgets;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

    //初始化视频控件
    void initVideo();
    //初始化视频回放控件
    void initVideoCh();
    //播放视频
    void playVideo();

private slots:
    void on_btnSelect_clicked();
    void on_btnDownload_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void on_btnSelectAll_clicked();
    void on_btnSelectNone_clicked();

    void on_cboxNvr_currentIndexChanged(int);
    void on_listWidget_itemPressed(QListWidgetItem *item);
};

#endif // FRMVIDEOPLAYWEB_H
