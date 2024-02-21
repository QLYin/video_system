#ifndef FRMVIDEOPLAYIMAGE_H
#define FRMVIDEOPLAYIMAGE_H

#include <QWidget>
class VideoWidget;
class QListWidgetItem;

namespace Ui {
class frmVideoPlayImage;
}

class frmVideoPlayImage : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPlayImage(QWidget *parent = 0);
    ~frmVideoPlayImage();

    //获取视频控件用于统一设置属性
    VideoWidget *getVideoWidget();

private:
    Ui::frmVideoPlayImage *ui;

    //是否暂停
    bool isPause;
    //是否停止
    bool isStop;
    //图标大小
    int iconSize;

    //视频控件
    VideoWidget *videoWidget;
    //播放定时器
    QTimer *timerPlay;
    //图片队列索引
    int listIndex;
    //图片文件队列
    QStringList listFile;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化视频控件
    void initVideo();
    //挨个加载图片文件
    void loadImage();

    //添加元素到列表框
    void addItem(const QString &text, const QString &data);

private slots:
    void on_btnSelect_clicked();
    QStringList getSelectFile(bool checked);
    void on_btnDownload_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    //获取报告内容html格式
    QString getHtml();
    void dataout(quint8 type);
    void on_btnPdf_clicked();
    void on_btnPrint_clicked();

    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnMute_clicked();

    void on_listWidget_doubleClicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);

    void on_sliderPosition_clicked();
    void on_sliderPosition_sliderMoved(int value);
    void on_cboxSpeed_currentIndexChanged(int index);

signals:
    void receiveImage(const QImage &image);
};

#endif // FRMVIDEOPLAYIMAGE_H
