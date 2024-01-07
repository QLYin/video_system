#include "frmmain.h"
#include "ui_frmmain.h"
#include "qthelper.h"
#include "weather.h"
#include "devicehelper.h"
#include "framelesswidget2.h"
#include "..\deviceconnect\tcpclienthelper.h"
#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "frmmain1.h"
#include "frmmodule.h"
#include "frmtimecpu.h"
#include "frmwelcome.h"
#include "frmweather.h"

#include "frmlogout.h"
#include "frmvideo.h"
#include "frmmap.h"
#include "frmdata.h"
#include "frmconfig.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
    this->initLocation();
    this->initLogo();
    this->initTitleInfo();
    this->initDeviceConnect();
    this->configChanged(AppConfig::WeatherCity, AppConfig::WeatherInterval, AppConfig::WeatherStyle);
}

frmMain::~frmMain()
{
    TcpClient::Instance()->uninit();
    delete ui;
}

void frmMain::closeEvent(QCloseEvent *e)
{
    //没有自动登录以及当前非重启状态
    //需要的话可以改成永远弹出退出窗体
    if (!AppConfig::AutoLogin && !AppData::IsReboot) {
        frmLogout::Instance()->show();
        e->ignore();
    } else {
        if (!AppData::IsReboot) {
            AppEvent::Instance()->slot_exitAll();
        }

        //如果不采用暴力退出可能会打印出现 QThread: Destroyed while thread is still running
        //如果在关闭的时候出现崩溃现象,建议启用下面这行代码
        //有些msvc套件暴力退出的时候会提示: 快速异常检测失败,将不会调用异常处理程序
#ifndef Q_CC_MSVC
        exit(0);
#endif
    }
}

void frmMain::showEvent(QShowEvent *e)
{
    setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(e);
    AppEvent::Instance()->slot_changeMainForm(2);
}

