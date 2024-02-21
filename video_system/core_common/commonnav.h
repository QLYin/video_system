#ifndef COMMONNAV_H
#define COMMONNAV_H

#include "head.h"

class CommonNav
{
public:
    //主导航按钮图标左侧
    static bool NavMainLeft;
    //子导航按钮图标左侧
    static bool NavSubLeft;

    //初始化导航布局
    static void initNavLayout(QWidget *widget, QLayout *layout, bool left);
    //初始化导航按钮
    static void initNavBtn(QToolButton *btn, const QString &name, const QString &text, bool left);
    //初始化导航按钮图标
    static void initNavBtnIcon(QAbstractButton *btn, int icon, bool left, int offset = 0);

    //设置图形字体图标
    static void setIconBtn(QAbstractButton *btn, int icon);
    static void setIconBtn(QAbstractButton *btn, int icon, int size, int width, int height, int fontSize = 0);
    //设置对应面板按钮图标
    static void setIconBtn(QWidget *widget, bool like = false);
    static void setIconBtn(QWidget *widget, int size, int width, int height, bool like, int fontSize = 0);
    //传入自定义名称图标集合设置按钮图标
    static void setIconBtn(const QList<QString> &names, const QList<int> &icons,
                           QWidget *widget, int size, int width, int height, bool like, int fontSize = 0);
};

#endif // COMMONNAV_H
