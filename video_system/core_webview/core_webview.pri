include ($$PWD/webcore.pri)

#定义了savehtmlfile则表示网页会保存成本地文件
#一般linux上需要保存成本地文件不然加载不了
win32 {DEFINES += savehtmlfile1}
unix  {DEFINES += savehtmlfile}

#带 webkitwidgets 模块的只能通过文件形式调用
contains(QT, webkitwidgets) {
DEFINES += savehtmlfile
}

#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

HEADERS += $$PWD/webjsdata.h
HEADERS += $$PWD/webview.h

SOURCES += $$PWD/webjsdata.cpp
SOURCES += $$PWD/webview.cpp

#引入miniblink浏览器内核类
contains(DEFINES, webminiblink) {
HEADERS += $$PWD/wke.h
HEADERS += $$PWD/miniblink.h
SOURCES += $$PWD/miniblink.cpp
}

