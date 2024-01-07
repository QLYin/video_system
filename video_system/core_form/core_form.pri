#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

FORMS += \
    $$PWD/frmconfigdb.ui \
    $$PWD/frmconfiguser.ui \
    $$PWD/frmlogin.ui \
    $$PWD/frmlogout.ui

HEADERS += \
    $$PWD/formhelper.h \
    $$PWD/frmconfigdb.h \
    $$PWD/frmconfiguser.h \
    $$PWD/frmlogin.h \
    $$PWD/frmlogout.h \
    $$PWD/userhelper.h

SOURCES += \
    $$PWD/formhelper.cpp \
    $$PWD/frmconfigdb.cpp \
    $$PWD/frmconfiguser.cpp \
    $$PWD/frmlogin.cpp \
    $$PWD/frmlogout.cpp \
    $$PWD/userhelper.cpp
