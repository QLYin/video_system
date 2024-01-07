QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
HEADERS += $$PWD/datahead.h
HEADERS += $$PWD/datastruct.h

HEADERS += $$PWD/datacsv.h
SOURCES += $$PWD/datacsv.cpp

HEADERS += $$PWD/datahelper.h
SOURCES += $$PWD/datahelper.cpp

HEADERS += $$PWD/dataother.h
SOURCES += $$PWD/dataother.cpp

HEADERS += $$PWD/dataprint.h
SOURCES += $$PWD/dataprint.cpp

HEADERS += $$PWD/datareport.h
SOURCES += $$PWD/datareport.cpp

HEADERS += $$PWD/dataxls.h
SOURCES += $$PWD/dataxls.cpp
