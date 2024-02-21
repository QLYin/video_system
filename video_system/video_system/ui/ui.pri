#定制--机器人模块哦
INCLUDEPATH += $$PWD/frmrobot
include ($$PWD/frmrobot/frmrobot.pri)

INCLUDEPATH += $$PWD/frmmap
INCLUDEPATH += $$PWD/frmipc
INCLUDEPATH += $$PWD/frmdata
INCLUDEPATH += $$PWD/frmdemo
INCLUDEPATH += $$PWD/frmmain
INCLUDEPATH += $$PWD/frmvideo
INCLUDEPATH += $$PWD/frmconfig
INCLUDEPATH += $$PWD/frmmodule

include ($$PWD/frmmap/frmmap.pri)
include ($$PWD/frmipc/frmipc.pri)
include ($$PWD/frmdata/frmdata.pri)
include ($$PWD/frmdemo/frmdemo.pri)
include ($$PWD/frmmain/frmmain.pri)
include ($$PWD/frmvideo/frmvideo.pri)
include ($$PWD/frmconfig/frmconfig.pri)
include ($$PWD/frmmodule/frmmodule.pri)
