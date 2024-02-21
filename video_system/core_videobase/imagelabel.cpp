#include "imagelabel.h"
#include "qpainter.h"
#include "qdebug.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{

}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    if (image.isNull()) {
        return;
    }

    QPainter painter(this);
#if 0
    painter.drawImage(0, 0, image);
#else
    //按照比例自动居中绘制
    int pixX = rect().center().x() - image.width() / 2;
    int pixY = rect().center().y() - image.height() / 2;
    QPoint point(pixX, pixY);
    painter.drawImage(point, image);
#endif
}

void ImageLabel::setImage(const QImage &image, bool scale)
{
    //QImage默认会浅拷贝,采用copy则深拷贝,不然图像快了会产生断层,开启copy会增加CPU
    this->image = image;
    //this->image = image.copy();

    //超过大小才需要缩放
    int w1 = this->width();
    int h1 = this->height();
    int w2 = image.width();
    int h2 = image.height();
    if (scale && (w2 > w1 || h2 > h1)) {
        this->image = this->image.scaled(this->size() - QSize(4, 4), Qt::KeepAspectRatio);
    }

    this->update();
}
