#include "appinit.h"
#include "appstyle.h"
#include "commonkey.h"
#include "qthelper.h"
#include "dbquery.h"
#include "devicemap.h"
#include "dbconnthread.h"
#include "frmconfigdb.h"

SINGLETON_IMPL(AppInit)
AppInit::AppInit(QObject *parent) : QObject(parent)
{
}

bool AppInit::eventFilter(QObject *watched, QEvent *event)
{
    //负责将按键字符串存入队列
    if (event->type() == QEvent::KeyPress) {
        //记住程序最后的活动时间-包括键盘+鼠标活动
        AppData::LastLiveTime = QDateTime::currentDateTime();
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int keyValue = keyEvent->key();
        if (keyValue == Qt::Key_Escape) {
            emit keyPressed("esc");
            return true;
        } else if (keyEvent->modifiers() & Qt::AltModifier) {
            if (keyValue == Qt::Key_Enter || keyValue == Qt::Key_Return) {
                emit keyPressed("alt+enter");
                return true;
            }
        }
    }

    if (event->type() == QEvent::MouseMove) {
        //记住程序最后的活动时间-包括键盘+鼠标活动
        AppData::LastLiveTime = QDateTime::currentDateTime();
        if (hideCursor) {
            hideCursor = false;
            qApp->restoreOverrideCursor();
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        //识别鼠标松开
        AppEvent::Instance()->slot_mouseButtonRelease();
    } else if (event->type() == QEvent::UngrabMouse) {
        //qchart控件居然鼠标按下是UngrabMouse事件而不是MouseButtonRelease
        AppEvent::Instance()->slot_mouseButtonRelease();
    }

    //以下代码处理无边框窗体可拖动
    QWidget *w = (QWidget *)watched;
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(watched, event);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = mouseEvent->globalPos() - w->pos();
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed) {
            w->move(mouseEvent->globalPos() - mousePoint);
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void AppInit::checkCursor()
{
    //全屏模式下超过规定时间鼠标未动则隐藏光标
    if (AppConfig::FormFull && !hideCursor && AppConfig::TimeHideCursor > 0) {
        QDateTime now = QDateTime::currentDateTime();
        if (AppData::LastLiveTime.secsTo(now) > AppConfig::TimeHideCursor) {
            hideCursor = true;
            qApp->setOverrideCursor(Qt::BlankCursor);
        }
    }
}

void AppInit::start()
{
    //安装全局事件过滤器
    qApp->installEventFilter(this);

    //一次性设置所有包括编码字体翻译等
    QtHelper::initAll();

    //新建目录
    QtHelper::checkPaths("db|map|log|logo|snap|config");

    //载入配置文件
    AppConfig::ConfigFile = QString("%1/config/%2.ini").arg(QtHelper::appPath()).arg("video_system");
    AppConfig::readConfig();
    AppStyle::initStyle();

    //设置左侧+最大化+最小化+关闭 图标
    GlobalConfig::IconMain = 0xea2a;
    GlobalConfig::IconMax = 0xf2d2;
    GlobalConfig::IconNormal = 0xf2d0;
    if (AppConfig::LogoImage.startsWith("icon_")) {
        QString icon = AppConfig::LogoImage.split("_").last();
        GlobalConfig::IconMain = icon.toInt(NULL, 16);
    }

    //初始化杂七杂八
    this->initOther();
    //初始化数据库
    this->initDb();
    //初始化不同工作模式的全局设置
    this->initWorkMode();
    //校验自动登录
    this->autoLogin();
}

void AppInit::initOther()
{
    //校验秘钥文件(key.db是常规秘钥 key.lic是机器码秘钥)
    bool ok1 = true;//CommonKey::checkLicense(QtHelper::appPath() + "/db/key.lic");
    bool ok2 = CommonKey::checkKey(QtHelper::appPath() + "/db/key.db", 110);
    if (!ok1 || !ok2) {
        exit(0);
    }

    //如果秘钥有限制则对应标题强制带上试用版标识
    if (CommonKey::commonKey->keyUseDate || CommonKey::commonKey->keyUseRun || CommonKey::commonKey->keyUseCount) {
        AppConfig::WorkMode = 0;
        AppConfig::TitleCn = "视频监控管理平台试用版";
    } else {
        AppConfig::TitleCn.replace("试用版", "");
    }

    //计算地图宽高
#if 1
    AppData::MapWidth = QtHelper::deskWidth() - AppData::LeftWidth - AppData::RightWidth - 25;
    AppData::MapHeight = QtHelper::deskHeight() - AppData::TopHeight - AppData::BottomHeight - 15;
#else
    AppData::MapWidth = AppConfig::FormGeometry.width() - AppData::LeftWidth - AppData::RightWidth - 25;
    AppData::MapHeight = AppConfig::FormGeometry.height() - AppData::TopHeight - AppData::BottomHeight - 15;
#endif

    AppData::NvrTypes << "常规" << "海康" << "大华" << "宇视" << "其他";
    AppData::IpcTypes << "常规" << "海康" << "大华" << "宇视" << "其他";

    //目录不存在则新建
    QtHelper::checkPath(AppData::VideoNormalPath);
    QtHelper::checkPath(AppData::VideoAlarmPath);
    QtHelper::checkPath(AppData::ImageNormalPath);
    QtHelper::checkPath(AppData::ImageAlarmPath);

    //重新设置完整的目录路径
    AppData::VideoNormalPath = QtHelper::appPath() + "/" + AppData::VideoNormalPath;
    AppData::VideoAlarmPath = QtHelper::appPath() + "/" + AppData::VideoAlarmPath;
    AppData::ImageNormalPath = QtHelper::appPath() + "/" + AppData::ImageNormalPath;
    AppData::ImageAlarmPath = QtHelper::appPath() + "/" + AppData::ImageAlarmPath;

    //载入地图文件名称集合
    AppData::MapPath = QtHelper::appPath() + "/map";
    QStringList filter;
    filter << "*.jpg" << "*.bmp" << "*.png";
    QDir mapPath(AppData::MapPath);
    AppData::MapNames << mapPath.entryList(filter);
    DeviceMap::Instance()->loadMap();

    //载入声音文件名称集合
    AppData::SoundPath = QtHelper::appPath() + "/sound";
    filter.clear();
    filter << "*.wav" << "*.mp3" << "*.mdi";
    QDir soundPath(AppData::SoundPath);
    AppData::SoundNames << soundPath.entryList(filter);

    //设置开机启动
    QtHelper::runWithSystem(AppConfig::AutoRun);

    //启动定时器计算多久用户没有操作过鼠标
    hideCursor = false;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkCursor()));
    timer->start(1000);
}

