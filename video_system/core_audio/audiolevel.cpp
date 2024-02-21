#include "audiolevel.h"
#include "qpainter.h"
#include "qdebug.h"

AudioLevel::AudioLevel(QWidget *parent) : QWidget(parent)
{
    level = 0;
    borderWidth = 2;

    //borderColor = QColor(0, 0, 0);
    //bgColor = QColor(255, 0, 0);

    QPalette palette;
    borderColor = palette.color(QPalette::Shadow);
    bgColor = palette.color(QPalette::Highlight);
}

void AudioLevel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    //绘制边框
    QPen pen;
    pen.setWidth(borderWidth);
    pen.setColor(borderColor);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(this->rect());

    //绘制进度
    int offset = borderWidth / 2 + 1;
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRect(offset, offset, width() * level, height() - offset * 2);
}

void AudioLevel::setLevel(qreal level)
{
    this->level = level;
    this->update();
}

void AudioLevel::clear()
{
    this->setLevel(0);
}
