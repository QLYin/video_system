QT += sql
lessThan(QT_MAJOR_VERSION, 5): QT += script

#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
HEADERS += $$PWD/dbhead.h

HEADERS += $$PWD/dbhelper.h
SOURCES += $$PWD/dbhelper.cpp

HEADERS += $$PWD/dbcleanthread.h
SOURCES += $$PWD/dbcleanthread.cpp

HEADERS += $$PWD/dbconnthread.h
SOURCES += $$PWD/dbconnthread.cpp

HEADERS += $$PWD/dbdelegate.h
SOURCES += $$PWD/dbdelegate.cpp

HEADERS += $$PWD/dbhttpthread.h
SOURCES += $$PWD/dbhttpthread.cpp

HEADERS += $$PWD/dbpage.h
SOURCES += $$PWD/dbpage.cpp

HEADERS += $$PWD/dbpagemodel.h
SOURCES += $$PWD/dbpagemodel.cpp

HEADERS += $$PWD/dbtablemodel.h
SOURCES += $$PWD/dbtablemodel.cpp

HEADERS += $$PWD/navpage.h
SOURCES += $$PWD/navpage.cpp
