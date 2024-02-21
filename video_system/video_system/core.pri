include ($$PWD/../core_db/core_db.pri)
include ($$PWD/../core_map/core_map.pri)
include ($$PWD/../core_form/core_form.pri)
include ($$PWD/../core_webview/core_webview.pri)
include ($$PWD/../core_tcpfile/core_tcpfile.pri)
include ($$PWD/../core_dataout/core_dataout.pri)
include ($$PWD/../core_common/core_common.pri)
include ($$PWD/../core_control/core_control.pri)
include ($$PWD/../core_qthelper/core_qthelper.pri)

include ($$PWD/../core_onvif/core_onvif.pri)
include ($$PWD/../core_video/core_video.pri)
include ($$PWD/../core_videobase/core_videobase.pri)

#不同的内核加载不同的模块
contains(DEFINES, audiox) {
include ($$PWD/../core_audio/core_audio.pri)
}

contains(DEFINES, openglx) {
include ($$PWD/../core_videoopengl/core_videoopengl.pri)
}

contains(DEFINES, qmedia) {
include ($$PWD/../core_videoqmedia/core_videoqmedia.pri)
}

contains(DEFINES, ffmpeg) {
include ($$PWD/../core_videoffmpeg/core_videoffmpeg.pri)
}

contains(DEFINES, vlcx) {
include ($$PWD/../core_videovlc/core_videovlc.pri)
}

contains(DEFINES, mpvx) {
include ($$PWD/../core_videompv/core_videompv.pri)
}

contains(DEFINES, mdkx) {
include ($$PWD/../core_videomdk/core_videomdk.pri)
}

contains(DEFINES, qtavx) {
include ($$PWD/../core_videoqtav/core_videoqtav.pri)
}

contains(DEFINES, haikang) {
include ($$PWD/../core_videohaikang/core_videohaikang.pri)
}

contains(DEFINES, easyplayer) {
include ($$PWD/../core_videoeasyplayer/core_videoeasyplayer.pri)
}
