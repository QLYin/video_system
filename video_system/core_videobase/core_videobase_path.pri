#区分不同的系统
path_sys = win
win32 {
path_sys = win
}

linux {
path_sys = linux
}

#Qt4套件不认识linux标记
unix:!macx {
path_sys = linux
}

macx {
path_sys = mac
}

android {
path_sys = android
}

#区分不同的位数 x86_64/amd64/arm64/arm64-v8a
path_bit = 32
contains(QT_ARCH, x.*64) {
path_bit = 64
} else:contains(QT_ARCH, a.*64) {
path_bit = 64
} else:contains(QT_ARCH, a.*64.*) {
path_bit = 64
}

#对应系统和位数的库目录
path_lib = lib$$path_sys$$path_bit
#可以重新定义直接指定路径
#path_lib =
#message($$path_lib)

win32 {
LIBS += -luser32
}

unix:!macx:!android {
#下面几行用于编译期间设置可执行文件rpath路径(方便查找库)
#查看rpath命令 readelf -d xxx | grep 'RPATH'
#如果编译提示 -disable-new-dtags: unknown option 则把下面的 ,-disable-new-dtags 去掉即可
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\',-disable-new-dtags"
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib\',-disable-new-dtags"
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/$$path_lib\',-disable-new-dtags"
}
