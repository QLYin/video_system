#ifndef QTHELPERCORE_H
#define QTHELPERCORE_H

#include <QObject>
#include <QRect>
#include <QFont>
#include "globalconfig.h"
class QWidget;

class QtHelperCore
{
public:
    //获取所有屏幕区域/当前鼠标所在屏幕索引/区域尺寸/缩放系数
    static QList<QRect> getScreenRects(bool available = true);
    static int getScreenIndex();
    static QRect getScreenRect(bool available = true);
    static qreal getScreenRatio(int index = -1, bool devicePixel = false);

    //矫正当前鼠标所在屏幕居中尺寸
    static QRect checkCenterRect(QRect &rect, bool available = true);

    //获取桌面宽度高度
    static int deskWidth();
    static int deskHeight();
    static QSize deskSize();

    //居中显示窗体
    //定义标志位指定是以桌面为参照还是主程序界面为参照
    static void setFormInCenter(QWidget *form, QWidget *centerBaseForm = 0);
    static void showForm(QWidget *form);

    //程序文件名称及所在路径
    static QString appName();
    static QString appPath();

    //程序最前面获取应用程序路径和名称
    static void getCurrentInfo(char *argv[], QString &path, QString &name);
    //程序最前面读取配置文件节点的值
    static QString getIniValue(const QString &fileName, const QString &key);
    static QString getIniValue(char *argv[], const QString &key, const QString &dir = QString());

    //获取编译器字符串
    static QString getCompilerString();
    //获取uuid
    static QString getUuid();

    //校验目录(不存在则新建)
    static void checkPath(const QString &dirName);
    //通用延时函数(支持Qt4 Qt5 Qt6)
    static void sleep(int msec, bool exec = true);
    //检查程序是否已经运行
    static void checkRun();

    //设置Qt自带样式
    static void setStyle();
    //设置编码
    static void setCode(bool utf8 = true);
    //设置字体
    static QFont addFont(const QString &fontFile, const QString &fontName);
    static void setFont(const QString &fontFile = ":/font/DroidSansFallback.ttf",
                        const QString &fontName = GlobalConfig::FontName,
                        int fontSize = GlobalConfig::FontSize);
    //设置翻译文件
    static void setTranslator();
    static void setTranslator(const QString &qmFile);

    //动态设置权限
    static bool checkPermission(const QString &permission);
    //申请安卓权限
    static void initAndroidPermission();

    //一次性设置所有包括编码字体翻译等
    static void initAll(bool utf8 = true);
    //初始化main函数最前面执行的一段代码
    static void initMain(bool desktopSettingsAware = false, bool use96Dpi = true, bool logCritical = true);
    //初始化opengl类型(1=AA_UseDesktopOpenGL 2=AA_UseOpenGLES 3=AA_UseSoftwareOpenGL)
    static void initOpenGL(quint8 type = 0, bool checkCardEnable = false, bool checkVirtualSystem = false);

    //执行命令行返回执行结果
    static QString doCmd(const QString &program, const QStringList &arguments, int timeout = 1000);
    //获取显卡是否被禁用
    static bool isVideoCardEnable();
    //获取是否在虚拟机环境
    static bool isVirtualSystem();
};

#endif // QTHELPERCORE_H
