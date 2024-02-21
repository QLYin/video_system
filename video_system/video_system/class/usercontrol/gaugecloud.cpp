#pragma execution_character_set("utf-8")

#include "gaugecloud.h"
#include "qfontdatabase.h"
#include "qpainter.h"
#include "qevent.h"
#include "qmath.h"
#include "qtimer.h"
#include "qdebug.h"

GaugeCloud::GaugeCloud(QWidget *parent) : QWidget(parent)
{
    margin = 0;
    gradient = true;
    centerBorderWidth = 3;

    iconText = "0xf0d7";
    centerText = "0xf021";
    cloudStyle = CloudStyle_Black;
    this->initColor();

    enter = false;
    pressed = false;

    showAngle = true;
    autoRepeat = true;
    autoRepeatDelay = 500;
    autoRepeatInterval = 200;

    position = 0;
    positions << "底部" << "左下角" << "左侧" << "左上角" << "顶部" << "右上角" << "右侧" << "右下角" << "中间";

    //启动定时器判断长按
    timer = new QTimer(this);
    timer->setInterval(autoRepeatInterval);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkPressed()));

    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/font/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    this->setFont(iconFont);
    this->setMouseTracking(true);
}

GaugeCloud::~GaugeCloud()
{
}

void GaugeCloud::enterEvent(QEvent *)
{
    enter = true;
    this->update();
}

void GaugeCloud::leaveEvent(QEvent *)
{
    enter = false;
    this->update();
}

void GaugeCloud::mousePressEvent(QMouseEvent *)
{
    pressed = true;
    this->update();
}

void GaugeCloud::mouseReleaseEvent(QMouseEvent *)
{
    timer->stop();
    pressed = false;
    this->update();
    Q_EMIT mouseReleased(position, positions.at(position));
}

void GaugeCloud::mouseMoveEvent(QMouseEvent *e)
{
    lastPoint = e->pos();
    this->update();
}

void GaugeCloud::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    //以中心点为基准,分别计算八方位区域和中间区域
    QPointF center = this->rect().center();
    double centerSize = (double)side / ((double)100 / 30);
    double iconSize = (double)side / ((double)100 / 10);
    double offset1 = 3.6;
    double offset2 = 2.65;

    //计算当前按下点到中心点的距离,如果小于内圆的半径则认为在内圆中
    double offset = twoPtDistance(lastPoint, this->rect().center());
    inCenter = (offset <= centerSize / 2);

    //中间区域
    centerRect = QRectF(center.x() - centerSize / 2, center.y() - centerSize / 2, centerSize, centerSize);
    //左侧图标区域
    leftRect = QRectF(center.x() - iconSize * offset1, center.y() - iconSize / 2, iconSize, iconSize);
    //上侧图标区域
    topRect = QRectF(center.x() - iconSize / 2, center.y() - iconSize * offset1, iconSize, iconSize);
    //右侧图标区域
    rightRect = QRectF(center.x() + iconSize * (offset1 - 1), center.y() - iconSize / 2, iconSize, iconSize);
    //下侧图标区域
    bottomRect = QRectF(center.x() - iconSize / 2, center.y() + iconSize * (offset1 - 1), iconSize, iconSize);
    //左上角图标区域
    leftTopRect = QRectF(center.x() - iconSize * offset2, center.y() - iconSize * offset2, iconSize, iconSize);
    //右上角图标区域
    rightTopRect = QRectF(center.x() + iconSize * (offset2 - 1), center.y() - iconSize * offset2, iconSize, iconSize);
    //左下角图标区域
    leftBottomRect = QRectF(center.x() - iconSize * offset2, center.y() + iconSize * (offset2 - 1), iconSize, iconSize);
    //右下角图标区域
    rightBottomRect = QRectF(center.x() + iconSize * (offset2 - 1), center.y() + iconSize * (offset2 - 1), iconSize, iconSize);

    //不显示斜角就将该区域置空
    if (!showAngle) {
        leftTopRect = QRect(0, 0, 0, 0);
        rightTopRect = QRect(0, 0, 0, 0);
        leftBottomRect = QRect(0, 0, 0, 0);
        rightBottomRect = QRect(0, 0, 0, 0);
    }

    //绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);

    //有边距则先绘制边距背景
    if (margin > 0) {
        drawCircle(&painter, 99,  gradient ? bgColor : borderColor1);
    }

    if (gradient) {
        //设置圆锥渐变
        QConicalGradient color(0, 0, 100);
        color.setColorAt(0, borderColor1);
        color.setColorAt(0.4, borderColor2);
        color.setColorAt(0.7, borderColor3);
        color.setColorAt(1, borderColor4);
        //绘制色彩外圆
        drawCircle(&painter, 99 - margin, color);
        //绘制中间背景
        drawCircle(&painter, 90 - margin, bgColor);
    } else {
        //绘制外圆背景
        drawCircle(&painter, 99 - margin, borderColor1);
        //绘制圆弧
        drawArc(&painter, 91 - margin, borderColor2);
        //绘制中间背景
        drawCircle(&painter, 83 - margin, bgColor);
        //绘制内圆外边框
        //drawCircle(&painter, 36 - margin, borderColor3);
    }

    //绘制内圆边框
    drawCircle(&painter, 30 + centerBorderWidth - margin, (pressed && inCenter) ? pressColor : centerBorderColor);
    //绘制内圆背景
    drawCircle(&painter, 30 - margin, inCenter ? centerHoverColor : centerBgColor);

    //绘制八方位+中间图标
    drawText(&painter);

