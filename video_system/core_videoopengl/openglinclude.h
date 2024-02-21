#ifndef OPENGLINCLUDE_H
#define OPENGLINCLUDE_H

#include "qcoreapplication.h"
#include "qstringlist.h"
#include "qdatetime.h"
#include "qdebug.h"

#ifndef GL_RG
#define GL_RG 0x8227
#endif
#ifndef GL_RED
#define GL_RED 0x1903
#endif
#ifndef GL_QUADS
#define GL_QUADS 0x0007
#endif
#ifndef GL_CLAMP
#define GL_CLAMP 0x2900
#endif
#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#endif

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif

#ifdef openglx
#include "rgbopenglwidget.h"
#include "yuvopenglwidget.h"
#include "nv12openglwidget.h"
#else
#include "rgbwidget.h"
#include "yuvwidget.h"
#include "nv12widget.h"
#endif

//#define VersionString ""
#define VersionString "#version 330 \n"
//#define VersionString "#version 330 core \n"
//#define VersionString "#version 330 compatibility \n"

//opengles的float/int等要手动指定精度
inline void initFragment(QStringList &list)
{
    bool useOpenGLES = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    useOpenGLES = QCoreApplication::testAttribute(Qt::AA_UseOpenGLES);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    useOpenGLES = false;
#endif
#ifdef Q_OS_LINUX
    useOpenGLES = false;
#endif
#ifdef __arm__
    useOpenGLES = true;
#endif
#ifdef Q_OS_ANDROID
    useOpenGLES = true;
#endif
    if (useOpenGLES) {
        list << "precision mediump int;";
        list << "precision mediump float;";
    }
}

#endif // OPENGLINCLUDE_H
