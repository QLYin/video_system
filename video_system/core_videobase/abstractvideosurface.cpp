#include "abstractvideosurface.h"
#include "qvideosurfaceformat.h"
#include "qdatetime.h"
#include "qdebug.h"

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif

AbstractVideoSurface::AbstractVideoSurface(QObject *parent) : QAbstractVideoSurface(parent)
{
    type = 1;
    formatWidth = 0;
    formatHeight = 0;
    imageFormat = QImage::Format_RGB32;
}

QList<QVideoFrame::PixelFormat> AbstractVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    QList<QVideoFrame::PixelFormat> formats;
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        formats << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    }
    return formats;
}

bool AbstractVideoSurface::start(const QVideoSurfaceFormat &format)
{
    QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();
    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        if (size.width() != formatWidth || size.height() != formatHeight) {
            this->formatWidth = format.frameWidth();
            this->formatHeight = format.frameHeight();
            this->imageFormat = imageFormat;
            emit resolutionChanged(formatWidth, formatHeight);
        }

        QAbstractVideoSurface::start(format);
        return true;
    } else {
        return false;
    }
}

void AbstractVideoSurface::stop()
{
    QAbstractVideoSurface::stop();
}

bool AbstractVideoSurface::present(const QVideoFrame &frame)
{
    if (surfaceFormat().pixelFormat() != frame.pixelFormat() || surfaceFormat().frameSize() != frame.size()) {
        this->setError(IncorrectFormatError);
        this->stop();
        return false;
    } else {
        QVideoFrame videoFrame = frame;
        if (videoFrame.map(QAbstractVideoBuffer::ReadOnly)) {
            //Qt5.15新增了image方法直接转换
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            image = videoFrame.image();
#else
            image = QImage(videoFrame.bits(), videoFrame.width(), videoFrame.height(), videoFrame.bytesPerLine(), imageFormat);
#endif
            //如果方向是从下到上则需要镜像翻转
            if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
                image = image.mirrored();
            }

            if (type == 1) {
                //大分辨率要拷贝才行(否则容易图像出问题)
                emit receiveImage(image.copy(), 0);
            } else if (type == 2) {
                if (image.format() != QImage::Format_RGB888) {
                    image = image.convertToFormat(QImage::Format_RGB888);
                }
                emit receiveRgb(image.width(), image.height(), image.bits(), 1);
                //emit receiveRgb(videoFrame.width(), videoFrame.height(), videoFrame.bits(), 2);
            }

            videoFrame.unmap();
        }
        return true;
    }
}

void AbstractVideoSurface::setType(int type)
{
    this->type = type;
}

int AbstractVideoSurface::getWidth() const
{
    return this->formatWidth;
}

int AbstractVideoSurface::getHeight() const
{
    return this->formatHeight;
}

QSize AbstractVideoSurface::sizeHint() const
{
    return QSize(formatWidth, formatHeight);
}

QImage AbstractVideoSurface::getImage() const
{
    return this->image;
}
