#ifndef DATAHEAD_H
#define DATAHEAD_H

#include <QtGui>
#include <QtSql>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#include <QtPrintSupport>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS qPrintable (QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif
#ifndef STRDATETIME
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#endif

#include "datastruct.h"
#include "datacsv.h"
#include "dataxls.h"
#include "dataprint.h"
#include "datahelper.h"

#endif // DATAHEAD_H
