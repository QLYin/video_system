#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

class FramelessWidget2;
class QAbstractButton;

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    //关闭的时候触发
    void closeEvent(QCloseEvent *);
    //窗体显示的时候触发
    void showEvent(QShowEvent *);
    //事件过滤器识别双击标题栏等处理
    bool eventFilter(QObject *watched, QEvent *event);

    //拦截系统事件用于修复系统休眠后唤醒程序的bug
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif

    //Qt4的写法
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif
#endif

private:
    Ui::frmMain *ui;

    //无边框类用于拉伸大小
    FramelessWidget2 *frameless;

    //导航按钮+图标集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化子窗体
    void initWidget();
    //初始化导航按钮
    void initNav();
    //初始化导航按钮图标
    void initIcon();
    //导航按钮单击事件
    void buttonClicked();

    //加载界面最后的位置
    void initLocation();
    //加载软件图标
    void initLogo();
    //加载标题中文英文
    void initTitleInfo();

    // 初始化设备连接
    void initDeviceConnect();

    //识别全屏切换
    void fullScreen(bool full);
    //全局鼠标右键松开
    void mouseButtonRelease();

    //生成模块菜单+执行菜单处理
    void initAction(const QList<QString> &titles, const QList<bool> &visibles);
    void doAction();
    //停靠窗体显示隐藏信号通知顶部菜单发生变化
    void visibilityChangedFromModule(const QString &title, bool visible);

    //天气预报配置
    void configChanged(const QString &city, int interval, int style);

signals:
    //主界面菜单切换模块隐藏显示
    void visibilityChangedFromMain(const QString &title, bool visible);

private slots:
    //最大化+最小化+关闭按钮事件
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
};

#endif // FRMMAIN_H
