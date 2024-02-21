#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QWidget>

class YuvWidget: public QWidget
{
    Q_OBJECT
public:
    explicit YuvWidget(QWidget *parent = 0);
    ~YuvWidget();

public slots:
    //设置是否yuyv422格式
    void setYuyv(bool yuyv);
    //清空数据
    void clear();
    //设置图片尺寸
    void setFrameSize(int width, int height);
    //更新纹理数据
    void updateTextures(quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);
    //统一一个函数
    void updateFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);

public slots:
    //播放本地文件
    void play(const QString &fileName, int frameRate);
    //停止播放
    void stop();

signals:
    //播放文件结束
    void playFinsh();
};

#endif // YUVWIDGET_H
