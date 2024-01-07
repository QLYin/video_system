#ifndef GLOBALHEAD_H
#define GLOBALHEAD_H

#pragma execution_character_set("utf-8")

//日期时间字符串宏定义
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

//数据 0
#define data_zero (char)0x00

//定义换行符
#ifdef Q_OS_WIN
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

//定义无边框标题栏高度+对话框最小宽高
#ifdef __arm__
#define QtTitleMinSize 40
#define QtDialogMinWidth 350
#define QtDialogMinHeight 180
#define QtBtnMinWidth 90
#define QtIconWidth 22
#define QtIconHeight 22
#else
#define QtTitleMinSize 30
#define QtDialogMinWidth 280
#define QtDialogMinHeight 150
#define QtBtnMinWidth 80
#define QtIconWidth 18
#define QtIconHeight 18
#endif

#include "head.h"
#include "iconhelper.h"
#include "globalconfig.h"
#include "globalstyle.h"
#include "qtform.h"

#endif // GLOBALHEAD_H
