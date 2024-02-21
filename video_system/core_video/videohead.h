#ifndef VIDEOHEAD_H
#define VIDEOHEAD_H

//把这几个头文件全部包含下懒得每次增加一个新类又来引入对应头文件
#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtNetwork>
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef TIME
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#endif
#ifndef QDATE
#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#endif
#ifndef QTIME
#define QTIME qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif
#ifndef STRDATETIME
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#endif
#ifndef STRDATETIMEMS
#define STRDATETIMEMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))
#endif

#include "videostruct.h"
#include "videothread.h"

#endif // VIDEOHEAD_H