void AppInit::initDb()
{
    //初始化数据库连接信息结构体数据
    DbInfo dbInfo;
    frmConfigDb::initDbInfo(dbInfo, "qt_sql_default_connection");

    bool ok = true;
    QString dbType = AppConfig::LocalDbType.toUpper();
    if (dbType == "SQLITE") {
        dbInfo.dbName = DbHelper::getDbDefaultFile("video_system");
        if (QFile(dbInfo.dbName).size() <= 4) {
            ok = false;
        }
    }

    //如果打开失败则弹出数据库配置界面进行设置
    DbData::DbLocal = new DbConnThread;
    DbData::DbLocal->setConnInfo(DbHelper::getDbType(dbType), dbInfo);
    if (!ok || !DbData::DbLocal->openDb()) {
        QtHelper::showFormExec(new frmConfigDb, "数据库配置");
        exit(0);
    } else {
        DbData::DbLocal->start();
    }

    DbQuery::loadNvrInfo();
    DbQuery::loadIpcInfo();
    DbQuery::loadPollInfo();
    DbQuery::loadRecordInfo();
    DbQuery::checkDb();

    //查询最大记录数
    DbData::UserLogID = DbHelper::getMaxID("LogInfo", "LogID");
    //DbQuery::addUserLog(100, "测试数据");
}

