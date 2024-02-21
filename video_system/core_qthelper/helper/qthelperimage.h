#ifndef QTHELPERIMAGE_H
#define QTHELPERIMAGE_H

#include <QPixmap>
class QLabel;
class QWidget;

class QtHelperImage
{
public:
    //获取等比缩放过的图片
    static QPixmap getPixmap(QWidget *widget, const QPixmap &pixmap, bool scale = false);
    static void setPixmap(QLabel *label, const QString &file, bool scale = false);
    //设置logo图片支持 资源文件+本地图片+图形字体+svg自动变色 等多种形式
    static void setLogo(QLabel *label, const QString &file, int width, int height, int offset = 20,
                        const QString &oldColor = QString(), const QString &newColor = QString());

    //传入图片尺寸和窗体区域及边框大小返回居中区域(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static QRect getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth = 2, int scaleMode = 0);
    //传入图片尺寸和窗体尺寸及缩放策略返回合适尺寸(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static void getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode = 0, bool fast = true);
};

#endif // QTHELPERIMAGE_H
