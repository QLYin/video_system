﻿#pragma execution_character_set("utf-8")

#include "lcddatetime.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qdebug.h"

LcdDateTime::LcdDateTime(QWidget *parent) : QLCDNumber(parent)
{
    setDotHide(false);
    setFormat("yyyy-MM-dd HH:mm:ss");
    this->setSegmentStyle(QLCDNumber::Flat);

    isHide = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(setDateTime()));
    start(1000);
}

LcdDateTime::~LcdDateTime()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

QSize LcdDateTime::sizeHint() const
{
    return QSize(300, 50);
}

QSize LcdDateTime::minimumSizeHint() const
{
    return QSize(10, 10);
}

bool LcdDateTime::getDotHide() const
{
    return this->dotHide;
}

void LcdDateTime::setDotHide(bool dotHide)
{
    if (this->dotHide != dotHide) {
        this->dotHide = dotHide;
    }
}

QString LcdDateTime::getFormat() const
{
    return this->format;
}

void LcdDateTime::setFormat(const QString &format)
{
    if (this->format != format) {
        this->format = format;
        if (format == "yyyy-MM-dd HH:mm:ss") {
            digitCount = 19;
        } else if (format == "yyyy-MM-dd") {
            digitCount = 10;
        } else if (format == "HH:mm:ss") {
            digitCount = 8;
        }
        this->setDigitCount(digitCount);
    }
}

void LcdDateTime::start(int interval)
{
    setDateTime();
    timer->start(interval);
}

void LcdDateTime::setDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QString str = now.toString(format);
    if (dotHide) {
        if (!isHide) {
            str.replace(":", " ");
        }
        isHide = !isHide;
    }

    this->display(str);
}
