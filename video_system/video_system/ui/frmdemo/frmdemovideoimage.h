#ifndef FRMDEMOVIDEOIMAGE_H
#define FRMDEMOVIDEOIMAGE_H

#include <QWidget>

namespace Ui {
class frmDemoVideoImage;
}

class frmDemoVideoImage : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoImage(QWidget *parent = 0);
    ~frmDemoVideoImage();

private:
    Ui::frmDemoVideoImage *ui;

    //图片队列索引
    int listIndex;
    //图片文件队列
    QStringList listFile;
    //播放定时器
    QTimer *timerPlay;

private slots:
    //初始化窗体数据
    void initForm();
    //挨个加载图片文件
    void loadImage();

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();

signals:
    void receiveImage(const QImage &image);
};

#endif // FRMDEMOVIDEOIMAGE_H
