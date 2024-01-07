#include "widgethelper.h"
#include "qtcpsocket.h"
#include "urlhelper.h"

QList<QRect> WidgetHelper::getScreenRects()
{
    QList<QRect> rects;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    int screenCount = qApp->screens().count();
    QList<QScreen *> screens = qApp->screens();
    for (int i = 0; i < screenCount; ++i) {
        QScreen *screen = screens.at(i);
        rects << screen->geometry();
    }
#else
    int screenCount = qApp->desktop()->screenCount();
    QDesktopWidget *desk = qApp->desktop();
    for (int i = 0; i < screenCount; ++i) {
        rects << desk->screenGeometry(i);
    }
#endif
    return rects;
}

QRect WidgetHelper::getScreenRect(int screenIndex)
{
    //指定了屏幕索引则取指定的(没有指定则取当前鼠标所在屏幕)
    QList<QRect> rects = WidgetHelper::getScreenRects();
    if (screenIndex >= 0 && screenIndex < rects.count()) {
        return rects.at(screenIndex);
    } else {
        //当前屏幕区域包含当前鼠标所在坐标则说明是当前屏幕
        QPoint pos = QCursor::pos();
        foreach (QRect rect, rects) {
            if (rect.contains(pos)) {
                return rect;
            }
        }
    }
}

QString WidgetHelper::getResolution(int width, int height)
{
    //取偶数(虚拟机中很可能是奇数的分辨率)
    if (width % 2 != 0) {
        width--;
    }

    if (height % 2 != 0) {
        height--;
    }

    return QString("%1x%2").arg(width).arg(height);
}

QString WidgetHelper::getResolution(const QString &resolution)
{
    QStringList sizes = WidgetHelper::getSizes(resolution);
    return getResolution(sizes.at(0).toInt(), sizes.at(1).toInt());
}

void WidgetHelper::checkRect(int screenIndex, QString &resolution, int &offsetX, int &offsetY)
{
    QRect rect = WidgetHelper::getScreenRect(screenIndex);
    if (resolution == "0x0") {
        resolution = WidgetHelper::getResolution(rect.width(), rect.height());
    } else {
        resolution = WidgetHelper::getResolution(resolution);
    }

    //偏移值必须小于分辨率否则重置
    if (offsetX > rect.width()) {
        offsetX = 0;
    }
    if (offsetY > rect.height()) {
        offsetY = 0;
    }

    //判断设定的偏移值加上设定的分辨率是否超出了真实的分辨率
    QStringList sizes = WidgetHelper::getSizes(resolution);
    int width = sizes.at(0).toInt();
    int height = sizes.at(1).toInt();

    if (offsetX + width > rect.width()) {
        width = rect.width() - offsetX;
    }
    if (offsetY + height > rect.height()) {
        height = rect.height() - offsetY;
    }

    //如果超出了分辨率则重新设置采集的分辨率
    resolution = WidgetHelper::getResolution(width, height);

    //多个屏幕需要加上屏幕起始坐标
    if (offsetX == 0) {
        offsetX = rect.x();
    } else {
        offsetX += rect.x();
    }
    if (offsetY == 0) {
        offsetY = rect.y();
    } else {
        offsetY += rect.y();
    }

    //qDebug() << TIMEMS << screenIndex << offsetX << offsetY << resolution;
}

int WidgetHelper::getBorderWidth(AbstractVideoWidget *widget, int *size)
{
    //根据画面尺寸主动加大边框
    int offset = widget->getVideoWidth() / 35;
    if (size) {
        *size = offset;
    }

    int borderWidth = offset / 10;
    borderWidth = (borderWidth < 1 ? 1 : borderWidth);
    return borderWidth;
}

QRect WidgetHelper::getRect(const QString &p1, const QString &p2)
{
    QStringList list1 = p1.split(",");
    QStringList list2 = p2.split(",");
    QPoint pos1(list1.at(0).toInt(), list1.at(1).toInt());
    QPoint pos2(list2.at(0).toInt(), list2.at(1).toInt());
    return WidgetHelper::getRect(pos1, pos2);
}

