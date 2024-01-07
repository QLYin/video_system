#pragma execution_character_set("utf-8")

#include "barvolume.h"
#include "qevent.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qdebug.h"

BarVolume::BarVolume(QWidget *parent) : QWidget(parent)
{
    vertical = true;
    reverse = false;
    pressAdjust = false;

    value = 0;
    step = 10;
    space = 5;
    padding = 3;
    radius = 5;

    bgColorStart = QColor(60, 60, 60);
    bgColorEnd = QColor(100, 100, 100);
    barBgColor = QColor(167, 167, 167);

    barColorStart = QColor(85, 255, 0);
    barColorMid = QColor(249, 216, 47);
    barColorEnd = QColor(249, 107, 24);
}

void BarVolume::mousePressEvent(QMouseEvent *e)
{
    mouseMoveEvent(e);
}

void BarVolume::mouseMoveEvent(QMouseEvent *e)
{
    if (!pressAdjust) {
        return;
    }

    int value = 0;
    QPoint p = e->pos();

    //下面用来判断值0(小于第一格的一半)
    if (vertical) {
        if (reverse) {
            QRectF rect = rects.first();
            if (p.y() < (rect.y() + rect.height() / 2)) {
                this->setValue(value);
                return;
            }
        } else {
            QRectF rect = rects.last();
            if (p.y() > (rect.y() + rect.height() / 2)) {
                this->setValue(value);
                return;
            }
        }
    } else {
        if (reverse) {
            QRectF rect = rects.last();
            if (p.x() > (rect.x() + rect.width() / 2)) {
                this->setValue(value);
                return;
            }
        } else {
            QRectF rect = rects.first();
            if (p.x() < (rect.x() + rect.width() / 2)) {
                this->setValue(value);
                return;
            }
        }
    }

    int count = rects.count();
    for (int i = 0; i < count; ++i) {
        QRectF rect = rects.at(i);
        //这里可以自行增加代码过滤到中间才切换
        if (rect.contains(p)) {
            if (vertical) {
                value = (reverse ? (i + 1) : (step - i));
            } else {
                value = (reverse ? (step - i) : (i + 1));
            }

            this->setValue(value);
            break;
        }
    }
}

void BarVolume::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景
    drawBg(&painter);
    //绘制进度背景
    drawBarBg(&painter);
    //绘制进度
    drawBar(&painter);
}

void BarVolume::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient bgGradient = getLinearGradient();
    bgGradient.setColorAt(0.0, bgColorStart);
    bgGradient.setColorAt(1.0, bgColorEnd);
    painter->setBrush(bgGradient);
    painter->drawRect(rect());
    painter->restore();
}

void BarVolume::drawBarBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setOpacity(0.4);
    painter->setBrush(barBgColor);

    rects.clear();
    double start = space;
    double increment = getIncrement();
    for (int i = 0; i < step; ++i) {
        QRectF rect;
        if (vertical) {
            rect = QRectF(QPointF(space, start), QPointF(width() - space, start + increment));
        } else {
            rect = QRectF(QPointF(start, space), QPointF(start + increment, height() - space));
        }

        //将每个进度对应的区域存起来(方便用来按下调节判断到了哪个进度)
        rects << rect;
        painter->drawRoundedRect(rect, radius, radius);
        start += increment + padding;
    }

    painter->restore();
}

void BarVolume::drawBar(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setOpacity(1.0);

    QLinearGradient barGradient = getLinearGradient();
    barGradient.setColorAt(0.0, barColorStart);
    barGradient.setColorAt(0.5, barColorMid);
    barGradient.setColorAt(1.0, barColorEnd);
    painter->setBrush(barGradient);

    double start = space;
    double increment = getIncrement();
    if (vertical) {
        start = reverse ? space : (padding * (step - value) + (step - value) * increment + space);
    } else {
        start = reverse ? (padding * (step - value) + (step - value) * increment + space) : space;
    }

    for (int i = 0; i < value; ++i) {
        QRectF rect;
        if (vertical) {
            rect = QRectF(QPointF(space, start), QPointF(width() - space, start + increment));
        } else {
            rect = QRectF(QPointF(start, space), QPointF(start + increment, height() - space));
        }

        painter->drawRoundedRect(rect, radius, radius);
        start += increment + padding;
    }

    painter->restore();
}

double BarVolume::getIncrement()
{
    //每格的步长 = (高度或宽度 - 上下两个间隔 - (格数 - 1) * 间隔) / 格数
    return (double)((vertical ? height() : width()) - (space * 2) - (step - 1) * padding) / step;
}

