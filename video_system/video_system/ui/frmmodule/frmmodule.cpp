#include "frmmodule.h"
#include "ui_frmmodule.h"
#include "qthelper.h"
#include "customtitlebar.h"
#include "frmvideopanel.h"

#include "frmdevicetree.h"
#include "frmdevicegps.h"
#include "frmdevicegps2.h"
#include "frmmsglist.h"
#include "frmmsgtable.h"
#include "frmwebview.h"

#include "frmipcptz.h"
#include "frmipccontrol.h"
#include "frmipcpreset.h"
#include "frmipcpolling.h"

#include "frmrobotdata.h"
#include "frmrobotdebug.h"
#include "frmrobotdebug2.h"
#include "frmrobotemulate.h"

frmModule::frmModule(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmModule)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->addWidget();
}

frmModule::~frmModule()
{
    delete ui;
}

void frmModule::showEvent(QShowEvent *)
{
    //首次显示的时候加载布局
    static bool isLoad = false;
    if (!isLoad) {
        isLoad = true;
        //先设置下默认尺寸
        this->initSize();

        //初始化布局(如果希望显示后就立马切换到布局可以用下面这行)
        //QMetaObject::invokeMethod(this, "initLayout", Qt::QueuedConnection);
        QTimer::singleShot(100, this, SLOT(initLayout()));
        QTimer::singleShot(1000, this, SLOT(changeWindowOpacity()));
        return;
    }

    //再次显示的时候将刚才暂时隐藏的浮动窗体显示
    foreach (QDockWidget *dockWidget, hideWidgets) {
        dockWidget->setVisible(true);
    }
}

void frmModule::hideEvent(QHideEvent *)
{
    //记住所有可见浮动的窗体有哪些,切换到其他页面需要暂时隐藏
    hideWidgets.clear();
    foreach (QDockWidget *dockWidget, dockWidgets) {
        if (dockWidget->isVisible() && dockWidget->isFloating()) {
            dockWidget->setVisible(false);
            hideWidgets << dockWidget;
        }
    }
}

void frmModule::initForm()
{
    //程序退出的时候保存布局到配置文件
    connect(AppEvent::Instance(), SIGNAL(exitAll()), this, SLOT(saveLayout()));
    //系统设置中透明度值改变立即应用
    connect(AppEvent::Instance(), SIGNAL(changeWindowOpacity()), this, SLOT(changeWindowOpacity()));
    //全屏切换自动切换对应的布局
    connect(AppEvent::Instance(), SIGNAL(fullScreen(bool)), this, SLOT(fullScreen(bool)));
}