#if 0
    //重置坐标系,并绘制八方位区域及中间区域,判断是否正确
    painter.resetTransform();
    painter.setPen(Qt::white);
    painter.drawRect(centerRect);
    painter.drawRect(leftRect);
    painter.drawRect(topRect);
    painter.drawRect(rightRect);
    painter.drawRect(bottomRect);
    painter.drawRect(leftTopRect);
    painter.drawRect(rightTopRect);
    painter.drawRect(leftBottomRect);
    painter.drawRect(rightBottomRect);
#endif
}

void GaugeCloud::drawCircle(QPainter *painter, int radius, const QBrush &brush)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);

    //绘制圆
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void GaugeCloud::drawArc(QPainter *painter, int radius, const QColor &color)
{
    painter->save();
    painter->setBrush(Qt::NoBrush);

    QPen pen;
    pen.setWidthF(10);
    pen.setColor(color);
    painter->setPen(pen);

    QRectF rect = QRectF(-radius, -radius, radius * 2, radius * 2);
    painter->drawArc(rect, 0 * 16, 360 * 16);

    painter->restore();
}

void GaugeCloud::drawText(QPainter *painter)
{
    bool ok;
    int radius = 100;
    painter->save();

    //判断当前按下坐标是否在中心区域,按下则文本不同颜色
    if (inCenter) {
        if (pressed) {
            position = 8;
            if (autoRepeat) {
                QTimer::singleShot(autoRepeatDelay, timer, SLOT(start()));
            }

            Q_EMIT mousePressed(position, positions.at(position));
        }

        painter->setPen(pressed ? pressColor : enterColor);
    } else {
        painter->setPen(textColor);
    }

    QFont font;
    font.setPixelSize(30);
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    font.setHintingPreference(QFont::PreferNoHinting);
#endif
    painter->setFont(font);

    //绘制中间图标
    QRectF centerRect(-radius, -radius, radius * 2, radius * 2);
    QString centerText = this->centerText;
    centerText.replace("0x", "");
    QChar centerChar = QChar(centerText.toInt(&ok, 16));
    painter->drawText(centerRect, Qt::AlignCenter, centerChar);

    //绘制八方位图标
    radius = 72 - margin;
    int offset = 15;
    int steps = 8;
    double angleStep = 360.0 / steps;

    font.setPixelSize(28);
    painter->setFont(font);

    //从下侧图标开始绘制,顺时针旋转
    QRect iconRect(-offset / 2, radius - offset, offset, offset);
    QString iconText = this->iconText;
    iconText.replace("0x", "");
    QChar iconChar = QChar(iconText.toInt(&ok, 16));
    for (int i = 0; i < steps; ++i) {
        //没有显示斜角则不绘制
        if (!showAngle && (i == 1 || i == 3 || i == 5 || i == 7)) {
            painter->rotate(angleStep);
            continue;
        }

        //判断鼠标按下的是哪个区域
        bool contains = false;
        if (bottomRect.contains(lastPoint) && i == 0) {
            contains = true;
        } else if (leftBottomRect.contains(lastPoint) && i == 1) {
            contains = true;
        } else if (leftRect.contains(lastPoint) && i == 2) {
            contains = true;
        } else if (leftTopRect.contains(lastPoint) && i == 3) {
            contains = true;
        } else if (topRect.contains(lastPoint) && i == 4) {
            contains = true;
        } else if (rightTopRect.contains(lastPoint) && i == 5) {
            contains = true;
        } else if (rightRect.contains(lastPoint) && i == 6) {
            contains = true;
        } else if (rightBottomRect.contains(lastPoint) && i == 7) {
            contains = true;
        }

        if (contains) {
            if (pressed) {
                position = i;
                if (autoRepeat) {
                    QTimer::singleShot(autoRepeatDelay, timer, SLOT(start()));
                }

                Q_EMIT mousePressed(position, positions.at(position));
            }

            painter->setPen(pressed ? pressColor : enterColor);
        } else {
            painter->setPen(textColor);
        }

        painter->drawText(iconRect, Qt::AlignCenter, iconChar);
        painter->rotate(angleStep);
    }

    painter->restore();
}

