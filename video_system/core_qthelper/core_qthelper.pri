QT *= network
greaterThan(QT_MAJOR_VERSION, 4) {
lessThan(QT_MAJOR_VERSION, 6) {
android {QT *= androidextras}
} else {
QT *= core-private
}}

#指定编译产生的文件分门别类放到对应目录
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj

#指定生成的目标文件存放位置
!android:!ios {
DESTDIR = $$PWD/../bin
}

#把所有警告都关掉眼不见为净
CONFIG += warn_off
#开启大资源支持
CONFIG += resources_big
#开启后会将打印信息用控制台输出
#CONFIG += console
#开启后不会生成空的 debug release 目录
#CONFIG -= debug_and_release

#引入全志H3芯片依赖(不需要的用户可以删除)
include ($$PWD/h3.pri)

#引入模块
include ($$PWD/util/util.pri)
include ($$PWD/form/form.pri)
include ($$PWD/helper/helper.pri)

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/util
INCLUDEPATH += $$PWD/form
INCLUDEPATH += $$PWD/helper

#可以指定不加载对应的资源文件
!contains(DEFINES, no_qrc_qss) {
RESOURCES += $$PWD/qrc/qss.qrc
}

!contains(DEFINES, no_qrc_qm) {
RESOURCES += $$PWD/qrc/qm.qrc
}

!contains(DEFINES, no_qrc_font) {
RESOURCES += $$PWD/qrc/font.qrc
}

wasm {
RESOURCES += $$PWD/qrc/wasm.qrc
}
