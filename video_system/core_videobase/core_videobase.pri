#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
include($$PWD/core_videobase_path.pri)
include($$PWD/core_videobase_include.pri)

HEADERS += $$PWD/widgethead.h
HEADERS += $$PWD/widgetstruct.h

HEADERS += $$PWD/widgethelper.h
SOURCES += $$PWD/widgethelper.cpp

HEADERS += $$PWD/urlhelper.h
SOURCES += $$PWD/urlhelper.cpp

HEADERS += $$PWD/filterhelper.h
SOURCES += $$PWD/filterhelper.cpp

HEADERS += $$PWD/bannerwidget.h
SOURCES += $$PWD/bannerwidget.cpp

HEADERS += $$PWD/abstractvideothread.h
SOURCES += $$PWD/abstractvideothread.cpp

HEADERS += $$PWD/abstractvideowidget.h
SOURCES += $$PWD/abstractvideowidget.cpp

HEADERS += $$PWD/abstractsavethread.h
SOURCES += $$PWD/abstractsavethread.cpp

#Qt5中用QAbstractVideoSurface Qt6中用QVideoSink代替
contains(DEFINES, multimedia) {
lessThan(QT_MAJOR_VERSION, 6) {
HEADERS += $$PWD/abstractvideosurface.h
SOURCES += $$PWD/abstractvideosurface.cpp
}}

#没有引入过则引入这个类
!contains(DEFINES, imagelabel) {
DEFINES *= imagelabel
HEADERS += $$PWD/imagelabel.h
SOURCES += $$PWD/imagelabel.cpp
}

#没有引入音频模块则引用一个空类
!contains(DEFINES, audiox) {
HEADERS += $$PWD/audioplayer.h
SOURCES += $$PWD/audioplayer.cpp
}