bool frmMain::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->widgetTop) {
        //双击标题栏最大化
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnMenu_Max_clicked();
        }
    } else if (watched == ui->labLogoImage) {
        //鼠标按下logo弹出关于窗体
        if (event->type() == QEvent::MouseButtonPress) {
            AboutInfo aboutInfo;
            aboutInfo.title = AppConfig::TitleCn;
            aboutInfo.version = AppData::Version;
            aboutInfo.copyright = AppConfig::Copyright;
            aboutInfo.http = AppConfig::CompanyHttp;
            aboutInfo.tel = AppConfig::CompanyTel;
            QtHelper::showAboutInfo(aboutInfo);
        }
    } else if (watched == ui->labWeather) {
        if (event->type() == QEvent::MouseButtonPress) {
            //鼠标中键按下弹出天气预报网址
            if (qApp->mouseButtons() == Qt::MiddleButton) {
                QDesktopServices::openUrl(QUrl(AppConfig::WeatherHttp));
            }
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            //鼠标左键双击弹出天气设置界面
            if (qApp->mouseButtons() == Qt::LeftButton && AppConfig::EnableWeather) {
                frmWeather *weather = new frmWeather;
                connect(weather, SIGNAL(configChanged(QString, int, int)), this, SLOT(configChanged(QString, int, int)));
                weather->setAttribute(Qt::WA_DeleteOnClose);
                weather->setConfig(AppConfig::WeatherCity, AppConfig::WeatherInterval, AppConfig::WeatherStyle);
                QtHelper::showFormExec(weather, "城市天气配置", false);
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
bool frmMain::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool frmMain::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif

{
    if (eventType == "windows_generic_MSG") {
#ifdef Q_OS_WIN
        MSG *msg = static_cast<MSG *>(message);
        //qDebug() << TIMEMS << msg->wParam << msg->message;
        if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST) {
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC) {
            //休眠唤醒后自动打开
            this->showNormal();
        }
#endif
    } else if (eventType == "NSEvent") {
#ifdef Q_OS_MACOS
#endif
    } else if (eventType == "xcb_generic_event_t") {
#ifdef Q_OS_LINUX
#endif
    }
    return false;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#ifdef Q_OS_WIN
bool frmMain::winEvent(MSG *message, long *result)
{
    return nativeEvent("windows_generic_MSG", message, result);
}
#endif
#endif

void frmMain::initForm()
{
    ui->widgetBtn->setProperty("flag", "btnNavTop");
    ui->widgetMain->setProperty("form", true);
    ui->widgetTop->setProperty("form", "title");
    ui->widgetTop->setFixedHeight(AppData::TopHeight);
    ui->widgetTop->installEventFilter(this);
    ui->labLogoImage->installEventFilter(this);
    ui->labWeather->installEventFilter(this);
    this->setGeometry(AppConfig::FormGeometry);

    //设置窗体居中参照窗体
    QtHelper::centerBaseForm = this;
    //设置无边框窗体
    QtHelper::setFramelessForm(this);
    this->setProperty("canMove", false);

    //无边框可拉伸类
    frameless = new FramelessWidget2(this);
    frameless->setWidget(this);

    //设置右上角菜单,图形字体
    //如果发现部分操作系统不显示对应的图标可以自行更改成其他图标
    //GlobalConfig::IconNormal = 0xf067;
    //GlobalConfig::IconMax = 0xf067;
    IconHelper::setIcon(ui->btnMenu_Min, GlobalConfig::IconMin);
    IconHelper::setIcon(ui->btnMenu_Max, GlobalConfig::IconNormal);
    IconHelper::setIcon(ui->btnMenu_Close, GlobalConfig::IconClose, 14);

    //加载右上角信息窗体
    ui->widgetInfo->layout()->addWidget(new frmTimeCpu);
    //ui->widgetInfo->layout()->addWidget(new frmWelcome);

    //调整不同导航样式下的布局间距
    CommonNav::initNavLayout(ui->widgetMenu, ui->layoutNav, false);
}

void frmMain::initWidget()
{
    //关联样式改变信号自动重新设置图标等处理
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initLogo()));
    connect(AppEvent::Instance(), SIGNAL(changeLogo()), this, SLOT(initLogo()));
    connect(AppEvent::Instance(), SIGNAL(changeTitleInfo()), this, SLOT(initTitleInfo()));
    connect(AppEvent::Instance(), SIGNAL(fullScreen(bool)), this, SLOT(fullScreen(bool)));
    connect(AppEvent::Instance(), SIGNAL(mouseButtonRelease()), this, SLOT(mouseButtonRelease()));

    //中间视频监控通道及悬停模块
    frmModule *module = new frmModule;
    ui->stackedWidget->addWidget(module);

    //停靠窗体加载完成发送信号通知生成菜单
    connect(module, SIGNAL(loadModuleFinshed(QList<QString>, QList<bool>)),
            this, SLOT(initAction(QList<QString>, QList<bool>)));
    //模块切换显示隐藏通知主界面更新菜单
    connect(module, SIGNAL(visibilityChangedFromModule(QString, bool)),
            this, SLOT(visibilityChangedFromModule(QString, bool)));
    //菜单切换模块显示隐藏通知模块类处理
    connect(this, SIGNAL(visibilityChangedFromMain(QString, bool)),
            module, SLOT(visibilityChangedFromMain(QString, bool)));

    //视频回放
    frmVideo *video = new frmVideo;
    ui->stackedWidget->addWidget(video);

    //电子地图
    frmMap *map = new frmMap;
    ui->stackedWidget->addWidget(map);

    //备用模块
    frmMain1 *main1 = new frmMain1;
    ui->stackedWidget->addWidget(main1);

    //日志记录
    frmData *data = new frmData;
    ui->stackedWidget->addWidget(data);

    //系统设置
    frmConfig *config = new frmConfig;
    ui->stackedWidget->addWidget(config);
}

void frmMain::initNav()
{
    QList<QString> names, texts;
    names << "btnView" << "btnVideo" << "btnMap" << "btnMain1" << "btnData" << "btnConfig";
    texts << "视频监控" << "视频回放" << "电子地图" << AppData::TitleMain1 << "日志查询" << "系统设置";
    icons << 0xe68c << 0xe68d << 0xe695 << AppData::IconMain1 << 0xe699 << 0xe706;

    if (AppConfig::WorkMode == 3) {
        texts.clear();
        texts << "课堂实训" << "视频回放" << "电子地图" << AppData::TitleMain1 << "教学日志" << "系统设置";
    }

    //根据设定实例化导航按钮对象
    for (int i = 0; i < texts.count(); ++i) {
        QToolButton *btn = new QToolButton;
        CommonNav::initNavBtn(btn, names.at(i), texts.at(i), false);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        ui->layoutNav->addWidget(btn);
        btns << btn;
    }

    //自动打开上次的窗体
    btns.first()->setChecked(true);
    int index = names.indexOf(AppConfig::LastFormMain);
    index >= 0 ? btns.at(index)->click() : btns.first()->click();

    //强制改成默认视频界面
    //btns.at(0)->click();
    btns.at(3)->setVisible(AppConfig::EnableMain1);

    //互动课堂不需要电子地图模块
    if (AppConfig::WorkMode == 3) {
        btns.at(2)->setVisible(false);
    }
}

void frmMain::initIcon()
{
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        CommonNav::initNavBtnIcon(btns.at(i), icons.at(i), false);
    }
}

