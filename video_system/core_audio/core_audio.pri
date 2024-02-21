#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
include($$PWD/core_audio_include.pri)

HEADERS += $$PWD/audioinclude.h

HEADERS += $$PWD/audiohelper.h
SOURCES += $$PWD/audiohelper.cpp

HEADERS += $$PWD/audiolevel.h
SOURCES += $$PWD/audiolevel.cpp

HEADERS += $$PWD/audioplayer.h
SOURCES += $$PWD/audioplayer.cpp

HEADERS += $$PWD/audiorecorder.h
SOURCES += $$PWD/audiorecorder.cpp