void GaugeCloud::initColor()
{
    if (cloudStyle == CloudStyle_Black) {
        if (gradient) {
            borderColor1 = QColor(135, 139, 144);
            borderColor2 = QColor(41, 44, 48);
            borderColor3 = QColor(46, 49, 54);
            borderColor4 = QColor(135, 139, 144);
        } else {
            borderColor1 = QColor(25, 31, 37);
            borderColor2 = QColor(41, 45, 54);
            borderColor3 = QColor(30, 30, 30);
        }

        bgColor = QColor(68, 73, 79);
        textColor = QColor(200, 200, 200);
        enterColor = QColor(230, 230, 230);
        pressColor = QColor(48, 190, 255);

        centerBorderColor = QColor(40, 43, 51);
        centerBgColor = QColor(92, 95, 100);
        centerHoverColor = QColor(68, 73, 79);
    } else if (cloudStyle == CloudStyle_White) {
        if (gradient) {
            borderColor1 = QColor(34, 163, 169);
            borderColor2 = QColor(240, 201, 136);
            borderColor3 = QColor(211, 77, 37);
            borderColor4 = QColor(34, 163, 169);
        } else {
            borderColor1 = QColor(250, 250, 250);
            borderColor2 = QColor(180, 180, 180);
            borderColor3 = QColor(160, 160, 160);
        }

        bgColor = QColor(249, 249, 249);
        textColor = QColor(150, 150, 150);
        enterColor = QColor(50, 50, 50);
        pressColor = QColor(229, 82, 72);

        centerBorderColor = QColor(208, 208, 208);
        centerBgColor = QColor(245, 245, 245);
        centerHoverColor = QColor(255, 255, 255);
    } else if (cloudStyle == CloudStyle_Blue) {
        if (gradient) {
            borderColor1 = QColor(79, 163, 219);
            borderColor2 = QColor(227, 183, 106);
            borderColor3 = QColor(217, 178, 109);
            borderColor4 = QColor(79, 163, 219);
        } else {
            borderColor1 = QColor(23, 54, 81);
            borderColor2 = QColor(4, 86, 126);
            borderColor3 = QColor(4, 86, 126);
        }

        bgColor = QColor(31, 66, 98);
        textColor = QColor(75, 165, 221);
        enterColor = QColor(50, 185, 207);
        pressColor = QColor(67, 193, 251);

        centerBorderColor = QColor(23, 54, 81);
        centerBgColor = QColor(34, 73, 115);
        centerHoverColor = QColor(35, 82, 133);
    } else if (cloudStyle == CloudStyle_Purple) {
        if (gradient) {
            borderColor1 = QColor(87, 87, 155);
            borderColor2 = QColor(129, 82, 130);
            borderColor3 = QColor(54, 89, 166);
            borderColor4 = QColor(87, 87, 155);
        } else {
            borderColor1 = QColor(49, 48, 82);
            borderColor2 = QColor(55, 55, 92);
            borderColor3 = QColor(146, 144, 198);
        }

        bgColor = QColor(55, 55, 92);
        textColor = QColor(146, 144, 198);
        enterColor = QColor(154, 96, 180);
        pressColor = enterColor.lighter();

        centerBorderColor = QColor(49, 48, 82);
        centerBgColor = QColor(62, 59, 103);
        centerHoverColor = QColor(85, 81, 137);
    }
}

