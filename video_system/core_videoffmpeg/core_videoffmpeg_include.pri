#如果没有定义任何版本则默认是ffmpeg4
#如果要支持xp只能用ffmpeg2/ffmpeg3
!contains(DEFINES, ffmpeg2) {
!contains(DEFINES, ffmpeg3) {
!contains(DEFINES, ffmpeg4) {
!contains(DEFINES, ffmpeg5) {
!contains(DEFINES, ffmpeg6) {
DEFINES *= ffmpeg4
}}}}}

#区分主目录
contains(DEFINES, ffmpeg6) {
path_main = ffmpeg6
} else:contains(DEFINES, ffmpeg5) {
path_main = ffmpeg5
} else:contains(DEFINES, ffmpeg4) {
path_main = ffmpeg4
} else:contains(DEFINES, ffmpeg3) {
path_main = ffmpeg3
} else {
path_main = ffmpeg2
}

#库完整目录
path_full = $$PWD/$$path_main/$$path_lib
#包含头文件
INCLUDEPATH += $$PWD/$$path_main/include

#在某些环境下可能如果报错的话可能还需要主动链接一些库 -lm -lz -lbz2 -lrt -ld -llzma -lvdpau -lX11 -lx264 等
#具体报错提示自行搜索可以找到答案(增加需要链接的库即可)
#链接库文件(编译好以后需要将动态库文件复制到可执行文件下的lib目录才能正常运行)
LIBS += -L$$path_full/ -lavformat -lavfilter -lavcodec -lswresample -lswscale -lavutil

#如果系统环境变量中能够找到库则可以用下面的方式(uos系统环境中有ffmpeg库)
#LIBS += -L/ -lavformat -lavfilter -lavcodec -lswresample -lswscale -lavutil

#引入本地设备(linux上如果是静态的ffmpeg库则需要去掉)
DEFINES *= ffmpegdevice

android {
DEFINES -= ffmpegdevice
#默认提供的是ffmpeg4对应的动态库(如果是静态库则不用下面这些打包)
ANDROID_EXTRA_LIBS += $$path_full/libavformat.so
ANDROID_EXTRA_LIBS += $$path_full/libavfilter.so
ANDROID_EXTRA_LIBS += $$path_full/libavcodec.so
ANDROID_EXTRA_LIBS += $$path_full/libswresample.so
ANDROID_EXTRA_LIBS += $$path_full/libswscale.so
ANDROID_EXTRA_LIBS += $$path_full/libavutil.so
ANDROID_EXTRA_LIBS += $$path_full/libfdk-aac.so
ANDROID_EXTRA_LIBS += $$path_full/libx264.so
LIBS += -L$$path_full/ -lx264 #-lx265
}

#引入本地设备库
contains(DEFINES, ffmpegdevice) {
LIBS += -L$$path_full/ -lavdevice
}

#指定依赖路径
linux:!android {
LIBS += -Wl,-rpath,$$path_full
#存在264/265的库则引入(编码才会用到)
exists($$path_full/libx264.*) {
LIBS += -L$$path_full/ -lpostproc -lx264 -lx265
}}
