#ifndef AUDIOINCLUDE_H
#define AUDIOINCLUDE_H

#include "qglobal.h"
#ifdef multimedia
#include <QtMultimedia>

//由于Qt6中的类改了名字需要重定义省的到处定义
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
#define QAudioInputx QAudioSource
#define QAudioOutputx QAudioSink
#define QAudioRecorder QMediaRecorder
#else
#define QAudioInputx QAudioInput
#define QAudioOutputx QAudioOutput
#define QAudioDevice QAudioDeviceInfo
#endif

#else
class QIODevice;
class QAudioInputx;
class QAudioOutputx;
class QAudioRecorder;
#endif

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

#include "qdatetime.h"
#include "qdebug.h"
#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif

#endif // AUDIOINCLUDE_H
