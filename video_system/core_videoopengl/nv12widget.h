#ifndef NV12WIDGET_H
#define NV12WIDGET_H

#include <QWidget>

class Nv12Widget: public QWidget
{
    Q_OBJECT
public:
    explicit Nv12Widget(QWidget *parent = 0);
    ~Nv12Widget();

public slots:
    //清空数据
    void clear();
    //设置图片尺寸
    void setFrameSize(int width, int height);
    //更新纹理数据
    void updateTextures(quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV);
    //统一一个函数
    void updateFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV);

public slots:
    //播放本地文件
    void play(const QString &fileName, int frameRate);
    //停止播放
    void stop();

signals:
    //播放文件结束
    void playFinsh();
};

#endif // NV12WIDGET_H
