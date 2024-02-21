#下面表示版本小于5.0
lessThan(QT_MAJOR_VERSION, 5) {
DEFINES -= openglx
}

#下面表示版本小于5.5
lessThan(QT_MAJOR_VERSION, 6) {
lessThan(QT_MINOR_VERSION, 5) {
DEFINES -= openglx
}}

#Qt6单独提取出来了openglwidgets模块
greaterThan(QT_MAJOR_VERSION, 5) {
QT *= openglwidgets
}

!android {
#表示arm平台构建套件(如果你的板子对应有qopenglwidget模块请自行屏蔽)
contains(QT_ARCH, arm) || contains(QT_ARCH, arm64) {
DEFINES -= openglx
}}
