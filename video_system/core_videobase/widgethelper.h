#ifndef WIDGETHELPER_H
#define WIDGETHELPER_H

#include "widgethead.h"
#include "abstractvideowidget.h"

class WidgetHelper
{
public:
    //获取当前屏幕区域
    static QList<QRect> getScreenRects();
    static QRect getScreenRect(int screenIndex = -1);
    //传入宽高输出分辨率字符串(矫正奇数)
    static QString getResolution(int width, int height);
    static QString getResolution(const QString &resolution);
    //根据屏幕区域校验参数
    static void checkRect(int screenIndex, QString &resolution, int &offsetX, int &offsetY);

    //根据视频尺寸获取合适的线条宽度(用于滤镜绘制矩形)
    static int getBorderWidth(AbstractVideoWidget *widget, int *size = NULL);
    //传入两个坐标返回矩形
    static QRect getRect(const QString &p1, const QString &p2);
    static QRect getRect(const QPoint &p1, const QPoint &p2);

    //传入图片尺寸和窗体区域及边框大小返回居中区域(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static QRect getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth = 2, const ScaleMode &scaleMode = ScaleMode_Auto);
    //传入图片尺寸和窗体尺寸及缩放策略返回合适尺寸(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static void getScaledImage(QImage &image, const QSize &widgetSize, const ScaleMode &scaleMode = ScaleMode_Auto, bool fast = true);

    //分辨率字符串分割 640x480/640*480
    static QStringList getSizes(const QString &size);
    //根据旋转度数交换宽高
    static void rotateSize(int rotate, int &width, int &height);
    //校验网络地址是否可达
    static bool checkUrl(const QString &url, int timeout = 500);

    //传入标签格式获取日期时间字符串
    static QString getTextByFormat(const OsdInfo &osd);
    //传入标签位置获取真实坐标
    static QPoint getPointByPosition(const OsdInfo &osd, const QRect &rect, int &flag);

    //绘制标签信息
    static void drawOsd(QPainter *painter, const OsdInfo &osd, const QRect &rect);
    //绘制矩形区域比如人脸框
    static void drawRect(QPainter *painter, const QRect &rect, int borderWidth = 3, QColor borderColor = Qt::red, bool angle = false);
    //绘制点集合多边形路径比如三角形
    static void drawPoints(QPainter *painter, const QList<QPoint> &pts, int borderWidth = 3, QColor borderColor = Qt::red);
    //绘制路径集合
    static void drawPath(QPainter *painter, QPainterPath path, int borderWidth = 3, QColor borderColor = Qt::red);

    //显示截图预览
    static QLabel *showImage(QLabel *label, QWidget *widget, const QImage &image);
    //演示添加标签
    static void addOsd(AbstractVideoWidget *widget, int &index);
    static void addOsd2(AbstractVideoWidget *widget, int count);
    //演示添加图形
    static void addGraph(AbstractVideoWidget *widget, int &index);
    static void addGraph2(AbstractVideoWidget *widget, int count);
};

#endif // WIDGETHELPER_H