void GaugeCloud::checkPressed()
{
    if (pressed) {
        Q_EMIT mousePressed(position, positions.at(position));
    } else {
        timer->stop();
    }
}

double GaugeCloud::twoPtDistance(const QPointF &pt1, const QPointF &pt2)
{
    double offsetX = pt2.x() - pt1.x();
    double offsetY = pt2.y() - pt1.y();
    return qSqrt((offsetX * offsetX) + (offsetY * offsetY));
}

QSize GaugeCloud::sizeHint() const
{
    return QSize(200, 200);
}

QSize GaugeCloud::minimumSizeHint() const
{
    return QSize(50, 50);
}

int GaugeCloud::getMargin() const
{
    return this->margin;
}

void GaugeCloud::setMargin(int margin)
{
    if (this->margin != margin) {
        this->margin = margin;
        this->update();
    }
}

bool GaugeCloud::getGradient() const
{
    return this->gradient;
}

void GaugeCloud::setGradient(bool gradient)
{
    if (this->gradient != gradient) {
        this->gradient = gradient;
        this->initColor();
        this->update();
    }
}

QColor GaugeCloud::getBorderColor1() const
{
    return this->borderColor1;
}

void GaugeCloud::setBorderColor1(const QColor &borderColor1)
{
    if (this->borderColor1 != borderColor1) {
        this->borderColor1 = borderColor1;
        this->update();
    }
}

QColor GaugeCloud::getBorderColor2() const
{
    return this->borderColor2;
}

void GaugeCloud::setBorderColor2(const QColor &borderColor2)
{
    if (this->borderColor2 != borderColor2) {
        this->borderColor2 = borderColor2;
        this->update();
    }
}

QColor GaugeCloud::getBorderColor3() const
{
    return this->borderColor3;
}

void GaugeCloud::setBorderColor3(const QColor &borderColor3)
{
    if (this->borderColor3 != borderColor3) {
        this->borderColor3 = borderColor3;
        this->update();
    }
}

QColor GaugeCloud::getBorderColor4() const
{
    return this->borderColor4;
}

void GaugeCloud::setBorderColor4(const QColor &borderColor4)
{
    if (this->borderColor4 != borderColor4) {
        this->borderColor4 = borderColor4;
        this->update();
    }
}

QColor GaugeCloud::getBgColor() const
{
    return this->bgColor;
}

void GaugeCloud::setBgColor(const QColor &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
        this->update();
    }
}

QColor GaugeCloud::getTextColor() const
{
    return this->textColor;
}

void GaugeCloud::setTextColor(const QColor &textColor)
{
    if (this->textColor != textColor) {
        this->textColor = textColor;
        this->update();
    }
}