QSize BarVolume::sizeHint() const
{
    return QSize(100, 300);
}

QSize BarVolume::minimumSizeHint() const
{
    return QSize(10, 30);
}

QLinearGradient BarVolume::getLinearGradient()
{
    QLinearGradient linearGradient;
    if (vertical) {
        if (reverse) {
            linearGradient = QLinearGradient(QPointF(0, 0), QPointF(0, height()));
        } else {
            linearGradient = QLinearGradient(QPointF(0, height()), QPointF(0, 0));
        }
    } else {
        if (reverse) {
            linearGradient = QLinearGradient(QPointF(width(), 0), QPointF(0, 0));
        } else {
            linearGradient = QLinearGradient(QPointF(0, 0), QPointF(width(), 0));
        }
    }

    return linearGradient;
}

bool BarVolume::getVertical() const
{
    return this->vertical;
}

void BarVolume::setVertical(bool vertical)
{
    if (this->vertical != vertical) {
        this->vertical = vertical;
        this->update();
    }
}

bool BarVolume::getReverse() const
{
    return this->reverse;
}

void BarVolume::setReverse(bool reverse)
{
    if (this->reverse != reverse) {
        this->reverse = reverse;
        this->update();
    }
}

bool BarVolume::getPressAdjust() const
{
    return this->pressAdjust;
}

void BarVolume::setPressAdjust(bool pressAdjust)
{
    if (this->pressAdjust != pressAdjust) {
        this->pressAdjust = pressAdjust;
        this->update();
    }
}

int BarVolume::getValue() const
{
    return this->value;
}

void BarVolume::setValue(int value)
{
    if (this->value != value) {
        this->value = (value > step ? step : value);
        this->update();
        Q_EMIT valueChanged(this->value);
    }
}

void BarVolume::setValue(qreal value)
{
    this->setValue(int(value * step));
}

void BarVolume::clear()
{
    this->value = 0;
    this->update();
}

int BarVolume::getStep() const
{
    return this->step;
}

void BarVolume::setStep(int step)
{
    if (this->step != step) {
        this->step = step;
        this->update();
    }
}

int BarVolume::getSpace() const
{
    return this->space;
}

void BarVolume::setSpace(int space)
{
    if (this->space != space) {
        this->space = space;
        this->update();
    }
}

int BarVolume::getPadding() const
{
    return this->padding;
}

void BarVolume::setPadding(int padding)
{
    if (this->padding != padding) {
        this->padding = padding;
        this->update();
    }
}

int BarVolume::getRadius() const
{
    return this->radius;
}

void BarVolume::setRadius(int radius)
{
    if (this->radius != radius) {
        this->radius = radius;
        this->update();
    }
}

QColor BarVolume::getBgColorStart() const
{
    return this->bgColorStart;
}

void BarVolume::setBgColorStart(QColor bgColorStart)
{
    if (this->bgColorStart != bgColorStart) {
        this->bgColorStart = bgColorStart;
        this->update();
    }
}

QColor BarVolume::getBgColorEnd() const
{
    return this->bgColorEnd;
}

void BarVolume::setBgColorEnd(QColor bgColorEnd)
{
    if (this->bgColorEnd != bgColorEnd) {
        this->bgColorEnd = bgColorEnd;
        this->update();
    }
}

QColor BarVolume::getBarBgColor() const
{
    return this->barBgColor;
}

void BarVolume::setBarBgColor(QColor barBgColor)
{
    if (this->barBgColor != barBgColor) {
        this->barBgColor = barBgColor;
        this->update();
    }
}

QColor BarVolume::getBarColorStart() const
{
    return this->barColorStart;
}

void BarVolume::setBarColorStart(QColor barColorStart)
{
    if (this->barColorStart != barColorStart) {
        this->barColorStart = barColorStart;
        this->update();
    }
}

QColor BarVolume::getBarColorMid() const
{
    return this->barColorMid;
}

void BarVolume::setBarColorMid(QColor barColorMid)
{
    if (this->barColorMid != barColorMid) {
        this->barColorMid = barColorMid;
        this->update();
    }
}

QColor BarVolume::getBarColorEnd() const
{
    return this->barColorEnd;
}

void BarVolume::setBarColorEnd(QColor barColorEnd)
{
    if (this->barColorEnd != barColorEnd) {
        this->barColorEnd = barColorEnd;
        this->update();
    }
}
