#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
HEADERS += $$PWD/tcpfilehead.h

HEADERS += $$PWD/frmfileupload.h
SOURCES += $$PWD/frmfileupload.cpp
FORMS   += $$PWD/frmfileupload.ui

HEADERS += $$PWD/tcpfilehelper.h
SOURCES += $$PWD/tcpfilehelper.cpp

HEADERS += $$PWD/tcpreceivefileserver.h
SOURCES += $$PWD/tcpreceivefileserver.cpp

HEADERS += $$PWD/tcpreceivefilethread.h
SOURCES += $$PWD/tcpreceivefilethread.cpp

HEADERS += $$PWD/tcpsearchfileserver.h
SOURCES += $$PWD/tcpsearchfileserver.cpp

HEADERS += $$PWD/tcpsendfilethread.h
SOURCES += $$PWD/tcpsendfilethread.cpp
