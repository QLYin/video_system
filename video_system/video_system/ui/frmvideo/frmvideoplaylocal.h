#ifndef FRMVIDEOPLAYLOCAL_H
#define FRMVIDEOPLAYLOCAL_H

#include <QWidget>
#include <QDateTime>
class VideoWidget;
class QListWidgetItem;

namespace Ui {
class frmVideoPlayLocal;
}

class frmVideoPlayLocal : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPlayLocal(QWidget *parent = 0);
    ~frmVideoPlayLocal();

    //获取视频控件用于统一设置属性
    VideoWidget *getVideoWidget();

private:
    Ui::frmVideoPlayLocal *ui;

    //是否停止
    bool isStop;
    //图标大小
    int iconSize;

    //最后双击的时间
    QDateTime doubleClickTime;
    //视频窗体控件
    VideoWidget *videoWidget;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化视频控件
    void initVideo();

    //添加元素到列表框
    void addItem(const QString &text, const QString &data);

private slots:
    //播放开始
    void receivePlayStart(int time);
    //播放结束
    void receivePlayFinsh();
    //文件时长
    void receiveDuration(qint64 duration);
    //播放时长
    void receivePosition(qint64 position);
    //接收到拖曳文件
    void fileDrag(const QString &url);

private slots:
    QStringList getSelectFile(bool checked);
    void on_btnSelect_clicked();    
    void on_btnDownload_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void on_btnSelectAll_clicked();
    void on_btnSelectNone_clicked();

    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnMute_clicked();

    void on_listWidget_doubleClicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);

    void on_sliderPosition_clicked();
    void on_sliderPosition_sliderMoved(int value);
    void on_cboxSpeed_currentIndexChanged(int index);
};

#endif // FRMVIDEOPLAYLOCAL_H
