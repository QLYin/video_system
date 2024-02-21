#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
HEADERS += $$PWD/onvifhead.h
HEADERS += $$PWD/onvifstruct.h

HEADERS += $$PWD/onvifbase.h
SOURCES += $$PWD/onvifbase.cpp

HEADERS += $$PWD/onvifdevice.h
SOURCES += $$PWD/onvifdevice.cpp

HEADERS += $$PWD/onvifevent.h
SOURCES += $$PWD/onvifevent.cpp

HEADERS += $$PWD/onvifhelper.h
SOURCES += $$PWD/onvifhelper.cpp

HEADERS += $$PWD/onvifother.h
SOURCES += $$PWD/onvifother.cpp

HEADERS += $$PWD/onvifptz.h
SOURCES += $$PWD/onvifptz.cpp

HEADERS += $$PWD/onvifquery.h
SOURCES += $$PWD/onvifquery.cpp

HEADERS += $$PWD/onvifrequest.h
SOURCES += $$PWD/onvifrequest.cpp

HEADERS += $$PWD/onvifsearch.h
SOURCES += $$PWD/onvifsearch.cpp

HEADERS += $$PWD/onvifsnap.h
SOURCES += $$PWD/onvifsnap.cpp

HEADERS += $$PWD/onvifthread.h
SOURCES += $$PWD/onvifthread.cpp

HEADERS += $$PWD/onvifvideo.h
SOURCES += $$PWD/onvifvideo.cpp

HEADERS += $$PWD/onvifxml.h
SOURCES += $$PWD/onvifxml.cpp

RESOURCES += $$PWD/onvif.qrc
