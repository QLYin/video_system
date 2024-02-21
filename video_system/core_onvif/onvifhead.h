#ifndef ONVIFHEAD_H
#define ONVIFHEAD_H

#include <QtGui>
#include <QtXml>
#include <QtNetwork>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("hh:mm:ss zzz"))
#endif

//查看端口 netstat -aon|findstr "3702" 239.255.255.250/3702
//抓包过滤 ip.addr==192.168.0.64 and http  前缀还有 ip.dst ip.src
//更多规则 https://www.cnblogs.com/nmap/p/6291683.html
//官方文档 https://www.onvif.org/ch/profiles/specifications/

//onvif组播地址和端口
#define OnvifHost "239.255.255.250"
#define OnvifPort 3702

//返回结果字符串分隔符
#define OnvifValueSplit ":"
#define OnvifResultSplit "  "

//跳过空格
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#define SkipEmptyPartsx Qt::SkipEmptyParts
#else
#define SkipEmptyPartsx (QString::SkipEmptyParts)
#endif

#include "onvifstruct.h"
#include "onvifxml.h"
#include "onvifhelper.h"

#include "onvifsearch.h"
#include "onvifdevice.h"
#include "onvifrequest.h"
#include "onvifquery.h"

class OnvifDevice;
#include "onvifbase.h"
#include "onvifptz.h"
#include "onvifvideo.h"
#include "onvifevent.h"
#include "onvifsnap.h"
#include "onvifother.h"

#endif // ONVIFHEAD_H
