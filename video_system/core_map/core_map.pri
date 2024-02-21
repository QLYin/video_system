#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

contains(DEFINES, echarts) {
HEADERS += $$PWD/echartjs.h
SOURCES += $$PWD/echartjs.cpp

HEADERS += $$PWD/echarts.h
SOURCES += $$PWD/echarts.cpp
}

contains(DEFINES, mapdownload) {
DEFINES += needmapbaidu

HEADERS += $$PWD/mapgaode.h
SOURCES += $$PWD/mapgaode.cpp

HEADERS += $$PWD/maptengxun.h
SOURCES += $$PWD/maptengxun.cpp

HEADERS += $$PWD/maptian.h
SOURCES += $$PWD/maptian.cpp

HEADERS += $$PWD/mapdownhelper.h
SOURCES += $$PWD/mapdownhelper.cpp

HEADERS += $$PWD/mapdownthread.h
SOURCES += $$PWD/mapdownthread.cpp

HEADERS += $$PWD/mapdownload.h
SOURCES += $$PWD/mapdownload.cpp
}

contains(DEFINES, mapboundary) {
DEFINES += needmapbaidu
HEADERS += $$PWD/cityhelper.h
SOURCES += $$PWD/cityhelper.cpp
}

contains(DEFINES, mapbaidu) {
DEFINES += needmapbaidu
}

contains(DEFINES, mapmarker) {
DEFINES += needmapbaidu
}

contains(DEFINES, mapmarkers) {
DEFINES += needmapbaidu
}

contains(DEFINES, mapgps) {
DEFINES += needmapbaidu
}

#去除多余的不然会有多个 needmapbaidu 定义
contains(DEFINES, needmapbaidu) {
DEFINES -= needmapbaidu
DEFINES += needmapbaidu
}

#多个模块都采用的是百度地图类
contains(DEFINES, needmapbaidu) {
HEADERS += $$PWD/mapbaidu.h
SOURCES += $$PWD/mapbaidu.cpp

HEADERS += $$PWD/maphelper.h
SOURCES += $$PWD/maphelper.cpp
}
