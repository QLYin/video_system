QT += core gui sql network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

#开启预编译头可以加快编译速度(部分构建套件环境不支持/编译不通过可以注释下面这行试试)
PRECOMPILED_HEADER = head.h

#开启运行文件附带调试输出窗口
CONFIG  += console1
#禁用qdebug打印输出
DEFINES += QT_NO_DEBUG_OUTPUT1
#开启release模式输出日志带行号等信息
DEFINES += QT_MESSAGELOGCONTEXT
#去掉串口模块(目前监控系统没用上/备用)
DEFINES += nocom

#引入音频模块audio(为了避免和变量冲突后面加上x)
DEFINES += audiox
#引入绘制模块opengl(采用GPU绘制)
DEFINES += openglx

#启用内核ffmpeg(如果要支持xp只能用ffmpeg2/ffmpeg3)
DEFINES += ffmpeg ffmpeg4 videoffmpeg
DEFINES += qmedia1 vlcx1 mpvx1 mdkx1 qtavx1 haikang1 easyplayer1

#启用百度地图
DEFINES += mapbaidu
#启用换肤
DEFINES += quistyle

TARGET      = video_system
TEMPLATE    = app
RC_FILE     = qrc/main.rc

HEADERS     += head.h
SOURCES     += main.cpp
RESOURCES   += qrc/main.qrc

INCLUDEPATH += $$PWD
include ($$PWD/3rd.pri)
include ($$PWD/core.pri)
include ($$PWD/ui/ui.pri)
include ($$PWD/class/class.pri)

#如果要去掉浏览器内核则开启下面这行即可
#DEFINES -= webkit webengine webminiblink
