#ifndef RGBWIDGET_H
#define RGBWIDGET_H

#include <QWidget>

class RgbWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RgbWidget(QWidget *parent = 0);
    ~RgbWidget();

public slots:
    //清空数据
    void clear();
    //设置图片尺寸
    void setFrameSize(int width, int height);
    //更新纹理数据
    void updateTextures(quint8 *dataRGB, int type);
    //统一一个函数
    void updateFrame(int width, int height, quint8 *dataRGB, int type);

public slots:
    //显示图片
    void setImage(const QImage &image);
    //播放本地文件
    void play(const QString &fileName, int frameRate);
    //停止播放
    void stop();

signals:
    //播放文件结束
    void playFinsh();
};

#endif // RGBWIDGET_H
