#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QtXml>
#include <QtNetwork>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif

#pragma execution_character_set("utf-8")

#include "appconfig.h"
#include "appdata.h"
#include "appevent.h"
#include "dbdata.h"
#include "commonnav.h"
#include "formhelper.h"
#include "userhelper.h"