void frmMain::buttonClicked()
{
    //判断是否有对应模块的权限
    QAbstractButton *btn = (QAbstractButton *)sender();
    if (!UserHelper::checkPermission(btn->text())) {
        btn->setChecked(false);
        //切换到永远有权限的页面
        QMetaObject::invokeMethod(btns.at(0), "clicked");
        return;
    }

    //切换到当前窗体
    ui->stackedWidget->setCurrentIndex(btns.indexOf(btn));

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormMain = btn->objectName();
    AppConfig::writeConfig();
}

void frmMain::initLocation()
{
    if (AppConfig::FormMax) {
        AppConfig::FormGeometry = this->geometry();
        AppConfig::writeConfig();

        //全屏和QWebEngineView控件一起会产生右键菜单无法弹出的bug(需要上移一个像素)
        if (AppConfig::FullScreen) {
            QRect rect = QtHelper::getScreenRect(false);
            rect.setY(-1);
            this->setGeometry(rect);
        } else {
            QRect rect = QtHelper::getScreenRect(true);
            this->setGeometry(rect);
        }
    } else {
        this->setGeometry(AppConfig::FormGeometry);
    }

    IconHelper::setIcon(ui->btnMenu_Max, AppConfig::FormMax ? GlobalConfig::IconMax : GlobalConfig::IconNormal);
    frameless->setMoveEnable(!AppConfig::FormMax);
    frameless->setResizeEnable(!AppConfig::FormMax);
}

void frmMain::initLogo()
{
    QtHelper::setLogo(ui->labLogoImage, AppConfig::LogoImage, 50, AppData::TopHeight - 15, 20, "#000000", GlobalConfig::TextColor);
}

void frmMain::initTitleInfo()
{
    //把右上角信息的隐藏开关也放到这里懒得再开个信号
    ui->widgetInfo->setVisible(AppConfig::RightInfo);

    //如果中文标题有换行则替换成正确的换行并隐藏英文标题
    QString titleCn = AppConfig::TitleCn;
    if (titleCn.contains("\\n")) {
        titleCn.replace("\\n", "\n");
        ui->labTitleEn->setVisible(false);
    } else {
        ui->labTitleEn->setVisible(true);
    }

    //设置中英文标题
    ui->labTitleCn->setText(titleCn);
    ui->labTitleEn->setText(AppConfig::TitleEn);
    this->setWindowTitle(ui->labTitleCn->text());
}

void frmMain::initDeviceConnect()
{
    TcpClient::Instance()->init();
    connect(TcpClient::Instance(), &TcpClient::socketConnected, this, []()
        {
            AppEvent::Instance()->slot_tcpConnected();
            TcpClientHelper::sendUnlockDevice();
            //TcpClientHelper::sendDataSync(1);
            //TcpClientHelper::sendDataSync(4);
            //TcpClientHelper::sendSceneInfo();
            TcpClientHelper::sendWallSet();
        });

    connect(TcpClient::Instance(), &TcpClient::socketData, this, [](const QVariantMap data)
        {
            AppEvent::Instance()->slot_tcpSockectData(data);
            qDebug() << ">>>>>>>>>> data: " << data;
        });
}

void frmMain::fullScreen(bool full)
{
    QtHelper::sleep(100);
    if (full) {
        if (!AppConfig::FormMax) {
            on_btnMenu_Max_clicked();
        }

        ui->widgetTop->setVisible(false);
    } else {
        ui->widgetTop->setVisible(true);
    }
}