void frmModule::initSize()
{
    if (dockWidths.count() == 0 || dockHeights.count() == 0) {
        return;
    }
    if (dockWidths.count() != dockHeights.count()) {
        return;
    }
    if (dockWidths.count() != dockWidgets.count()) {
        return;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    this->resizeDocks(dockWidgets, dockWidths, Qt::Horizontal);
    this->resizeDocks(dockWidgets, dockHeights, Qt::Vertical);
#endif
}

void frmModule::initMenu()
{
    //将子模块的名称集合以及可见状态发给主界面生成右键菜单
    QList<QString> titles;
    QList<bool> visibles;
    foreach (QDockWidget *dockWidget, dockWidgets) {
        titles << dockWidget->windowTitle();
        visibles << dockWidget->isVisible();
    }
    emit loadModuleFinshed(titles, visibles);
}

void frmModule::initWidget()
{
    //实例化视频监控通道画面
    frmVideoPanel *videoPanel = new frmVideoPanel;
    AppData::videoPanel = videoPanel;
    videoPanel->setObjectName("centralWidget_frmVideoPanel");

    //设置中心窗体
    this->setCentralWidget(videoPanel);
    //设置停靠参数,不允许重叠,只允许拖动
    //this->setDockOptions(QMainWindow::AnimatedDocks);

    //清空列表
    this->dockWidgets.clear();
    this->dockWidths.clear();
    this->dockHeights.clear();

    //停靠窗体默认尺寸 绝大部分模块都按照这个尺寸来
    int width = 220;
    int height = 500;

    //根据不同的工作模式加载不同的模块
    //可以自行更改对应的名称
    if (AppConfig::WorkMode == 1) {
        frmDeviceGps *deviceGps = new frmDeviceGps;
        frmRobotData *robotData = new frmRobotData;
        connect(robotData, SIGNAL(moveDevice(int, QString, QString)),
                deviceGps, SLOT(moveDevice(int, QString, QString)));

        newWidget(new frmDeviceTree, "设备列表", width, height);
        newWidget(new frmMsgTable, "窗口信息", width, 200);

        newWidget(deviceGps, "设备轨迹", width, 200);
        newWidget(robotData, "仿真数据", width, height);
        newWidget(new frmRobotDebug2, "数据调试", width, height);
        newWidget(new frmRobotEmulate, "运动仿真", width, height);
    } else if (AppConfig::WorkMode == 2) {
        newWidget(new frmMsgList, "图文警情", width, height);
        newWidget(new frmMsgTable, "窗口信息", width, 200);

        newWidget(new frmDeviceGps2, "飞行轨迹", width, 250);
        newWidget(new frmIpcPtz, "云台控制", width, height);
        newWidget(new frmDeviceTree, "设备列表", width, height);
    } else {
        newWidget(new frmMsgList, "图文警情", width, height);
        newWidget(new frmMsgTable, "窗口信息", width, height);
        newWidget(new frmDeviceGps, "悬浮地图", width, 120);
        newWidget(new frmDeviceTree, "设备列表", width, height);

        QString name = (AppConfig::WorkMode == 3 ? "实训操作" : "云台控制");
        newWidget(new frmIpcPtz, name, width, height);
        newWidget(new frmIpcControl, "设备控制", width, height);
        newWidget(new frmIpcPreset, "预置巡航", width, height);

        frmIpcPolling *ipcPolling = new frmIpcPolling;
        connect(videoPanel, SIGNAL(pollStart()), ipcPolling, SLOT(on_btnPollStart_clicked()));
        newWidget(ipcPolling, "视频轮询", width, height);

        //如果开启了自动轮询并且上次处于轮询状态则立即轮询
        //否则自动播放最后的视频地址集合
        if (AppConfig::AutoPoll && AppConfig::Polling) {
            AppConfig::Polling = false;
            QTimer::singleShot(1000, ipcPolling, SLOT(on_btnPollStart_clicked()));
        } else {
            AppConfig::Polling = false;
            QTimer::singleShot(1000, videoPanel, SLOT(play_video_all()));
        }
    }

#if 0
    //演示加载网页浏览模块
    frmWebView *webView = new frmWebView;
    //webView->resize(400, 300);
    //webView->setUrl("http://www.qtcn.org/");
    webView->setUrl("https://threejs.org/examples/?q=fbx#webgl_loader_fbx");
    QDockWidget *dockWebView = newWidget(webView, "网页浏览", width, height);
    dockWebView->setFloating(true);
    //设置当前子模块固定尺寸
    dockWebView->setFixedSize(1000, 600);
    dockWebView->setProperty("FixedSize", true);
    //加入到停靠布局中
    addWidget(dockWebView, 0);
#endif

#if 0
    //演示独立的视频监控窗体,多屏幕
    frmVideoPanel *videoPanel2 = new frmVideoPanel;
    videoPanel2->resize(400, 300);
    QDockWidget *dockVideoPanel = newWidget(videoPanel2, "图像监控", width, height);
    //加入到停靠布局中
    addWidget(dockVideoPanel, 0);
#endif

    //设置允许各种嵌套比如上下排列左右排列非常灵活
    //此设置会和下面的 setDockOptions 中的参数覆盖所以要注意顺序
    //this->setDockNestingEnabled(true);

    //设置停靠参数,不允许重叠,只允许拖动和嵌套
    //this->setDockOptions(AnimatedDocks | AllowNestedDocks);

    //将底部左侧作为左侧区域,底部右侧作为右侧区域,否则底部区域会填充拉伸
    //this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    //this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

QDockWidget *frmModule::newWidget(QWidget *widget, const QString &title, int width, int height)
{
    //设置拉伸策略
    //widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //实例化停靠窗体
    QString objName = widget->objectName();
    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setObjectName("dockWidget_" + objName);
    dockWidget->setWindowTitle(title);
    dockWidget->setWidget(widget);

    //禁用弹出菜单
    if (!AppConfig::EnableModelMenu) {
        dockWidget->setContextMenuPolicy(Qt::PreventContextMenu);
    }

    //自定义停靠窗体标题栏
    CustomTitleBar *titleBar = new CustomTitleBar;
    titleBar->setObjectName("titleBar_" + objName);
    titleBar->setFull(false);
    titleBar->setTitle(title);
    dockWidget->setTitleBarWidget(titleBar);
    connect(dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));

    //没有权限不能对停靠窗体做任何动作
    if (!UserHelper::checkPermission("调整布局")) {
        dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    }

    //设置只允许左侧和右侧停靠
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //设置顶部不可停靠
    //dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    dockWidgets << dockWidget;
    dockWidths << width;
    dockHeights << height;
    return dockWidget;
}

void frmModule::addWidget()
{
    //根据不同的工作模式调整模块的位置
    //下面的居然有顺序要求才能应用透明度,貌似要从右到左,妹的不知道怎么回事
    if (AppConfig::WorkMode == 1) {
        //添加右侧窗体
        addWidget(2, 1);
        addWidget(3, 1);
        addWidget(4, 1);
        addWidget(5, 1);

        //添加左侧窗体
        addWidget(0, 0);
        addWidget(1, 0);

        //合并窗体形成选项卡
        this->tabifyDockWidget(dockWidgets.at(3), dockWidgets.at(4));
        this->tabifyDockWidget(dockWidgets.at(4), dockWidgets.at(5));

        //主动切换到对应窗体选项卡
        dockWidgets.at(3)->raise();
    } else if (AppConfig::WorkMode == 2) {
        //添加右侧窗体
        addWidget(2, 1);
        addWidget(3, 1);
        addWidget(4, 1);

        //添加左侧窗体
        addWidget(0, 0);
        addWidget(1, 0);
    } else {
        //添加左侧窗体
        addWidget(2, 0);
        addWidget(0, 0);
        addWidget(1, 0);
        addWidget(7, 0);

        //添加右侧窗体
        addWidget(3, 1);
        addWidget(4, 1);
        addWidget(5, 1);
        addWidget(6, 1);

        //合并窗体形成选项卡(0-1-7模块一起/4-5-6模块一起)
        this->tabifyDockWidget(dockWidgets.at(0), dockWidgets.at(1));
        this->tabifyDockWidget(dockWidgets.at(1), dockWidgets.at(7));
        this->tabifyDockWidget(dockWidgets.at(4), dockWidgets.at(5));
        this->tabifyDockWidget(dockWidgets.at(5), dockWidgets.at(6));

        //主动切换到对应窗体选项卡(默认切换选中0/3/5模块)
        dockWidgets.at(0)->raise();
        dockWidgets.at(4)->raise();

        //高度不够默认隐藏一些模块(可以手动在标题栏右键开启)
        if (QtHelper::deskHeight() <= 800) {
            dockWidgets.at(6)->hide();
            dockWidgets.at(7)->hide();
        }
    }
}

void frmModule::addWidget(int index, int position)
{
    if (index < dockWidgets.count()) {
        addWidget(dockWidgets.at(index), position);
    }
}

void frmModule::addWidget(QDockWidget *widget, int position)
{
    //设置停靠位置
    Qt::DockWidgetArea area;
    if (position == 0) {
        area = Qt::LeftDockWidgetArea;
    } else if (position == 1) {
        area = Qt::RightDockWidgetArea;
    } else if (position == 2) {
        area = Qt::TopDockWidgetArea;
    } else if (position == 3) {
        area = Qt::BottomDockWidgetArea;
    } else if (position == 4) {
        area = Qt::AllDockWidgetAreas;
    } else {
        area = Qt::NoDockWidgetArea;
    }

    this->addDockWidget(area, widget);
}

void frmModule::changeWindowOpacity()
{
    foreach (QDockWidget *dockWidget, dockWidgets) {
        dockWidget->setWindowOpacity((qreal)AppConfig::WindowOpacity / 100);
    }
}

void frmModule::fullScreen(bool full)
{
    //先保存原来的布局再加载新的布局
    this->saveLayout(!AppConfig::FormFull);
    QTimer::singleShot(200, this, SLOT(fullScreen()));
}

void frmModule::fullScreen()
{
    this->initLayout(AppConfig::FormFull);
}

QString frmModule::getLayoutIni(bool full)
{
    QString flag = full ? "full" : "normal";
    QString file = QString("%1/layout/video_workmode%2_%3.ini").arg(QtHelper::appPath()).arg(AppConfig::WorkMode).arg(flag);
    return file;
}

void frmModule::initLayout(bool full)
{
    //调用Qt自己的函数 restoreState 来加载布局
    //不同的工作模式对应不同的布局文件,区分全屏和正常状态
    QString file = getLayoutIni(full);
    QByteArray data = AppConfig::readLayout(file);
    this->restoreState(data);

    //生成停靠窗体菜单
    this->initMenu();
}

void frmModule::saveLayout(bool full)
{
    //当前页面不可见不用处理
    if (!this->isVisible()) {
        return;
    }

    //调用Qt自己的函数 saveState 来保存布局
    //不同的工作模式对应不同的布局文件,区分全屏和正常状态
    QString file = getLayoutIni(full);
    QByteArray data = this->saveState();
    AppConfig::writeLayout(file, data);
}

void frmModule::resetLayout(bool full)
{
    //删除布局配置文件并重启(重启后会按照默认值自动生成新的布局配置文件)
    QFile(getLayoutIni(full)).remove();
    FormHelper::reboot();
}

void frmModule::visibilityChanged(bool visible)
{
    //参数中的 visible 没有用如果该模块在tab中并且不是当前tab
    QDockWidget *dockWidget = (QDockWidget *)sender();
    emit visibilityChangedFromModule(dockWidget->windowTitle(), dockWidget->isVisible());
}

void frmModule::visibilityChangedFromMain(const QString &title, bool visible)
{
    //当前模块不可见则不用处理
    if (!this->isVisible()) {
        return;
    }

    //根据传过来的不同菜单标题进行处理
    if (title.endsWith("当前布局")) {
        //保存当前布局
        this->saveLayout();
    } else if (title.endsWith("所有模块")) {
        //隐藏所有模块
        foreach (QDockWidget *dockWidget, dockWidgets) {
            dockWidget->setVisible(visible);
        }
    } else if (title.endsWith("普通布局")) {
        //复位普通布局
        resetLayout(false);
    } else if (title.endsWith("全屏布局")) {
        //复位全屏布局
        resetLayout(true);
    } else {
        //找到对应的子模块切换显示隐藏
        foreach (QDockWidget *dockWidget, dockWidgets) {
            if (dockWidget->windowTitle() == title) {
                dockWidget->setVisible(visible);
                break;
            }
        }
    }
}
