#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

HEADERS += $$PWD/videohead.h
HEADERS += $$PWD/videostruct.h

HEADERS += $$PWD/videourl.h
SOURCES += $$PWD/videourl.cpp

HEADERS += $$PWD/videobox.h
SOURCES += $$PWD/videobox.cpp

HEADERS += $$PWD/videoplayback.h
SOURCES += $$PWD/videoplayback.cpp

HEADERS += $$PWD/videoutil.h
SOURCES += $$PWD/videoutil.cpp

HEADERS += $$PWD/videohelper.h
SOURCES += $$PWD/videohelper.cpp

HEADERS += $$PWD/videothread.h
SOURCES += $$PWD/videothread.cpp

#避免和Qt4中的phonon库中的头文件冲突加个x
HEADERS += $$PWD/videowidgetx.h
SOURCES += $$PWD/videowidgetx.cpp

HEADERS += $$PWD/videomanage.h
SOURCES += $$PWD/videomanage.cpp

HEADERS += $$PWD/videotask.h
SOURCES += $$PWD/videotask.cpp
