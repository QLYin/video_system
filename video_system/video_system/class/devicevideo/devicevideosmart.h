#ifndef DEVICEVIDEOSMART_H
#define DEVICEVIDEOSMART_H

#include <QThread>
#include <QRect>
class VideoWidget;

//人工智能处理类
class DeviceVideoSmart : public QThread
{
    Q_OBJECT
public:
    explicit DeviceVideoSmart(QObject *parent = 0);

protected:
    void run();

private:
    //线程停止+暂停标志位
    volatile bool stopped;
    volatile bool isPause;

    //字体大小
    int fontSize;
    //视频控件
    VideoWidget *videoWidget;

private slots:
    //视频实时码率
    void receiveKbps(qreal kbps, int frameRate);
    //播放成功
    void receivePlayStart(int time);

    //收到方框区域集合信号
    void receiveRects(const QList<QRect> &rects);
    //定义两个参数,这样就涵盖了所有的情况 type表示类型相当于唯一标识
    void receiveData(const QString &type, const QVariant &data);

public slots:
    //设置视频控件
    void setVideoWidget(VideoWidget *videoWidget);

    //停止或者暂停处理
    void stop();
    void pause();
};

#endif // DEVICEVIDEOSMART_H