QColor GaugeCloud::getEnterColor() const
{
    return this->enterColor;
}

void GaugeCloud::setEnterColor(const QColor &enterColor)
{
    if (this->enterColor != enterColor) {
        this->enterColor = enterColor;
        this->update();
    }
}

QColor GaugeCloud::getPressColor() const
{
    return this->pressColor;
}

void GaugeCloud::setPressColor(const QColor &pressColor)
{
    if (this->pressColor != pressColor) {
        this->pressColor = pressColor;
        this->update();
    }
}

int GaugeCloud::getCenterBorderWidth() const
{
    return this->centerBorderWidth;
}

void GaugeCloud::setCenterBorderWidth(int centerBorderWidth)
{
    if (this->centerBorderWidth != centerBorderWidth) {
        this->centerBorderWidth = centerBorderWidth;
        this->update();
    }
}

QColor GaugeCloud::getCenterBorderColor() const
{
    return this->centerBorderColor;
}

void GaugeCloud::setCenterBorderColor(const QColor &centerBorderColor)
{
    if (this->centerBorderColor != centerBorderColor) {
        this->centerBorderColor = centerBorderColor;
        this->update();
    }
}

QColor GaugeCloud::getCenterBgColor() const
{
    return this->centerBgColor;
}

void GaugeCloud::setCenterBgColor(const QColor &centerBgColor)
{
    if (this->centerBgColor != centerBgColor) {
        this->centerBgColor = centerBgColor;
        this->update();
    }
}

QColor GaugeCloud::getCenterHoverColor() const
{
    return this->centerHoverColor;
}

void GaugeCloud::setCenterHoverColor(const QColor &centerHoverColor)
{
    if (this->centerHoverColor != centerHoverColor) {
        this->centerHoverColor = centerHoverColor;
        this->update();
    }
}

QString GaugeCloud::getIconText() const
{
    return this->iconText;
}

void GaugeCloud::setIconText(const QString &iconText)
{
    if (this->iconText != iconText) {
        this->iconText = iconText;
        this->update();
    }
}

QString GaugeCloud::getCenterText() const
{
    return this->centerText;
}

void GaugeCloud::setCenterText(const QString &centerText)
{
    if (this->centerText != centerText) {
        this->centerText = centerText;
        this->update();
    }
}

GaugeCloud::CloudStyle GaugeCloud::getCloudStyle() const
{
    return this->cloudStyle;
}

void GaugeCloud::setCloudStyle(const GaugeCloud::CloudStyle &cloudStyle)
{
    if (this->cloudStyle != cloudStyle) {
        this->cloudStyle = cloudStyle;
        this->initColor();
        this->update();
    }
}

bool GaugeCloud::getShowAngle() const
{
    return this->showAngle;
}

void GaugeCloud::setShowAngle(bool showAngle)
{
    if (this->showAngle != showAngle) {
        this->showAngle = showAngle;
        this->update();
    }
}

bool GaugeCloud::getAutoRepeat() const
{
    return this->autoRepeat;
}

void GaugeCloud::setAutoRepeat(bool autoRepeat)
{
    if (this->autoRepeat != autoRepeat) {
        this->autoRepeat = autoRepeat;
    }
}

int GaugeCloud::getAutoRepeatDelay() const
{
    return this->autoRepeatDelay;
}

void GaugeCloud::setAutoRepeatDelay(int autoRepeatDelay)
{
    if (this->autoRepeatDelay != autoRepeatDelay) {
        this->autoRepeatDelay = autoRepeatDelay;
    }
}

int GaugeCloud::getAutoRepeatInterval() const
{
    return this->autoRepeatInterval;
}

void GaugeCloud::setAutoRepeatInterval(int autoRepeatInterval)
{
    if (this->autoRepeatInterval != autoRepeatInterval) {
        this->autoRepeatInterval = autoRepeatInterval;
        timer->setInterval(autoRepeatInterval);
    }
}
