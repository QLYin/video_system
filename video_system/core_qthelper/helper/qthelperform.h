#ifndef QTHELPERFORM_H
#define QTHELPERFORM_H

#include <QWidget>
#include "qtabout.h"
class QLabel;
class QPushButton;

class QtHelperForm
{
public:
    //全局变量控制是否需要阴影
    static bool formShadow;
    //设置边框阴影
    static void setFormShadow(QWidget *widget, QLayout *layout, const QString &color, int margin, int radius);
    //立即更新所有阴影边框颜色比如换肤的时候需要用到
    static void setFormShadow(const QString &color);

    //设置无边框窗体
    static void setFramelessForm(QWidget *widgetMain,
                                 bool tool = false, bool top = false,
                                 bool menu = true, bool x11 = false);
    static void setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                 QLabel *labIco, QPushButton *btnClose,
                                 bool tool = true, bool top = true,
                                 bool menu = false, bool x11 = true);

    //定义标志位启用系统的还是自定义的对话框
    static bool isCustomUI;
    //弹出自定义窗体
    static int showFormExec(QWidget *widget, const QString &title, bool windowModal = true);

    //通用弹出框
    static int showMessageBox(const QString &text, int type = 0, int timeout = 0, bool exec = false);
    //弹出消息框
    static void showMessageBoxInfo(const QString &text, int timeout = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &text, int timeout = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &text);

    //弹出右下角信息框
    static void showTipBox(const QString &title, const QString &tip, bool fullScreen = false,
                           bool center = true, int timeout = 0);
    //隐藏右下角信息框
    static void hideTipBox();

    //弹出输入框
    static QString showInputBox(const QString &title, int type = 0, int timeout = 0,
                                const QString &placeholderText = QString(), bool pwd = false,
                                const QString &defaultValue = QString());
    //弹出日期选择框
    static int showDateSelect(QString &dateStart, QString &dateEnd, const QString &format = "yyyy-MM-dd");

    //弹出关于对话框
    static void showAboutInfo(const AboutInfo &info, int timeout = 0, bool exec = false);
    //弹出中间提示框
    static void showSplashInfo(const QString &text, int fontSizeMain, int fontSizeSub, int timeout = 0, bool exec = false);
    //隐藏中间提示框
    static void hideSplashInfo();    
};

#endif // QTHELPERFORM_H