void AppInit::initWorkMode()
{
    //根据不同的工作模式设定不同的模块可见+初始化变量值等
    if (AppConfig::WorkMode == 1) {
        AppConfig::TitleCn = QString::fromUtf8("机器人监控管理平台");
        AppConfig::TitleEn = "Robot monitoring and management platform";
        AppConfig::LogoImage = "robot_white";
        AppConfig::MapCenter = "116.358539,39.986091";
        AppConfig::MapZoom = 19;

        //特定模式下强制修改通道启用配置
        AppConfig::EnableChannel8 = false;
        AppConfig::EnableChannel9 = false;
        AppConfig::EnableChannel13 = false;
        AppConfig::EnableChannel16 = false;
        AppConfig::EnableChannel25 = false;
        AppConfig::EnableChannel36 = false;
        AppConfig::EnableChannel64 = false;

        //特定模式下强制修改功能模块配置启用禁用
        AppConfig::EnableMain1 = true;
        AppConfig::EnableMapImage = false;
        AppConfig::EnableVideoPlayWeb = false;
        AppConfig::EnableVideoPlayNvr = false;
        AppConfig::EnableVideoPlayImage = false;
        AppConfig::EnableConfigPoll = false;
        AppConfig::EnableConfigRecord = false;
        AppConfig::EnableVideoUpload = false;

        AppData::TitleMain1 = QString::fromUtf8("实时数据");
        AppData::DeviceIconFile = "./device/device_robot.png";
        AppData::DeviceIconFile2 = "./device/device_robot2.png";
        AppData::DeviceIconSize = 25;
        AppData::DeviceIconSize2 = 50;

        AppData::GpsDeviceNames << "机器人";
        AppData::GpsDevicePoints << AppConfig::MapCenter;
        AppData::GpsDeviceColors << "#22A3A9";
    } else if (AppConfig::WorkMode == 2) {
        AppConfig::TitleCn = QString::fromUtf8("无人机智能监控管理平台");
        AppConfig::TitleEn = "UAV monitoring and management platform";

        AppData::DeviceIconFile = "./device/device_uavs.png";
        AppData::DeviceIconFile2 = "./device/device_uavs2.png";
        AppData::DeviceIconSize = 30;
        AppData::DeviceIconSize2 = 60;

        //有多少个就加多少个
        AppData::GpsDeviceNames << "无人机1" << "无人机2";
        AppData::GpsDevicePoints << "121.414736,31.183927" << "121.415086,31.183769";
        AppData::GpsDeviceColors << "#22A3A9" << "#D64D54";
    } else if (AppConfig::WorkMode == 3) {
        //AppConfig::TitleCn = QString::fromUtf8("智慧校园综合管理平台");
        //AppConfig::TitleEn = "Smart Campus Management Platform";
        AppConfig::DefaultChName = "课桌";
        AppConfig::DefaultIpcName = "学生#1";
        AppConfig::DefaultNvrName = "教室#1";

        AppConfig::TreeRtsp = false;
        AppConfig::ChannelBgText = 1;
        AppConfig::EnableMapImage = false;
        AppConfig::EnableConfigUser = false;
        AppConfig::EnableConfigRecord = false;
    } else {
        //AppConfig::EnableMain1 = false;
        AppData::GpsDeviceNames << "测试设备";
        AppData::GpsDevicePoints << AppConfig::MapCenter;
        AppData::GpsDeviceColors << "#22A3A9";
    }

    AppData::GpsDeviceCount = AppData::GpsDeviceNames.count();

    //设置权限对应的文字描述
    UserHelper::PermissionName.clear();
    UserHelper::PermissionName << "系统设置" << "删除记录" << "调整布局" << "视频回放" << "电子地图" << "日志查询" << "用户管理";
}

void AppInit::autoLogin()
{
    UserHelper::loadUserInfo();
    if (AppConfig::AutoLogin) {
        UserHelper::CurrentUserName = AppConfig::LastLoginer;
        UserHelper::getUserInfo();
        DbQuery::addUserLog("用户自动登录");
    }
}