void frmMain::mouseButtonRelease()
{
    //Qt史上最古老的的bug,部分控件有鼠标按下但是不会将 MouseButtonRelease 传递给父类
    frameless->setMousePressed(false);
}

void frmMain::initAction(const QList<QString> &titles, const QList<bool> &visibles)
{
    if (!AppConfig::EnableTitleMenu) {
        return;
    }

    //设置右键菜单模式
    ui->widgetTop->setContextMenuPolicy(Qt::ActionsContextMenu);

    //先移除之前的菜单
    QList<QAction *> actions = ui->widgetTop->actions();
    foreach (QAction *action, actions) {
        ui->widgetTop->removeAction(action);
        action->deleteLater();
    }

    //循环根据模块名称生成对应的菜单
    int count = titles.count();
    for (int i = 0; i < count; ++i) {
        QString text = QString("%1%2").arg(visibles.at(i) ? "隐藏" : "显示").arg(titles.at(i));
        QAction *action = new QAction(text, this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(doAction()));
        ui->widgetTop->addAction(action);
    }

    //增加其他菜单
    QStringList listAction;
    listAction << "显示所有模块" << "隐藏所有模块" << "保存当前布局" << "复位普通布局" << "复位全屏布局";
    foreach (QString text, listAction) {
        QAction *action = new QAction(text, this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(doAction()));
        ui->widgetTop->addAction(action);
    }

    //下面演示如何通过代码针对单个模块显示或者隐藏
    //emit visibilityChangedFromMain("窗口信息", false);
    //emit visibilityChangedFromMain("设备列表", true);
    //qDebug() << TIMEMS << titles << visibles;
}

void frmMain::doAction()
{
    //根据不同的菜单名字执行对应的动作
    //执行完不用更新菜单名称,对面会自动触发可见信号改变执行下面的 visibilityChangedFromModule 来改变
    QAction *action = (QAction *)sender();
    QString text = action->text();
    QString title = text.mid(2, text.length());
    bool visible = text.startsWith("显示");
    emit visibilityChangedFromMain(title, visible);
}

void frmMain::visibilityChangedFromModule(const QString &title, bool visible)
{
    //根据传过来的不同的模块名称更新对应菜单的名字
    QList<QAction *> actions = ui->widgetTop->actions();
    foreach (QAction *action, actions) {
        QString text = action->text();
        QString flag = text.mid(2, text.length());
        if (flag == title) {
            text = QString("%1%2").arg(visible ? "隐藏" : "显示").arg(title);
            action->setText(text);
            break;
        }
    }
}

void frmMain::configChanged(const QString &city, int interval, int style)
{
    AppConfig::WeatherCity = city;
    AppConfig::WeatherInterval = interval;
    AppConfig::WeatherStyle = style;
    AppConfig::writeConfig();

    //设置天气
    static Weather *weather = 0;
    if (weather == 0) {
        weather = new Weather(this);
    }

    weather->setCity(AppConfig::WeatherCity);
    weather->setLabel(ui->labWeather, AppConfig::WeatherStyle);
    weather->start(AppConfig::WeatherInterval);
    weather->query();
}

void frmMain::on_btnMenu_Min_clicked()
{
#ifdef Q_OS_MACOS
    this->setWindowFlags(this->windowFlags() & ~Qt::FramelessWindowHint);
#endif
    this->showMinimized();
    AppEvent::Instance()->slot_changeMainForm(0);
}

void frmMain::on_btnMenu_Max_clicked()
{
    AppConfig::FormMax = !AppConfig::FormMax;
    AppConfig::writeConfig();
    this->initLocation();

    //最大化以后有个bug,悬停样式没有取消掉,需要主动模拟鼠标动一下
    if (AppConfig::FormMax) {
        QEvent event(QEvent::Leave);
        QApplication::sendEvent(ui->btnMenu_Max, &event);
    }
}

void frmMain::on_btnMenu_Close_clicked()
{
    //退出前保存最后的窗体坐标+宽高
    if (!AppConfig::FormMax) {
        AppConfig::FormGeometry = this->geometry();
        AppConfig::writeConfig();
    }

    this->close();
}
