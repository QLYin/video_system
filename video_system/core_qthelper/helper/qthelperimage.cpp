#include "qthelperimage.h"
#include "iconhelper.h"
#include "qthelpercore.h"

QPixmap QtHelperImage::getPixmap(QWidget *widget, const QPixmap &pixmap, bool scale)
{
    if (pixmap.isNull()) {
        return pixmap;
    }

    //scale=false 超过尺寸才需要等比缩放
    QPixmap pix = pixmap;
    if (scale) {
        pix = pix.scaled(widget->size() - QSize(2, 2), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else if (pix.width() > widget->width() || pix.height() > widget->height()) {
        pix = pix.scaled(widget->size() - QSize(2, 2), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return pix;
}

void QtHelperImage::setPixmap(QLabel *label, const QString &file, bool scale)
{
    //文件不存在则不用处理
    if (!QFile(file).exists()) {
        label->clear();
        return;
    }

    QPixmap pixmap(file);
    pixmap = getPixmap(label, pixmap);
    label->setPixmap(pixmap);
}

void QtHelperImage::setLogo(QLabel *label, const QString &file,
                             int width, int height, int offset,
                             const QString &oldColor, const QString &newColor)
{
    //如果是icon开头则表示图形字体否则取logo图片
    if (file.startsWith("icon")) {
        //设置图形字体作为logo
        height = 55, width = 80;
        QString text = file.split("_").last();
        int icon = text.toInt(NULL, 16);
        IconHelper::setIcon(label, icon, height);
    } else {
        //区分资源文件和本地文件
        QString fileName = file;
        if (!file.startsWith(":/")) {
            //height = 50;
            fileName = QString("%1/logo/%2.png").arg(QtHelperCore::appPath()).arg(file);
        }

        //svg图片自动换颜色
        if (file.endsWith(".svg")) {
            //打开文件对指定颜色进行替换
            QFile f(file);
            if (f.open(QFile::ReadOnly)) {
                QString text = f.readAll();
                text.replace(oldColor, newColor);
                f.close();

                //目录不存在则新建
                QtHelperCore::checkPath("logo");
                //打开新的文件输出
                fileName = QString("%1/logo/temp.svg").arg(QtHelperCore::appPath());
                QFile f2(fileName);
                if (f2.open(QFile::WriteOnly)) {
                    f2.write(text.toUtf8());
                    f2.close();
                }
            }
        }

        //文件不存在直接返回
        if (!QFile(fileName).exists()) {
            return;
        }

        //自动计算宽度比例
        QPixmap pixmap(fileName);
        if (pixmap.width() > 0 && pixmap.height() > 0) {
            if (pixmap.height() >= height) {
                width = (float)pixmap.width() / ((float)pixmap.height() / height);
            }
            pixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            label->setPixmap(pixmap);
        }
    }

    //设置logo标签最小宽度
    if (width - height < offset) {
        width += offset;
    }

    label->setFixedWidth(width);
    label->setAlignment(Qt::AlignCenter);
}

QRect QtHelperImage::getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth, int scaleMode)
{
    QSize newSize = imageSize;
    QSize widgetSize = widgetRect.size() - QSize(borderWidth * 1, borderWidth * 1);

    if (scaleMode == 0) {
        if (newSize.width() > widgetSize.width() || newSize.height() > widgetSize.height()) {
            newSize.scale(widgetSize, Qt::KeepAspectRatio);
        }
    } else if (scaleMode == 1) {
        newSize.scale(widgetSize, Qt::KeepAspectRatio);
    } else {
        newSize = widgetSize;
    }

    int x = widgetRect.center().x() - newSize.width() / 2;
    int y = widgetRect.center().y() - newSize.height() / 2;
    //不是2的倍数需要偏移1像素
    x += (x % 2 == 0 ? 1 : 0);
    y += (y % 2 == 0 ? 1 : 0);
    return QRect(x, y, newSize.width(), newSize.height());
}

void QtHelperImage::getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode, bool fast)
{
    Qt::TransformationMode mode = fast ? Qt::FastTransformation : Qt::SmoothTransformation;
    if (scaleMode == 0) {
        if (image.width() > widgetSize.width() || image.height() > widgetSize.height()) {
            image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
        }
    } else if (scaleMode == 1) {
        image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
    } else {
        image = image.scaled(widgetSize, Qt::IgnoreAspectRatio, mode);
    }
}
