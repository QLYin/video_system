#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
include($$PWD/core_videoffmpeg_include.pri)

HEADERS += $$PWD/ffmpeginclude.h
HEADERS += $$PWD/ffmpegstruct.h

HEADERS += $$PWD/ffmpeghelper.h
SOURCES += $$PWD/ffmpeghelper.cpp

HEADERS += $$PWD/ffmpegutil.h
SOURCES += $$PWD/ffmpegutil.cpp

HEADERS += $$PWD/ffmpegfilter.h
SOURCES += $$PWD/ffmpegfilter.cpp

#引入视频保存相关类
HEADERS += $$PWD/ffmpegsave.h
SOURCES += $$PWD/ffmpegsave.cpp

#HEADERS += $$PWD/ffmpegsavesimple.h
#SOURCES += $$PWD/ffmpegsavesimple.cpp

#当前组件功能较多可能被多个项目引入
#通过定义一个标识按照需要引入代码文件
contains(DEFINES, videoffmpeg) {
HEADERS += $$PWD/ffmpegsync.h
SOURCES += $$PWD/ffmpegsync.cpp

HEADERS += $$PWD/ffmpegthread.h
SOURCES += $$PWD/ffmpegthread.cpp
}
