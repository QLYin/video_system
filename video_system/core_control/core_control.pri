#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

RESOURCES += \
    $$PWD/control.qrc

HEADERS += \
    $$PWD/bottomwidget.h \
    $$PWD/colorcombobox.h \
    $$PWD/cpumemorylabel.h \
    $$PWD/customtitlebar.h \
    $$PWD/devicebutton.h \
    $$PWD/lcddatetime.h \
    $$PWD/panelwidget.h \
    $$PWD/switchbutton.h \
    $$PWD/xlineedit.h \
    $$PWD/xslider.h

SOURCES += \
    $$PWD/bottomwidget.cpp \
    $$PWD/colorcombobox.cpp \
    $$PWD/cpumemorylabel.cpp \
    $$PWD/customtitlebar.cpp \
    $$PWD/devicebutton.cpp \
    $$PWD/lcddatetime.cpp \
    $$PWD/panelwidget.cpp \
    $$PWD/switchbutton.cpp \
    $$PWD/xlineedit.cpp \
    $$PWD/xslider.cpp
