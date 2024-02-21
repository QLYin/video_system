#ifndef ABSTRACTVIDEOSURFACE_H
#define ABSTRACTVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>

class AbstractVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit AbstractVideoSurface(QObject *parent = 0);

protected:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    //bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    bool present(const QVideoFrame &frame);

private:
    int type;
    int formatWidth;
    int formatHeight;
    QImage image;
    QImage::Format imageFormat;

public:
    void setType(int type);

    int getWidth()      const;
    int getHeight()     const;
    QSize sizeHint()    const;
    QImage getImage()   const;

signals:
    //收到一张图片
    void receiveImage(const QImage &image, int time);
    //收到图片数据
    void receiveRgb(int width, int height, quint8 *dataRGB, int type);
    //分辨率发生变化(一般视频流中途可能会改变了分辨率)
    void resolutionChanged(int width, int height);
};

#endif // ABSTRACTVIDEOSURFACE_H
