#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
include($$PWD/core_videoopengl_include.pri)

HEADERS += $$PWD/openglinclude.h
contains(DEFINES, openglx) {
win32 {
LIBS *= -lopengl32 -lGLU32
}
HEADERS += $$PWD/rgbopenglwidget.h
SOURCES += $$PWD/rgbopenglwidget.cpp
HEADERS += $$PWD/yuvopenglwidget.h
SOURCES += $$PWD/yuvopenglwidget.cpp
HEADERS += $$PWD/nv12openglwidget.h
SOURCES += $$PWD/nv12openglwidget.cpp
} else {
HEADERS += $$PWD/rgbwidget.h
SOURCES += $$PWD/rgbwidget.cpp
HEADERS += $$PWD/yuvwidget.h
SOURCES += $$PWD/yuvwidget.cpp
HEADERS += $$PWD/nv12widget.h
SOURCES += $$PWD/nv12widget.cpp
}
