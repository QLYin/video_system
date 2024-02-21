greaterThan(QT_MAJOR_VERSION, 4) {
qtHaveModule(multimedia) {
QT += multimedia
DEFINES += playsound
}}

#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

HEADERS += $$PWD/base64helper.h
HEADERS += $$PWD/commonkey.h
HEADERS += $$PWD/commonnav.h
HEADERS += $$PWD/commonstyle.h
HEADERS += $$PWD/framelesswidget2.h
HEADERS += $$PWD/playwav.h
HEADERS += $$PWD/savelog.h
HEADERS += $$PWD/saveruntime.h

SOURCES += $$PWD/base64helper.cpp
SOURCES += $$PWD/commonkey.cpp
SOURCES += $$PWD/commonnav.cpp
SOURCES += $$PWD/commonstyle.cpp
SOURCES += $$PWD/framelesswidget2.cpp
SOURCES += $$PWD/playwav.cpp
SOURCES += $$PWD/savelog.cpp
SOURCES += $$PWD/saveruntime.cpp

