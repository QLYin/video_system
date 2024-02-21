#引入多媒体模块
!contains(DEFINES, multimedia) {
greaterThan(QT_MAJOR_VERSION, 4) {
qtHaveModule(multimedia) {
DEFINES *= multimedia multimedia5
QT *= multimedia
}} else {
#Qt4也有这个模块(没有的话可以手动去掉)
DEFINES *= multimedia multimedia4
QT *= multimedia
}}