QRect WidgetHelper::getRect(const QPoint &p1, const QPoint &p2)
{
    //qrect参数只支持左上角右下角坐标点参数传入/要先换算好
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    QPoint topleft = QPoint(qMin(x1, x2), qMin(y1, y2));
    QPoint bottomright = QPoint(qMax(x1, x2), qMax(y1, y2));
    return QRect(topleft, bottomright);
}

QRect WidgetHelper::getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth, const ScaleMode &scaleMode)
{
    QSize newSize = imageSize;
    QSize widgetSize = widgetRect.size() - QSize(borderWidth * 1, borderWidth * 1);

    if (scaleMode == ScaleMode_Auto) {
        if (newSize.width() > widgetSize.width() || newSize.height() > widgetSize.height()) {
            newSize.scale(widgetSize, Qt::KeepAspectRatio);
        }
    } else if (scaleMode == ScaleMode_Aspect) {
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

void WidgetHelper::getScaledImage(QImage &image, const QSize &widgetSize, const ScaleMode &scaleMode, bool fast)
{
    Qt::TransformationMode mode = fast ? Qt::FastTransformation : Qt::SmoothTransformation;
    if (scaleMode == ScaleMode_Auto) {
        if (image.width() > widgetSize.width() || image.height() > widgetSize.height()) {
            image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
        }
    } else if (scaleMode == ScaleMode_Aspect) {
        image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
    } else {
        image = image.scaled(widgetSize, Qt::IgnoreAspectRatio, mode);
    }
}

QStringList WidgetHelper::getSizes(const QString &size)
{
    QStringList sizes;
    if (size.contains("*")) {
        sizes = size.split("*");
    } else if (size.contains("x")) {
        sizes = size.split("x");
    }
    return sizes;
}

void WidgetHelper::rotateSize(int rotate, int &width, int &height)
{
    int w = width;
    int h = height;
    if (rotate == 90 || rotate == 270) {
        width = h;
        height = w;
    }
}

bool WidgetHelper::checkUrl(const QString &url, int timeout)
{
    //没有超时时间则认为永远返回真
    if (timeout <= 0) {
        return true;
    }

    //找出IP地址和端口号
    QString ip = UrlHelper::getUrlIP(url);
    int port = UrlHelper::getUrlPort(url);
    //取到了IP地址则判断IP地址(否则是网址暂时不判断)
    if (ip == url) {
        return true;
    }

    //局部的事件循环不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    QObject::connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(ip, port);
    eventLoop.exec();

    //超时没有连接上则判断该设备不在线
    if (tcpSocket.state() != QAbstractSocket::ConnectedState) {
        //qDebug() << TIMEMS << QString("连接失败 -> 地址: %1").arg(videoUrl);;
        return false;
    }

    return true;
}

QString WidgetHelper::getTextByFormat(const OsdInfo &osd)
{
    QString text = osd.text;
    QDateTime now = QDateTime::currentDateTime();
    if (osd.format == OsdFormat_Date) {
        text = now.toString("yyyy-MM-dd");
    } else if (osd.format == OsdFormat_Time) {
        text = now.toString("HH:mm:ss");
    } else if (osd.format == OsdFormat_DateTime) {
        text = now.toString("yyyy-MM-dd HH:mm:ss");
    } else if (osd.format == OsdFormat_DateTimeMs) {
        text = now.toString("yyyy-MM-dd HH:mm:ss.zzz");
    }

    return text;
}

QPoint WidgetHelper::getPointByPosition(const OsdInfo &osd, const QRect &rect, int &flag)
{
    //如果存在图片则取图片的尺寸(否则取当前字体下的文字的尺寸)
    int width = 0;
    int height = 0;
    if (osd.position != OsdPosition_Custom) {
        if (osd.format == OsdFormat_Image) {
            width = osd.image.width();
            height = osd.image.height();
        } else {
            QFont font;
            font.setFamily(osd.fontName);
            font.setPixelSize(osd.fontSize);
            QFontMetrics fm(font);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
            width = fm.width(osd.text);
#else
            width = fm.horizontalAdvance(osd.text);
#endif
            height = fm.height();
        }
    }

    //默认位置在左上角
    flag = Qt::AlignLeft | Qt::AlignTop;
    QPoint point = QPoint(rect.x(), rect.y());
    if (osd.position == OsdPosition_LeftTop) {
        flag = Qt::AlignLeft | Qt::AlignTop;
        point = QPoint(rect.x(), rect.y());
    } else if (osd.position == OsdPosition_LeftBottom) {
        flag = Qt::AlignLeft | Qt::AlignBottom;
        point = QPoint(rect.x(), rect.height() - height);
    } else if (osd.position == OsdPosition_RightTop) {
        flag = Qt::AlignRight | Qt::AlignTop;
        point = QPoint(rect.width() - width, rect.y());
    } else if (osd.position == OsdPosition_RightBottom) {
        flag = Qt::AlignRight | Qt::AlignBottom;
        point = QPoint(rect.width() - width, rect.height() - height);
    } else if (osd.position == OsdPosition_Center) {
        flag = Qt::AlignVCenter | Qt::AlignHCenter;
        point = QPoint(rect.width() / 2 - width / 2, rect.height() / 2 - height / 2);
    } else if (osd.position == OsdPosition_Custom) {
        point = osd.point;
    }

    return point;
}

void WidgetHelper::drawOsd(QPainter *painter, const OsdInfo &osd, const QRect &rect)
{
    //绘制下区域看下效果
    //painter->setPen(Qt::red);
    //painter->drawRect(rect);

    int flag;
    QPoint point = getPointByPosition(osd, rect, flag);

    //不同的格式绘制不同的内容
    if (osd.format == OsdFormat_Image) {
        painter->drawImage(point, osd.image);
    } else {
        QString text = getTextByFormat(osd);
        //设置颜色及字号
        QFont font;
        if (osd.fontName != "wenquanyi") {
            font.setFamily(osd.fontName);
        }

        font.setPixelSize(osd.fontSize);
        painter->setPen(osd.color);
        painter->setFont(font);

        if (osd.position == OsdPosition_Custom) {
            painter->drawText(point, text);
        } else {
            painter->drawText(rect, flag, text);
        }
    }
}

void WidgetHelper::drawRect(QPainter *painter, const QRect &rect, int borderWidth, QColor borderColor, bool angle)
{
    painter->setPen(QPen(borderColor, borderWidth));
    //背景颜色
    borderColor.setAlpha(50);
    //painter->setBrush(QBrush(borderColor));

    int x = rect.x();
    int y = rect.y();
    int width = rect.width();
    int height = rect.height();

    if (!angle) {
        painter->drawRect(x, y, width, height);
    } else {
        //绘制四个角
        int offset = 10;
        painter->drawLine(x, y, x + offset, y);
        painter->drawLine(x, y, x, y + offset);
        painter->drawLine(x + width - offset, y, x + width, y);
        painter->drawLine(x + width, y, x + width, y + offset);
        painter->drawLine(x, y + height - offset, x, y + height);
        painter->drawLine(x, y + height, x + offset, y + height);
        painter->drawLine(x + width - offset, y + height, x + width, y + height);
        painter->drawLine(x + width, y + height - offset, x + width, y + height);
    }
}

void WidgetHelper::drawPoints(QPainter *painter, const QList<QPoint> &pts, int borderWidth, QColor borderColor)
{
    //至少要两个点
    if (pts.count() < 2) {
        return;
    }

    painter->setPen(QPen(borderColor, borderWidth));
    //背景颜色
    borderColor.setAlpha(50);
    //painter->setBrush(QBrush(borderColor));

    //绘制多边形
    QPainterPath path;
    //先移动到起始点
    path.moveTo(pts.first());
    //逐个连接线条
    int count = pts.count();
    for (int i = 1; i < count; ++i) {
        path.lineTo(pts.at(i));
    }

    //闭合图形
    path.closeSubpath();
    painter->drawPath(path);
}

void WidgetHelper::drawPath(QPainter *painter, QPainterPath path, int borderWidth, QColor borderColor)
{
    painter->setPen(QPen(borderColor, borderWidth));
    painter->drawPath(path);
}

QLabel *WidgetHelper::showImage(QLabel *label, QWidget *widget, const QImage &image)
{
    //等比缩放下分辨率过大的图片
#ifdef Q_OS_ANDROID
    int maxWidth = widget->width();
    int maxHeight = widget->height();
#else
    int maxWidth = 1280;
    int maxHeight = 720;
#endif
    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmap.width() > maxWidth || pixmap.height() > maxHeight) {
        pixmap = pixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    //实例化截图标签
    if (!label) {
        label = new QLabel;
        label->setWindowTitle("截图预览");
        label->setAlignment(Qt::AlignCenter);
    }

    //设置图片
    label->setPixmap(pixmap);

    //安卓上要用dialog窗体才正常
#ifdef Q_OS_ANDROID
    QDialog dialog;
    dialog.setWindowTitle("截图预览");
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    dialog.setLayout(layout);
    dialog.setGeometry(widget->geometry());
    dialog.exec();
    label = NULL;
#else
    label->resize(pixmap.size());
    label->raise();
    label->show();
#endif
    return label;
}

void WidgetHelper::addOsd(AbstractVideoWidget *widget, int &index)
{
    index++;
    if (index > 8) {
        index = 1;
    }

    if (index == 7) {
        widget->clearOsd();
        addOsd2(widget, 100);
        return;
    }

    int fontSize = widget->getVideoWidth() / 30;

    //演示添加多种标签(具体参数参见结构体)
    OsdInfo osd;
    osd.fontSize = fontSize;
    if (index == 1) {
        //左上角文字
        osd.text = "摄像头: 测试";

        //支持 \n 换行以及带上日期时间(可以用多个 \n 换多行)
        //osd.text = "第一行文本\n第二行文本\n%{localtime}";
        //osd.text = "第一行文本\n\n第二行文本\n\n%{localtime}";

        //可以指定字体和图形代码
        //osd.fontName = "iconfont";
        //osd.text = QChar(0xe505);
    } else if (index == 2) {
        //右上角图片
        osd.image = QImage(":/image/bg_novideo.png");
        osd.format = OsdFormat_Image;
        osd.position = OsdPosition_RightTop;

        //设置唯一名称标识并将图片保存(滤镜基本上都是支持指定图片文件)
        osd.name = "osd.png";
        QString file = QString("./%1").arg(osd.name);
        osd.image.save(file, "png");
    } else if (index == 3) {
        //指定坐标显示标签
        osd.text = "QPoint(100, 100)";
        osd.color = Qt::white;
        osd.point = QPoint(100, 100);
        osd.position = OsdPosition_Custom;
    } else if (index == 4) {
        //左下角时间
        osd.color = Qt::white;
        //带透明背景颜色
        osd.bgColor = QColor(255, 0, 0, 60);
        osd.format = OsdFormat_Time;
        osd.position = OsdPosition_LeftBottom;
    } else if (index == 5) {
        //右下角日期时间
        osd.color = "#A279C5";
        osd.format = OsdFormat_DateTime;
        osd.position = OsdPosition_RightBottom;
    } else if (index == 6) {
        //中间文字
        osd.text = "中间文字";
        osd.position = OsdPosition_Center;
    }

    if (index <= 6) {
        //添加标签
        widget->appendOsd(osd);
    } else {
        //清空所有标签
        widget->clearOsd();
    }
}

void WidgetHelper::addOsd2(AbstractVideoWidget *widget, int count)
{
    int width = widget->getVideoWidth() - 100;
    int height = widget->getVideoHeight() - 100;
    int fontSize = widget->getVideoWidth() / 50;

    static QStringList colors = QColor::colorNames();
    for (int i = 0; i < count; ++i) {
        OsdInfo osd;
        osd.fontSize = fontSize + rand() % 30;
        osd.name = QString("graph%1").arg(i);
        osd.text = QString("文本%1").arg(i);
        osd.color = colors.at(rand() % colors.count());
        osd.point = QPoint(rand() % width, rand() % height);
        osd.position = OsdPosition_Custom;
        widget->setOsd(osd);
    }
}

void WidgetHelper::addGraph(AbstractVideoWidget *widget, int &index)
{
    index++;
    if (index > 6) {
        index = 1;
    }

    if (index == 5) {
        widget->clearGraph();
        addGraph2(widget, 100);
        return;
    }

    int borderWidth = WidgetHelper::getBorderWidth(widget);

    //演示添加多种图形(具体参数参见结构体)
    GraphInfo graph;
    graph.borderWidth = borderWidth;
    if (widget->getWidgetPara().graphDrawMode == DrawMode_Source) {
        //随机生成矩形
        int width = widget->width() - 100;
        int height = widget->height() - 100;
        if (widget->getIsRunning()) {
            width = widget->getVideoWidth() - 100;
            height = widget->getVideoHeight() - 100;
        }

        int x = rand() % width;
        int y = rand() % height;
        int w = 80 + rand() % 30;
        int h = 30 + rand() % 40;
        graph.rect = QRect(x, y, w, h);
        static QStringList colors = QColor::colorNames();
        graph.borderColor = colors.at(rand() % colors.count());
    } else {
        if (index == 1) {
            //三个图形一起(矩形/点集合/路径)
            graph.rect = QRect(10, 30, 150, 100);
            graph.points = QList<QPoint>() << QPoint(200, 100) << QPoint(250, 50) << QPoint(300, 150);
            QPainterPath path;
            path.addEllipse(50, 50, 50, 50);
            graph.path = path;
        } else if (index == 2) {
            //矩形
            graph.borderColor = Qt::white;
            graph.rect = QRect(10, 160, 130, 80);
        } else if (index == 3) {
            //点集合
            graph.borderColor = Qt::green;
            graph.points = QList<QPoint>() << QPoint(300, 100) << QPoint(400, 50) << QPoint(500, 150) << QPoint(450, 200) << QPoint(350, 180);
        } else if (index == 4) {
            //路径(圆形/圆角矩形)
            graph.borderColor = "#22A3A9";
            QPainterPath path;
            path.addEllipse(500, 30, 100, 50);
            path.addRoundedRect(550, 100, 150, 100, 5, 5);
            graph.path = path;
        }
    }

    if (index <= 4) {
        //添加图形
        widget->appendGraph(graph);
    } else {
        //清空所有图形
        widget->clearGraph();
    }
}

void WidgetHelper::addGraph2(AbstractVideoWidget *widget, int count)
{
    int width = widget->getVideoWidth() - 100;
    int height = widget->getVideoHeight() - 100;
    int borderWidth = WidgetHelper::getBorderWidth(widget);
    int wx = widget->getVideoWidth() / 50;
    int hx = widget->getVideoHeight() / 50;

    static QStringList colors = QColor::colorNames();
    for (int i = 0; i < count; ++i) {
        GraphInfo graph;
        graph.name = QString("graph%1").arg(i);
        int x = rand() % width;
        int y = rand() % height;
        int w = wx + rand() % 40;
        int h = hx + rand() % 30;
        graph.rect = QRect(x, y, w, h);
        graph.borderWidth = borderWidth;
        graph.borderColor = colors.at(rand() % colors.count());
        widget->setGraph(graph);
    }
}
